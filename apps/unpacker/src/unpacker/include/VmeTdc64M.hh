// -*- C++ -*-

// Author: Shuhei Hayakawa

#ifndef HDDAQ__VME_TDC64M_UNPACKER_H
#define HDDAQ__VME_TDC64M_UNPACKER_H

#include "VmeModule.hh"

namespace hddaq
{
  namespace unpacker
  {

    class VmeTdc64M
      : public VmeModule,
	private Uncopyable<VmeTdc64M>
    {

    public:
      static const unpacker_type k_type;
      static const unsigned int k_n_ch = 64;
      enum data_type {
	k_leading,
	k_trailing,
	k_hitnum,
	k_n_data_type
      };

      static const uint32_t k_local_tag_origin = 0U;
      static const uint32_t k_local_tag_max    = 0xffffU;

      static const unsigned int k_word_type_shift = 28;
      static const unsigned int k_word_type_mask  = 0xf;
      static const unsigned int k_HEADER_MAGIC  = 0xa;
      static const unsigned int k_DATA_MAGIC    = 0xc;
      static const unsigned int k_FOOTER_MAGIC  = 0xe;
      static const unsigned int k_INVALID_MAGIC = 0x3;
      ////////// header
      static const unsigned int k_data_size_shift = 0;
      static const unsigned int k_data_size_mask  = 0xfff;
      static const unsigned int k_module_id_shift = 16;
      static const unsigned int k_module_id_mask  = 0x1f;
      static const unsigned int k_emode_shift = 24;
      static const unsigned int k_emode_mask  = 0x1;
      static const unsigned int k_gmode_shift = 25;
      static const unsigned int k_gmode_mask  = 0x1;
      ////////// data
      static const unsigned int k_data_shift = 0;
      static const unsigned int k_data_mask  = 0x1ffff;
      static const unsigned int k_hit_number_shift = 17;
      static const unsigned int k_hit_number_mask  = 0xf;
      static const unsigned int k_channel_shift = 21;
      static const unsigned int k_channel_mask  = 0x3f;
      static const unsigned int k_edge_shift = 27;
      static const unsigned int k_edge_mask  = 0x1;
      ////////// footer
      static const unsigned int k_event_number_shift = 0;
      static const unsigned int k_event_number_mask  = 0xffff;
      static const unsigned int k_error1_shift = 16;
      static const unsigned int k_error1_mask  = 0x1;
      static const unsigned int k_error2_shift = 17;
      static const unsigned int k_error2_mask  = 0x1;
      static const unsigned int k_error3_shift = 18;
      static const unsigned int k_error3_mask  = 0x1;
      static const unsigned int k_error4_shift = 19;
      static const unsigned int k_error4_mask  = 0x1;

    public:
      VmeTdc64M(const unpacker_type& type);
      virtual ~VmeTdc64M();

    protected:
      virtual void check_data_format();
      virtual void decode();
      virtual void update_tag();
      virtual void resize_fe_data();

    };

  }
}
#endif
