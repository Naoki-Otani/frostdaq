// -*- C++ -*-

#ifndef HDDAQ__FUNC_NAME_H
#define HDDAQ__FUNC_NAME_H

#include <string>

namespace hddaq
{
  static const std::string
  make_func_name(const std::string& func,
		 const std::string& scope="",
		 const std::string& ret="",
		 const std::string& arg="")
  {
    return  (ret 
	     + (ret.empty() ? "" : " ")
	     + scope 
	     + "::" 
	     + func 
	     + "(" + arg + ")");
  };

}

#endif
