// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__FINESSE_ENCODER_PPG_UNPACKER_H
#define HDDAQ__FINESSE_ENCODER_PPG_UNPACKER_H

#include "FinesseEncoder.hh"

namespace hddaq
{
  namespace unpacker
  {

    // this class can be used with RPV-072 VME pulse pattern generator

  class FinesseEncoderPPG
    : public FinesseEncoder,
      private Uncopyable<FinesseEncoderPPG>
  {

  public:
    static const unpacker_type k_type;

  public:
    FinesseEncoderPPG(const unpacker_type& type);
    virtual ~FinesseEncoderPPG();

  protected:
    virtual void     decode();

  };

  }
}
#endif
