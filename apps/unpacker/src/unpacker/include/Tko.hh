// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__TKO_MODULE_UNPACKER_H
#define HDDAQ__TKO_MODULE_UNPACKER_H

#include "UnpackerImpl.hh"

namespace hddaq
{
  namespace unpacker
  {

    class Tko
      : public UnpackerImpl,
	private Uncopyable<Tko>
    {

    public:
      static const unpacker_type k_type;
      static const unsigned int k_SMP_MA_shift  = 27U;//11+16      
      static const uint32_t     k_SMP_MA_mask   = 0xf8000000U;
      static const unsigned int k_SMP_SA_shift  = 16U;
      static const unsigned int k_SMP_SA_mask   = 0x7ff0000U;
      static const unsigned int k_SMP_DATA_mask = 0xffffU;
      static const unsigned int k_tko_n_ch      = 32U;

    public:
      Tko(const unpacker_type& type);
      virtual ~Tko();

    protected:
      enum TkoInfo{
	k_n_decoded, 
	k_size_TkoInfo
      };

      virtual void         decode();
      virtual unsigned int get_fe_info(int data_type) const;
      virtual void         resize_fe_data();
//       virtual void set_data(unsigned int);
    };

  }
}
#endif
