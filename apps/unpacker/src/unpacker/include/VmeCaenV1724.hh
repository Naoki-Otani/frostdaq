// -*- C++ -*-

// Author: Tomonori Takahashi

// this unpacker is used for DMA(BLT) data format

#ifndef HDDAQ__VME_V1724_UNPACKER_H
#define HDDAQ__VME_V1724_UNPACKER_H

#include "VmeModule.hh"

namespace hddaq
{
  namespace unpacker
  {
    
    class VmeCaenV1724
      : public VmeModule,
	private Uncopyable<VmeCaenV1724>
    {

    public:

      static const uint32_t k_local_tag_origin = 0U;
      static const uint32_t k_local_tag_max    = 0xffffffU;

      static const unsigned int k_header_mask  = 0x0f;
      static const unsigned int k_header_shift = 28U;
      static const unsigned int k_HEADER_MAGIC  = 0x0aU;//1010

      static const unsigned int k_mode_mask  = 0x01;
      static const unsigned int k_mode_shift = 24U;
      static const unsigned int k_MODE_TRUE_MAGIC  = 0x1U;
      static const unsigned int k_MODE_FALSE_MAGIC = 0x0U;

      static const unsigned int k_event_size_mask = 0x0fffffff;
      static const unsigned int k_ch_mask_mask = 0xff;

      static const unpacker_type k_type;
      static const unsigned int k_n_ch = 8U;
      static const unsigned int k_n_data_type = 1U;

      static const unsigned int k_ch_mask  = 0x7U;
      static const unsigned int k_ch_shift = 24U;

      static const unsigned int k_sample_mask  = 0xffffffU;
      static const unsigned int k_sample_shift = 0U;

//      static const unsigned int k_data_mask  = 0xffffU;
      static const unsigned int k_data_mask  = 0x3fffU;
      static const unsigned int k_data_shift = 0U;
      static const unsigned int k_data_shift_even = 0U;
      static const unsigned int k_data_shift_odd  = 16U;
      static const unsigned int k_pedesup_data_mask  = 0xffffU;

      static const unsigned int k_event_number_mask  = 0xffffff;

    public:
      VmeCaenV1724(const unpacker_type& type);
      virtual ~VmeCaenV1724();
      
    protected:
      //      virtual void check_data_format();
      virtual void decode();
      virtual void update_tag();
      virtual void resize_fe_data();

    };
    
  }
}
#endif
