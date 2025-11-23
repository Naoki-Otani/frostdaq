// -*- C++ -*-

// Author: Koji Miwa for E13

#include "FeraAD413A.hh"

#include <algorithm>

#include "std_ostream.hh"
#include "HexDump.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type FeraAD413A::k_type = "FeraAD413A";

//______________________________________________________________________________
FeraAD413A::FeraAD413A(const unpacker_type& type)
  : UnpackerImpl(type)
{
}

//______________________________________________________________________________
FeraAD413A::~FeraAD413A()
{
}

//______________________________________________________________________________
void
FeraAD413A::decode()
{
  const_iterator first = m_data_first;
  const_iterator last  = m_data_last;

  for (const_iterator itr = first; itr!=last; ++itr)
    {
      int wordtype = (*itr >> k_bitshift_wordtype) & 0x1;
      if (1 == wordtype) {
	// Header
	//	unsigned int vsn = *itr & k_VSN_MASK;
      } else {
	// Data
	unsigned int  subaddr = (*itr >> k_bitshift_subaddr) & k_SUBADDR_MASK;
	unsigned int  adc = *itr & k_DATA_MASK;
	
	fill(subaddr, 0, adc);
      }
    }

  return;
}

//______________________________________________________________________________
void
FeraAD413A::resize_fe_data()
{
  m_fe_data.resize(k_n_ch);
  for (unsigned int i=0; i<k_n_ch; ++i) {
    m_fe_data[i].resize(k_n_data_type);
  }

  return;
}

}
}
