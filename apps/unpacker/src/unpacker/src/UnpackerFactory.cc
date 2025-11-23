// -*- C++ -*-

// Author: Tomonori Takahashi

#include "UnpackerFactory.hh"

#include "std_ostream.hh"

namespace hddaq
{
  namespace unpacker
  {

//______________________________________________________________________________
UnpackerImpl*
create_unpacker(const UnpackerImpl::unpacker_type& type)
{
  UnpackerFactory::creator_type& unpacker_constructor 
    = GUnpackerFactory::get_instance().get_creator(type);
//   GUnpackerFactory::get_instance().hoge();
  return unpacker_constructor(type);
}

//______________________________________________________________________________
UnpackerFactory::UnpackerFactory()
  : Factory
<
  UnpackerImpl, 
  defines::unpacker_type,
  UnpackerImpl* (*)(const UnpackerImpl::unpacker_type&)
> ()
{
}

//______________________________________________________________________________
UnpackerFactory::~UnpackerFactory()
{
//   cout << "#D UnpackerFactory::~UnpackerFactory()" << std::endl;
}

//______________________________________________________________________________
void 
UnpackerFactory::hoge(const std::string& arg) const
{
  cout << "#D UnpackerFactory::hoge()  " << arg << std::endl;
  return;
}

//______________________________________________________________________________
const std::string
UnpackerFactory::get_name() const
{
  return "UnpackerFactory";
}

  }
}
