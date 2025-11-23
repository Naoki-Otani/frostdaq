// -*- C++ -*-

// Author: Tomonori Takahashi

// CAEN 250 MHz latching scaler with multi event buffer
// this unpacker is used for DMA(BLT) data format

#ifndef HDDAQ__VVME_COINREG_UNPACKER_H
#define HDDAQ__VVME_COINREG_UNPACKER_H

#include "VmeModule.hh"

namespace hddaq
{
  namespace unpacker
  {
    
    class VVmeCoinReg
      : public VmeModule,
	private Uncopyable<VVmeCoinReg>
    {

    public:
      static const unpacker_type k_type;
      static const unsigned int k_n_ch = 16U;
      static const unsigned int k_n_data_type = 1U;

      static const unsigned int k_data_mask  = 0xffffU;
      static const unsigned int k_data_shift = 0U;

    public:
      VVmeCoinReg(const unpacker_type& type);
      virtual ~VVmeCoinReg();
      
    protected:
      virtual void check_data_format();
      virtual void decode();
      virtual void resize_fe_data();

    };
    
  }
}
#endif
