// -*- C++ -*-

// Author: Tomonori Takahashi

#include "FinesseEncoder.hh"

#include <algorithm>
#include <bitset>
#include <iterator>
#include <numeric>

#include "std_pair_dump.hh"
#include "std_ostream.hh"

namespace hddaq
{
  namespace unpacker
  {

    const defines::unpacker_type FinesseEncoder::k_type = "FinesseEncoder";

    // k_nbits should be 4, 8, 16, or 32
    static const unsigned int k_nbits = 16U;
//______________________________________________________________________________
// template struct DecodeImpl
//______________________________________________________________________________
template 
<
  unsigned int nbits,
  unsigned int iset,
  unsigned int ich = nbits - 1
>
struct DecodeImpl
{
  static const unsigned int gm_ch 
  = DecodeImpl<nbits, iset, ich-1>::gm_ch + 1U;
  static const unsigned int gm_mask
  = DecodeImpl<nbits, iset, ich-1>::gm_mask | (1U<<(gm_ch + iset*nbits));
  static const unsigned int gm_low = iset * nbits;
  static const unsigned int gm_hi  = (iset+1) * nbits -1;
};

//______________________________________________________________________________
// template specialization for ich = 0
//______________________________________________________________________________
template 
<
  unsigned int nbits,
  unsigned int iset
>    
struct DecodeImpl<nbits, iset, 0>
{
  static const unsigned int gm_ch   = 0U;
  static const unsigned int gm_mask = 1U << (iset * nbits);
  static const unsigned int gm_low  = iset * nbits;
  static const unsigned int gm_hi   = (iset+1) * nbits -1;
};

//______________________________________________________________________________
// template struct Decode
//______________________________________________________________________________
template 
<
  unsigned int nbits,
  unsigned int iset = 32U/nbits - 1U
>
struct Decode
{
  typedef DecodeImpl<nbits, iset> Impl;

  static void decode(FinesseEncoder& u,
		     uint32_t change_bits32,
		     uint32_t bits,
		     unsigned int time)
  {
    Decode<nbits, iset-1>::decode(u, change_bits32, bits, time);
    
    uint32_t change_bits = change_bits32 & Impl::gm_mask;
    if (change_bits==0U)
      return;
    
    for (unsigned int ch=Impl::gm_low; ch<=Impl::gm_hi; ++ch)
      {
	if (((change_bits>>ch) & 1U)!=0U)
	  {
	    if (u.test((bits>>ch) & 1U))
	      {
		u.fill(ch, FinesseEncoder::k_leading, time);
	      }
	    else
	      {
		u.fill(ch, FinesseEncoder::k_trailing, time);
	      }
	  }
      }
    return;
  };
  
  static void decode_edge(FinesseEncoder& u, 
			  FinesseEncoder::e_data_type data_type,
			  uint32_t bits32,
			  unsigned int time)
  {
    Decode<nbits, iset-1>::decode_edge(u, data_type, bits32, time);
    uint32_t bits = bits32 & Impl::gm_mask;
    if (!u.test(bits))
      return;
    
    for (unsigned int ch=Impl::gm_low; ch<=Impl::gm_hi; ++ch)
      {
	if (u.test((bits>>ch) & 1U))
	  u.fill(ch, data_type, time);
      }
    return;
  };
  
};
    
//______________________________________________________________________________
// template specialization for iset = 0
//______________________________________________________________________________
template <unsigned int nbits>
struct Decode<nbits, 0>
{
  typedef DecodeImpl<nbits,0> Impl;

  static void decode(FinesseEncoder& u,
		     uint32_t change_bits32,
		     uint32_t bits,
		     unsigned int time)
  {
    uint32_t change_bits = change_bits32 & Impl::gm_mask;
    if (change_bits==0U)
      return;
    
    for (unsigned int ch=Impl::gm_low; ch<=Impl::gm_hi; ++ch)
      {
	if (((change_bits>>ch) & 1U)!=0U)
	  {
	    if (u.test((bits>>ch) & 1U))
	      {
		u.fill(ch, FinesseEncoder::k_leading, time);
	      }
	    else
	      {
		u.fill(ch, FinesseEncoder::k_trailing, time);
	      }
	  }
      }
    return;
  };

  static void decode_edge(FinesseEncoder& u, 
			  FinesseEncoder::e_data_type data_type,
			  uint32_t bits32,
			  unsigned int time)
  {
    uint32_t bits = bits32 & Impl::gm_mask;
    if (!u.test(bits))
      return;

    for (unsigned int ch=Impl::gm_low; ch<=Impl::gm_hi; ++ch)
      {
	if (u.test((bits>>ch) & 1U))
	  u.fill(ch, data_type, time);
      }
    return;
  };

};

//______________________________________________________________________________
      uint32_t xor32(uint32_t so_far,
		     uint32_t new_face)
      {
	return so_far ^ new_face;
      }



//______________________________________________________________________________
// class FinesseEncoder
//______________________________________________________________________________
FinesseEncoder::FinesseEncoder(const unpacker_type& type)
  : Finesse(type)
{
}

//______________________________________________________________________________
FinesseEncoder::~FinesseEncoder()
{
}

//______________________________________________________________________________
unsigned int
FinesseEncoder::calc_check_sum() const
{
  const_iterator first = m_data_first;
  const_iterator last  = m_data_last;

  const unsigned int check_sum32
    = std::accumulate(first+1, last-1, 0U, xor32);
  const unsigned int check_sum16
    = (check_sum32 & 0x0000ffffU) 
    ^ ((check_sum32 >> 16) & 0x0000ffffU);
  return check_sum16;
}

//______________________________________________________________________________
void
FinesseEncoder::decode()
{
//   cout << "#D data size = " << m_data_size << std::endl;
  if (m_data_size==k_min_data_size)
    return;

  const_iterator prev    = m_body_first;
  const_iterator current = prev + 1;
  const_iterator last    = m_body_last;
  uint32_t bits32        = *prev;
  uint32_t change32      = 0U;
  unsigned int t         = 0U;

  if (test(bits32))
    {
      // signal starts at the 1st word
      Decode<k_nbits>::decode_edge(*this, k_leading, bits32, t);
    }

  for (;current!=last; ++prev, ++current)
    {
      ++t;
      change32 = (*prev) ^ (*current);
      if (change32!=0U)
	{
	  bits32 = *current;
	  Decode<k_nbits>::decode(*this, change32, bits32, t);
	}
    }

  if (test(bits32))
    {
      // signal ends at the last word
      Decode<k_nbits>::decode_edge(*this, k_trailing, bits32, t+1);
    }

  return;
}

//______________________________________________________________________________
const std::map<std::string, int>&
FinesseEncoder::get_data_id() const
{
  static std::map<std::string, int> s_data_ref;
  if(s_data_ref.empty())
    {
      s_data_ref["leading"]  = k_leading;
      s_data_ref["trailing"] = k_trailing;
    }
  return s_data_ref;
}

//______________________________________________________________________________
void
FinesseEncoder::resize_fe_data()
{
  m_fe_data.resize(k_n_ch);
  for (unsigned int i=0; i<k_n_ch; ++i)
    m_fe_data[i].resize(k_n_data_type);
  return;
}

//______________________________________________________________________________
bool
FinesseEncoder::test(unsigned int bits) const
{
  return ((bits!=0) ? true : false);
}

  }
}
