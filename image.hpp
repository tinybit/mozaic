#ifndef __INSTAME_IMAGE_HPP__
#define __INSTAME_IMAGE_HPP__

#include <Magick++.h>

namespace instame {

class image {
public:
	image(const std::string& path, int thumbnail_size, int rep_size = 20, bool remove_border = false);
	image(Magick::Image& image, int thumbnail_size, int rep_size = 20, bool remove_border = false);

	double distance(image& image);
	double distance_naive(image& image);
	double distance_exclusive(image& image);
	double distance_mean(image& image);

	void write_representation(const std::string& path);
	void write_thumbnail(const std::string& path);
	std::string path() const;

	std::string				filename;

private:
	Magick::ColorRGB 		get_pixel(int x, int y);
	void 					set_pixel(int x, int y, const Magick::ColorRGB& color);
	double 					pixel_distance(const Magick::ColorRGB& src, const Magick::ColorRGB& dst) const;
	void					process_image();

	std::string				m_path;
	Magick::Image			m_image;
	Magick::Image			m_thumbnail;
	Magick::PixelPacket*	m_pixels_rep;
	int						m_rep_size;
	int						m_thumbnail_size;
	bool					m_remove_border;

	static const double		m_threshold = 0.5;
};

} // namespace instame

#endif __INSTAME_IMAGE_HPP__