// -*- C++ -*-

// Author: Tomonori Takahashi

#include "replace_string.hh"

#include "std_ostream.hh"

namespace hddaq
{


//______________________________________________________________________________
void
replace_all(std::string& source,
	    const std::string& pattern,
	    const std::string& placement)
{
// 	cout << "rep1 = " << source << " " << pattern << " " << placement
// 		  << std::endl;
	std::string result(source);
	for (std::string::size_type pos = 0;
	     std::string::npos!= (pos=result.find(pattern, pos));
	     pos += placement.size())
	  result.replace(pos, pattern.size(), placement);
	source.swap(result);

// 	cout << "rep2 = " << source << " " << pattern << " " << placement
// 		  << std::endl;
	return;
}

}
