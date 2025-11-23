// -*- C++ -*-

// Author: Shuhei Hayakawa

#ifndef HDDAQ__VME_CAEN_V775_UNPACKER_H
#define HDDAQ__VME_CAEN_V775_UNPACKER_H

#include "VmeModule.hh"

namespace hddaq
{
  namespace unpacker
  {

    class VmeCaenV775
      : public VmeModule,
	private Uncopyable<VmeCaenV775>
    {

    public:
      static const unpacker_type k_type;
      static const unsigned int k_n_channel   = 32;
      enum data_type { k_tdc, k_n_data_type };

      static const uint32_t k_local_tag_origin = 0U;
      static const uint32_t k_local_tag_max    = 0xffffffU;

      static const unsigned int k_word_type_shift = 24;
      static const unsigned int k_word_type_mask  = 0x7;
      static const unsigned int k_HEADER_MAGIC    = 0x2;
      static const unsigned int k_DATA_MAGIC      = 0x0;
      static const unsigned int k_FOOTER_MAGIC    = 0x4;
      static const unsigned int k_INVALID_MAGIC   = 0x6;
      ////////// header
      static const unsigned int k_memorised_channel_shift = 8;
      static const unsigned int k_memorised_channel_mask  = 0x3f;
      static const unsigned int k_crate_shift             = 16;
      static const unsigned int k_crate_mask              = 0xff;
      static const unsigned int k_header_geo_shift        = 27;
      static const unsigned int k_header_geo_mask         = 0x1f;
      ////////// data
      static const unsigned int k_data_shift            = 0;
      static const unsigned int k_data_mask             = 0xfff;
      static const unsigned int k_overflow_shift        = 12;
      static const unsigned int k_overflow_mask         = 0x1;
      static const unsigned int k_under_threshold_shift = 13;
      static const unsigned int k_under_threshold_mask  = 0x1;
      static const unsigned int k_valid_shift           = 14;
      static const unsigned int k_valid_mask            = 0x1;
      static const unsigned int k_channel_shift         = 16;
      static const unsigned int k_channel_mask          = 0x3f;
      static const unsigned int k_data_geo_shift        = 27;
      static const unsigned int k_data_geo_mask         = 0x1f;
      ////////// footer
      static const unsigned int k_event_number_shift = 0;
      static const unsigned int k_event_number_mask  = 0xffffff;
      static const unsigned int k_footer_geo_shift   = 27;
      static const unsigned int k_footer_geo_mask    = 0x1f;

    public:
      VmeCaenV775(const unpacker_type& type);
      virtual ~VmeCaenV775();

    protected:
      virtual void check_data_format();
      virtual void decode();
      virtual void update_tag();
      virtual void resize_fe_data();

    };

  }
}
#endif
