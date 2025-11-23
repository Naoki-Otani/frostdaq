// -*- C++ -*-

// Author: Tomonori Takahashi

#include "IStreamRegister.hh"

#include <fstream>
#include <iostream>
#include <map>
#include <string>

#include "IStreamFactory.hh"
#include "IGZFileStream.hh"
#include "IBZFileStream.hh"
#include "ISocketStream.hh"

namespace hddaq
{
  namespace unpacker
  {

namespace
{

//______________________________________________________________________________
  template <typename StreamType>
  std::istream*
  create(const std::string& stream_name,
	 std::ios_base::openmode mode)
  {
    return new StreamType(stream_name.c_str(), mode);
  }

//______________________________________________________________________________
  template <>
  std::istream*
  create<std::istream>(const std::string& strean_name,
		       std::ios_base::openmode mode)
  {
    return new std::istream(std::cin.rdbuf());
  }

}



//______________________________________________________________________________
//______________________________________________________________________________
IStreamRegister::IStreamRegister()
{
  IStreamFactory& g_factory = GIStreamFactory::get_instance();

  g_factory.add_entry(".dat",     create<std::ifstream>);
  g_factory.add_entry(".gz",      create<IGZFileStream>);
  g_factory.add_entry(".bz2",     create<IBZFileStream>);
  g_factory.add_entry("socket",   create<ISocketStream>);
  g_factory.add_entry("std::cin", create<std::istream>);
}

//______________________________________________________________________________
IStreamRegister::~IStreamRegister()
{
//   std::cout << "#D IStreamRegister::~IStreamRegister()" << std::endl;
}

  }
}
