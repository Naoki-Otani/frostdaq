// -*- C++ -*-

// Author: Tomonori Takahashi

// CAEN 250 MHz latching scaler with multi event buffer
// this unpacker is used for DMA(BLT) data format

#ifndef HDDAQ__VME_CAEN_V830_UNPACKER_H
#define HDDAQ__VME_CAEN_V830_UNPACKER_H

#include "VmeModule.hh"

namespace hddaq
{
  namespace unpacker
  {
    
    class VmeCaenV830
      : public VmeModule,
	private Uncopyable<VmeCaenV830>
    {

    public:
      static const unpacker_type k_type;
      static const unsigned int k_n_ch = 32U;
      static const unsigned int k_n_data_type = 1U;

      enum e_header
	{
	  k_header_trigger_number_mask = 0x0000ffffU,
	  k_header_TS_mask             = 0x00030000U,
	  k_header_TS_shift            = 16U,
	  k_header_NWORDS_mask         = 0x00fc0000U,
	  k_header_NWORDS_shift        = 18U,
	  k_header_identifier_bit      = 0x04000000U,
	  k_header_identifier_shift    = 26U,
	  k_header_GEO_address_mask    = 0xf8000000U,
	  k_header_GEO_address_shift   = 27U
	};

      enum e_trigger_source
	{
	  k_external_trigger  = 0,
	  k_trigger_via_timer = 1,
	  k_trigger_via_vme   = 2
	};


    public:
      VmeCaenV830(const unpacker_type& type);
      virtual ~VmeCaenV830();
      
    protected:
      virtual void check_data_format();
      virtual void decode();
      virtual void resize_fe_data();

    };
    
  }
}
#endif
