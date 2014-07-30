#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED 
#include "stdafx.h"

namespace fs = ::boost::filesystem;

// return the filenames of all files that have the specified extension
// in the specified directory and all subdirectories
void get_all(const fs::path& root, const std::string& ext, std::vector<fs::path>& ret)
{
	if (!fs::exists(root)) return;

	if (fs::is_directory(root))
	{
		fs::recursive_directory_iterator it(root);
		fs::recursive_directory_iterator endit;
		while (it != endit)
		{
			if (fs::is_regular_file(*it) && it->path().extension() == ext)
			{
				ret.push_back(it->path().filename());
			}
			++it;
		}
	}
}
