// -*- C++ -*-

// Author: Tomonori Takahashi

// CAEN 250 MHz latching scaler with multi event buffer
// this unpacker is used for DMA(BLT) data format

#ifndef HDDAQ__VVME_FERA4300B_UNPACKER_H
#define HDDAQ__VVME_FERA4300B_UNPACKER_H

#include "VmeModule.hh"

namespace hddaq
{
  namespace unpacker
  {
    
    class VVmeFERA4300B
      : public VmeModule,
	private Uncopyable<VVmeFERA4300B>
    {

    public:
      static const unpacker_type k_type;
      static const unsigned int k_n_ch = 16U;
      static const unsigned int k_n_data_type = 1U;

      static const unsigned int k_data_type_mask  = 0x1U;
      static const unsigned int k_data_type_shift = 15U;

      static const unsigned int k_VSN_mask  = 0x1U;
      static const unsigned int k_VSN_shift = 0U;

      static const unsigned int k_EVNUM_mask  = 0xfU;
      static const unsigned int k_EVNUM_shift = 11U;

      static const unsigned int k_ch_mask  = 0xfU;
      static const unsigned int k_ch_shift = 11U;

      static const unsigned int k_data_mask  = 0x7ffU;
      static const unsigned int k_data_shift = 0U;

      static const unsigned int k_HEADER_MAGIC = 0x1U;
      static const unsigned int k_DATA_MAGIC   = 0x0U;
      //static const unsigned int k_FOOTER_MAGIC = 0x4U;


    public:
      VVmeFERA4300B(const unpacker_type& type);
      virtual ~VVmeFERA4300B();
      
    protected:
      virtual void check_data_format();
      virtual void decode();
      virtual void resize_fe_data();

    };
    
  }
}
#endif
