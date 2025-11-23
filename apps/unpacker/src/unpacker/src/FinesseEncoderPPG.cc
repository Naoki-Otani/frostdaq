// -*- C++ -*-

// Author: Tomonori Takahashi

#include "FinesseEncoderPPG.hh"

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

    const defines::unpacker_type FinesseEncoderPPG::k_type
    = "FinesseEncoderPPG";

//______________________________________________________________________________
FinesseEncoderPPG::FinesseEncoderPPG(const unpacker_type& type)
  : FinesseEncoder(type)
{
}

//______________________________________________________________________________
FinesseEncoderPPG::~FinesseEncoderPPG()
{
}

//______________________________________________________________________________
void
FinesseEncoderPPG::decode()
{
  const_iterator first = m_body_first;
  const_iterator last  = m_body_last;
  for (const_iterator i = first; i!=last; ++i)
    fill(0, 0, *i);

  return;
}

  }
}
