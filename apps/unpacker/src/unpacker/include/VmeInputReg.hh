// -*- C++ -*-

// Author: S.H. Hayakawa

#ifndef HDDAQ__VME_INPUTREG_UNPACKER_H
#define HDDAQ__VME_INPUTREG_UNPACKER_H

#include "VmeModule.hh"

namespace hddaq
{
  namespace unpacker
  {

    class VmeInputReg
      : public VmeModule,
	private Uncopyable<VmeInputReg>
    {

    public:
      static const unpacker_type k_type;
      static const unsigned int  k_n_ch = 32;
      enum e_data { k_reset_flag, k_n_data };

      static const unsigned int k_channel_mask = 0x1f;
      static const unsigned int k_data_mask    = 0x1;

    public:
      VmeInputReg(const unpacker_type& type);
      virtual ~VmeInputReg();

    protected:
      virtual void check_data_format();
      virtual void decode();
      virtual void resize_fe_data();
  
    };
  }

}

#endif

