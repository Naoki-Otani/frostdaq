// -*- C++ -*-

// Author: Tomonori Takahashi

#include "std_ostream.hh"

#include <fstream>
#include <map>

#include <cstdlib>

namespace hddaq
{
  std::ostream cerr(std::cerr.rdbuf());
  std::ostream cout(std::cout.rdbuf());
  std::ostream tag_summary(std::cout.rdbuf());

  namespace
  {
    std::ostream*
    get_ofs(const std::string& name,
	    const std::string& type)
    {
      static std::map<std::string, std::ostream*> s_os;
      std::ostream*& os = s_os[name];
      if (!os)
	{
	  if (name.find("std::cout")!=std::string::npos)
	    os = new std::ostream(std::cout.rdbuf());
	  else if (name.find("std::cerr")!=std::string::npos)
	    os = new std::ostream(std::cerr.rdbuf());
	  else
	    os = new std::ofstream(name.c_str());
	  if (os->fail())
	    {
	      cerr << "#E failed to open : " << name
		   << "  as " << type  << std::endl;
	      std::exit(0);
	    }
	}
      return os;
    }

  }


//______________________________________________________________________________
  void
  set_cerr(const std::string& name)
  {
    if (!name.empty())
      cerr.rdbuf(get_ofs(name, "cerr")->rdbuf());
    return;
  }

//______________________________________________________________________________
  void
  set_cout(const std::string& name)
  {
    if (!name.empty())
      cout.rdbuf(get_ofs(name, "cout")->rdbuf());
    return;
  }


//______________________________________________________________________________
  void
  set_tag_summary(const std::string& name)
  {
    if (!name.empty())
      tag_summary.rdbuf(get_ofs(name, "tag_summary")->rdbuf());
    return;
  }

}
