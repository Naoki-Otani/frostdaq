// -*- C++ -*-

// Author: Koji Miwa for E13

#ifndef HDDAQ__FERA_AD413A_UNPACKER_H
#define HDDAQ__FERA_AD413A_UNPACKER_H

#include "UnpackerImpl.hh"

namespace hddaq
{
  namespace unpacker
  {
    
    class FeraAD413A
      : public UnpackerImpl,
	private Uncopyable<FeraAD413A>
    {

    public:
      static const unpacker_type k_type;
      static const unsigned int k_n_ch = 4U;
      static const unsigned int k_n_data_type = 1U;

      static const uint32_t k_bitshift_wordtype = 15U;
      static const uint32_t k_bitshift_subaddr  = 13U;

      static const uint32_t k_VSN_MASK          = 0xffU;
      static const uint32_t k_SUBADDR_MASK      = 0x3U;
      static const uint32_t k_DATA_MASK         = 0x1fffU;

    public:
      FeraAD413A(const unpacker_type& type);
      virtual ~FeraAD413A();
      
    protected:
      virtual void decode();
      virtual void resize_fe_data();

    };
    
  }
}
#endif
