// -*- C++ -*-

// Author: Tomonori Takahashi

#include "FinesseEncoderInverse.hh"

#include <algorithm>
#include <bitset>
#include <iterator>
#include <numeric>

#include "std_ostream.hh"

namespace hddaq
{
  namespace unpacker
  {

//______________________________________________________________________________
  namespace
  {
//______________________________________________________________________________
    // uint32_t xor32(uint32_t so_far,
    // 		   uint32_t new_face)
    // {
    //   return so_far ^ new_face;
    // };

  }

    const defines::unpacker_type FinesseEncoderInverse::k_type
    = "FinesseEncoderInverse";

//______________________________________________________________________________
FinesseEncoderInverse::FinesseEncoderInverse(const unpacker_type& type)
  : FinesseEncoder(type)
{
}

//______________________________________________________________________________
FinesseEncoderInverse::~FinesseEncoderInverse()
{
}

//______________________________________________________________________________
bool
FinesseEncoderInverse::test(unsigned int bits) const
{
  return !FinesseEncoder::test(bits);
}


  }
}
