#include <iostream>
#include <stdexcept>
#include <cmath>
#include <fstream>
#include <sstream>

#include <boost/lexical_cast.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/fstream.hpp>

#include "tiler.hpp"

namespace instame {

tiler::tiler(const std::string& path, int tiles, int rep_size) :
	m_x_tiles(0),
	m_y_tiles(0),
	m_tile_size(0),
	m_rep_size(rep_size)
{
	m_image.read(path);

	// calculate tiles amount and dimentions
	size_t width = m_image.columns();
	size_t height = m_image.rows();

	if (width < height) {
		m_tile_size = width / tiles;

		//if (m_tile_size < 30) {
		//	throw std::runtime_error("too many tiles!");
		//}

		m_x_tiles = tiles;
		m_y_tiles = height / m_tile_size;
	}
	else {
		m_tile_size = height / tiles;

		//if (m_tile_size < 30) {
		//	throw std::runtime_error("too many tiles!");
		//}

		m_y_tiles = tiles;
		m_x_tiles = width / m_tile_size;
	}

	// cut source image into tiles
	for (size_t y = 0; y < m_y_tiles; ++y) {
		for (size_t x = 0; x < m_x_tiles; ++x) {
			int x_orig = x * m_tile_size;
			int y_orig = y * m_tile_size;
			
			int tile_width = m_tile_size;
			int tile_height = m_tile_size;

			int width_remainder = width - (x_orig + m_tile_size);
			int height_remainder = height - (y_orig + m_tile_size);

			if (width_remainder > 0 && width_remainder < m_tile_size) {
				tile_width = width - x_orig;
			}

			if (height_remainder > 0 && height_remainder < m_tile_size) {
				tile_height = height - y_orig;
			}

			Magick::Image img(m_image);
			img.crop(Magick::Geometry(tile_width, tile_height, x_orig, y_orig));

			boost::shared_ptr<image> image_ptr(new image(img, m_tile_size, m_rep_size));
			m_tiles.push_back(image_ptr);
		}
	}
}

void
tiler::create_mozaic(const std::string& images_path, const std::string& output_dir) {
	std::string html_header = "<html><head><style>.pictable{width:800px;height:800px;margin: 0 auto 0 auto;border: none;}</style></head><body><table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" class=\"pictable\">";
	std::string html_footer = "</table></body></html>";
	std::string html_file = output_dir + "index.html";

	// open html file, write header
	std::ofstream file(html_file.c_str());
	if (!file.is_open()) {
		throw std::runtime_error("can't open index.html file for writing!");
	}
	file << html_header;

	std::cout << "loading images..." << std::endl;

	// load all images in images_path
	if (boost::filesystem::exists(images_path)) {
		boost::filesystem::directory_iterator end;

		for (boost::filesystem::directory_iterator iter(images_path); iter != end; ++iter) {
			if (!boost::filesystem::is_directory(*iter)) {
				std::stringstream ss;
				ss << iter->path().filename();

				std::string filename = ss.str();
				filename = filename.substr(1, filename.length() - 1);
				filename = filename.substr(0, filename.length() - 1);
				if (filename != ".DS_Store") {

					Magick::Image img(images_path + filename);
					int border = 25;

					img.crop(Magick::Geometry(img.columns() - border * 2,
											  img.rows() - border * 2,
											  border,
											  border));

					Magick::Image imgA = img;
					imgA.crop(Magick::Geometry(img.columns() / 2,
											  img.rows() / 2,
											  0,
											  0));

					Magick::Image imgB = img;
					imgB.crop(Magick::Geometry(img.columns() / 2,
											  img.rows() / 2,
											  img.columns() / 2,
											  0));

					Magick::Image imgC = img;
					imgC.crop(Magick::Geometry(img.columns() / 2,
											  img.rows() / 2,
											  0,
											  img.rows() / 2));

					Magick::Image imgD = img;
					imgD.crop(Magick::Geometry(img.columns() / 2,
											  img.rows() / 2,
											  img.columns() / 2,
											  img.rows() / 2));

					boost::shared_ptr<image> image_ptrA(new image(imgA, m_tile_size, m_rep_size));
					image_ptrA->filename = filename + ".A";
					m_images.push_back(image_ptrA);

					boost::shared_ptr<image> image_ptrB(new image(imgB, m_tile_size, m_rep_size));
					image_ptrB->filename = filename + ".B";
					m_images.push_back(image_ptrB);

					boost::shared_ptr<image> image_ptrC(new image(imgC, m_tile_size, m_rep_size));
					image_ptrC->filename = filename + ".C";
					m_images.push_back(image_ptrC);

					boost::shared_ptr<image> image_ptrD(new image(imgD, m_tile_size, m_rep_size));
					image_ptrD->filename = filename + ".D";
					m_images.push_back(image_ptrD);

					boost::shared_ptr<image> image_ptr(new image(img, m_tile_size, m_rep_size));
					image_ptr->filename = filename;
					m_images.push_back(image_ptr);			
				}
			}
		}
	}

	std::cout << m_images.size() << " images loaded" << std::endl;
	std::cout << "making " << m_x_tiles << " by " << m_y_tiles << " mozaic" << std::endl;

	file << "\n<tr>\n";

	// go through tiles
	int counter = 1;
	std::list<boost::shared_ptr<image> >::iterator it = m_tiles.begin();
	for (; it != m_tiles.end(); ++it) {

		std::list<boost::shared_ptr<image> >::iterator closest_image_it;
		double distance = 99999999.0;

		std::list<boost::shared_ptr<image> >::iterator it2 = m_images.begin();
		for (; it2 != m_images.end(); ++it2) {
			double local_distance = (*it2)->distance(*(*it));

			if (local_distance < distance) {
				distance = local_distance;
				closest_image_it = it2;
			}
		}

		boost::shared_ptr<image> closest_image = *closest_image_it;
		//m_images.erase(closest_image_it);

		file << "\t<td>\n";
		
		std::string thumbname_file = output_dir + closest_image->filename + "thumb.jpg";
		closest_image->write_thumbnail(thumbname_file);

		file << "\t\t<img src=\"" << closest_image->filename << "thumb.jpg\"/>";

		file << "\t</td>\n";

		if (counter != 1 && counter % m_x_tiles == 0) {
			file << "</tr>\n<tr>\n";
		}

		//std::cout << counter << std::endl;
		++counter;

		// take tile, find closest repr, write image thumb and image to dir
		// append table stuff
	}

	file << "</tr>\n";

	std::cout << "complete!" << std::endl;

	file << html_footer;
	file.close();
}

} // namespace instame