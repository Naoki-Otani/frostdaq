// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__UNPACKER_REGISTER_H
#define HDDAQ__UNPACKER_REGISTER_H

#include "UnpackerFactory.hh"

namespace hddaq
{
  namespace unpacker
  {

  class UnpackerRegister 
    : private Uncopyable<UnpackerRegister>
  {

  private:
    typedef UnpackerFactory::id_type      id_type;
    typedef UnpackerFactory::creator_type creator_type;
    
  public:
     UnpackerRegister();
     UnpackerRegister(const id_type& type,
		      creator_type creator);
    ~UnpackerRegister();
  
  };

    template<typename T>
    UnpackerFactory::product_type*
    create(const UnpackerFactory::id_type& type)
    {
      return new T(type);
    }

  }  
}
#endif
