// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__O_STREAM_REGISTER_H
#define HDDAQ__O_STREAM_REGISTER_H

#include "Uncopyable.hh"

namespace hddaq
{
  namespace unpacker
  {

  class OStreamRegister
    : private Uncopyable<OStreamRegister>
  {

  public:
     OStreamRegister();
    ~OStreamRegister();

  };

  }
}
#endif
