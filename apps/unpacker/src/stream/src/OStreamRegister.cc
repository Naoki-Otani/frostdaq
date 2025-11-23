// -*- C++ -*-

// Author: Tomonori Takahashi

#include "OStreamRegister.hh"

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "OStreamFactory.hh"
#include "OGZFileStream.hh"
#include "OBZFileStream.hh"
#include "OSocketStream.hh"

namespace hddaq
{
  namespace unpacker
  {

namespace
{

  template <typename StreamType>
  std::ostream*
  create(const std::string& stream_name,
	 std::ios_base::openmode mode)
  {
    return new StreamType(stream_name.c_str(), mode);
  }

}



//______________________________________________________________________________
OStreamRegister::OStreamRegister()
{
  OStreamFactory& g_factory = GOStreamFactory::get_instance();
  
  g_factory.add_entry(".dat",   create<std::ofstream>);
  g_factory.add_entry(".gz",    create<OGZFileStream>);
  g_factory.add_entry(".bz2",   create<OBZFileStream>);
  g_factory.add_entry("socket", create<OSocketStream>);
}

//______________________________________________________________________________
OStreamRegister::~OStreamRegister()
{
//   std::cout << "#D OStreamRegister destructor called" << std::endl;
}

  }
}
