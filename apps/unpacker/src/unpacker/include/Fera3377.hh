// -*- C++ -*-

// Author: Koji Miwa for E13

#ifndef HDDAQ__FERA_3377_UNPACKER_H
#define HDDAQ__FERA_3377_UNPACKER_H

#include "UnpackerImpl.hh"

namespace hddaq
{
  namespace unpacker
  {
    
    class Fera3377
      : public UnpackerImpl,
	private Uncopyable<Fera3377>
    {

    public:
      static const unpacker_type k_type;
      static const unsigned int k_n_ch = 32U; 
      //static const unsigned int k_n_data_type = 1U;

      // Defition of data 
      enum e_data_type{
	k_leading     = 0,
	k_trailing    = 1,
	k_n_data_type = 2,
      };

      static const uint32_t k_bitshift_wordtype = 15U;
      static const uint32_t k_bitshift_FS       = 8U;
      static const uint32_t k_bitshift_channel  = 10U;
      static const uint32_t k_bitshift_RT       = 9U;

      static const uint32_t k_VSN_MASK          = 0xffU;
      static const uint32_t k_CHANNEL_MASK      = 0x1fU;
      static const uint32_t k_DATA_MASK         = 0xffU;

    public:
      Fera3377(const unpacker_type& type);
      virtual ~Fera3377();
      
    protected:
      virtual void decode();
      virtual void resize_fe_data();

    };
    
  }
}
#endif
