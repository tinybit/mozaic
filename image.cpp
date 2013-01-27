#include <iostream>
#include <cmath>
#include <stdexcept>

#include <boost/lexical_cast.hpp>

#include "image.hpp"

namespace instame {

image::image(const std::string& path, int thumbnail_size, int rep_size, bool remove_border) :
	m_path(path),
	m_pixels_rep(NULL),
	m_rep_size(rep_size),
	m_thumbnail_size(thumbnail_size),
	m_remove_border(remove_border)
{
	m_image.read(path);
	process_image();
}

image::image(Magick::Image& image, int thumbnail_size, int rep_size, bool remove_border) :
	m_image(image),
	m_rep_size(rep_size),
	m_pixels_rep(NULL),
	m_thumbnail_size(thumbnail_size),
	m_remove_border(remove_border)
{
	process_image();
}

void
image::process_image() {
	if (m_remove_border) {
		int border = 35;

		m_image.crop(Magick::Geometry(m_image.columns() - border * 2,
									  m_image.rows() - border * 2,
									  border,
									  border));
	}

	m_thumbnail = m_image;

	std::string tttt = boost::lexical_cast<std::string>(m_thumbnail_size);
	tttt = tttt + "x" + tttt + "!";
	Magick::Geometry thumb_geo(tttt.c_str());
	m_thumbnail.resize(thumb_geo);

	std::string rrrr = boost::lexical_cast<std::string>(m_rep_size);
	rrrr = rrrr + "x" + rrrr + "!";
	Magick::Geometry image_geo(rrrr);
	m_image.resize(image_geo);

	m_image.modifyImage();
	m_image.type(Magick::TrueColorType);
	m_pixels_rep = m_image.getPixels(0, 0, m_rep_size, m_rep_size);
}

double
image::distance(image& image) {
	return distance_naive(image);
}

double
image::distance_exclusive(image& img) {
	int bad_pixels = 0;

	for (int i = 0; i < m_rep_size; ++i) {
		for (int j = 0; j < m_rep_size; ++j) {
			Magick::ColorRGB pixel_dst = img.get_pixel(i, j);
			Magick::ColorRGB pixel_src = get_pixel(i, j);

			if (pixel_distance(pixel_dst, pixel_src) > m_threshold) {
				bad_pixels++;
			}
		}
	}

	return (double)bad_pixels / (double)(m_rep_size * m_rep_size);
}

double
image::distance_mean(image& img) {
	double total_distance = 0.0;

	for (int i = 0; i < m_rep_size; ++i) {
		for (int j = 0; j < m_rep_size; ++j) {
			Magick::ColorRGB pixel_dst = img.get_pixel(i, j);
			Magick::ColorRGB pixel_src = get_pixel(i, j);
			total_distance += pixel_distance(pixel_dst, pixel_src);
		}
	}

	return total_distance / (m_rep_size * m_rep_size);
}

double
image::distance_naive(image& img) {
	double total_distance = 0.0;

	for (int i = 0; i < m_rep_size; ++i) {
		for (int j = 0; j < m_rep_size; ++j) {
			Magick::ColorRGB pixel_dst = img.get_pixel(i, j);
			Magick::ColorRGB pixel_src = get_pixel(i, j);
			total_distance += pixel_distance(pixel_dst, pixel_src);
		}
	}

	return total_distance;
}

void
image::write_representation(const std::string& path) {
	m_image.write(path);
}

void
image::write_thumbnail(const std::string& path) {
	m_thumbnail.write(path);
}

Magick::ColorRGB
image::get_pixel(int x, int y) {
	Magick::Color pixel = m_pixels_rep[m_rep_size * y + x];
	Magick::ColorRGB pixel_rgb = pixel;
	return pixel_rgb;
}

void
image::set_pixel(int x, int y, const Magick::ColorRGB& color) {
	m_pixels_rep[m_rep_size * y + x] = color;
}

inline double
image::pixel_distance(const Magick::ColorRGB& src, const Magick::ColorRGB& dst) const {
	double delta_r = src.red() - dst.red();
	double delta_g = src.green() - dst.green();
	double delta_b = src.blue() - dst.blue();

	return sqrt(delta_r * delta_r + delta_g * delta_g + delta_b * delta_b);
}

std::string
image::path() const {
	return m_path;
}

} // namespace instame