// -*- C++ -*-

// Author: Koji Miwa for E13

#ifndef HDDAQ__VME_AMT_UNPACKER_H
#define HDDAQ__VME_AMT_UNPACKER_H

#include "VmeModule.hh"

namespace hddaq
{
  namespace unpacker
  {
    
    class VmeAmt
      : public VmeModule,
	private Uncopyable<VmeAmt>
    {

    public:
      static const unpacker_type k_type;
      static const unsigned int k_n_ch = 64U;
      //static const unsigned int k_n_data_type = 1U;

      enum e_data_type{
	k_leading     =0,
	k_trailing    =1,
	k_n_data_type =2,
      };

      static const uint32_t k_bitshift_wordtype = 29U;
      static const uint32_t k_bitshift_tntotal  = 16U;
      static const uint32_t k_bitshift_lt       = 28U;
      static const uint32_t k_bitshift_ch       = 20U;

      static const uint32_t k_TNTOTAL_MASK      = 0x1fffU;
      static const uint32_t k_EVENTNUM_MASK     = 0xffffU;
      static const uint32_t k_CHANNEL_MASK      = 0x3fU;
      static const uint32_t k_LT_MASK           = 0x1U;
      static const uint32_t k_TDC_MASK          = 0xfffffU;

    public:
      VmeAmt(const unpacker_type& type);
      virtual ~VmeAmt();
      
    protected:
      virtual void decode();
      virtual void resize_fe_data();

    };
    
  }
}
#endif
