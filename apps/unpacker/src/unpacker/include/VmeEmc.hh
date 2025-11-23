// -*- C++ -*-

// Author: Shuhei Hayakawa

#ifndef HDDAQ__VME_EMC_UNPACKER_H
#define HDDAQ__VME_EMC_UNPACKER_H

#include "VmeModule.hh"

namespace hddaq
{
  namespace unpacker
  {
    
    class VmeEmc
      : public VmeModule,
	private Uncopyable<VmeEmc>
    {

    public:
      static const unpacker_type k_type;
      static const unsigned int k_n_channel = 1;
      enum data_type { k_serial, k_xpos, k_ypos, k_state, k_utime, k_ltime, k_n_data_type };
      
      static const unsigned int k_word_type_shift = 28;
      static const unsigned int k_word_type_mask  = 0xf;
      static const unsigned int k_header_magic  = 0xa;
      static const unsigned int k_serial_magic  = 0x0;
      static const unsigned int k_xpos_magic    = 0x1;
      static const unsigned int k_ypos_magic    = 0x2;
      static const unsigned int k_state_magic   = 0x3;
      static const unsigned int k_utime_magic   = 0x4;
      static const unsigned int k_ltime_magic   = 0x5;
      static const unsigned int k_footer_magic  = 0xe;
      ////////// header
      static const unsigned int k_update_shift = 0;
      static const unsigned int k_update_mask  = 0x1;
      
      ////////// data
      static const unsigned int k_data_shift = 0;
      static const unsigned int k_data_mask  = 0xfffffff;
      
    public:
      VmeEmc(const unpacker_type& type);
      virtual ~VmeEmc();
      
    protected:
      virtual void check_data_format();
      virtual void decode();
      virtual void resize_fe_data();

    };
    
  }
}
#endif
