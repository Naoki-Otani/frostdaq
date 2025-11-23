// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__I_STREAM_REGISTER_H
#define HDDAQ__I_STREAM_REGISTER_H

#include "Uncopyable.hh"

namespace hddaq
{
  namespace unpacker
  {

  class IStreamRegister
    : private Uncopyable<IStreamRegister>
  {

  public:
     IStreamRegister();
    ~IStreamRegister();

  };

  }
}
#endif
