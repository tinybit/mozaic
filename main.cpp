#include <iostream>
#include <cstdlib>

#include <Magick++.h>
#include <boost/lexical_cast.hpp>

#include "tiler.hpp"

int main(int argc, char **argv)  {

	int tiles = 16;
	int rep_size = 10;

	if (argc > 1) {
		tiles = boost::lexical_cast<int>(argv[1]);
	}

	if (argc > 2) {
		rep_size = boost::lexical_cast<int>(argv[2]);
	}

	Magick::InitializeMagick(*argv);

	try { 
		instame::tiler tiler("../data/test.jpg", tiles, rep_size);
		tiler.create_mozaic("../data/images/", "../data/output/");
	}
	catch(const std::exception& ex) { 
		std::cout << "Caught exception: " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}