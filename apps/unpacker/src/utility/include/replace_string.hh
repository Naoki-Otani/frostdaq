// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__REPLACE_STRING_H
#define HDDAQ__REPLACE_STRING_H

#include <string>

namespace hddaq
{
    void
    replace_all(std::string& source,
		const std::string& pattern,
		const std::string& placement);
    
}
#endif
