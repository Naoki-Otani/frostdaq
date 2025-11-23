// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__FINESSE_ENCODER_INVERSE_UNPACKER_H
#define HDDAQ__FINESSE_ENCODER_INVERSE_UNPACKER_H

#include "FinesseEncoder.hh"

namespace hddaq
{
  namespace unpacker
  {

  class FinesseEncoderInverse
    : public FinesseEncoder,
      private Uncopyable<FinesseEncoderInverse>
  {

  public:
    static const unpacker_type k_type;

  public:
    FinesseEncoderInverse(const unpacker_type& type);
    virtual ~FinesseEncoderInverse();

    virtual bool test(unsigned int bits) const;

  };

  }
}
#endif
