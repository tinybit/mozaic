#ifndef __INSTAME_TILER_HPP__
#define __INSTAME_TILER_HPP__

#include <Magick++.h>
#include <list>
#include <boost/shared_ptr.hpp>

#include "image.hpp"

namespace instame {

class tiler {
public:
	tiler(const std::string& path, int tiles, int rep_size = 20);

	void create_mozaic(const std::string& images_path, const std::string& output_dir);

private:
	Magick::Image			m_image;
	int 					m_x_tiles;
	int 					m_y_tiles;
	int 					m_tile_size;
	int						m_rep_size;

	std::list<boost::shared_ptr<image> > m_tiles;
	std::list<boost::shared_ptr<image> > m_images;
};

} // namespace instame

#endif __INSTAME_TILER_HPP__