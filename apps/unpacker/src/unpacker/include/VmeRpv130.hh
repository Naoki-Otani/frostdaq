// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__VME_RPV_130_UNPACKER_H
#define HDDAQ__VME_RPV_130_UNPACKER_H


#include "Uncopyable.hh"
#include "VmeModule.hh"

namespace hddaq
{
  namespace unpacker
  {
    
    class VmeRpv130
      : public VmeModule,
	private Uncopyable<VmeRpv130>
    {

    public:
      static const unpacker_type k_type;
      static const unsigned int k_n_ch      = 8U;
      static const unsigned int k_data_mask = 0xffffU;

      enum e_data_type
      {
	k_latch1,
	k_latch2,
	k_flipflop,
	k_through,
	k_n_data_type
      };

    public:
               VmeRpv130(const unpacker_type& type);
      virtual ~VmeRpv130();

      virtual void decode();
      virtual const std::map<std::string, int>&
                   get_data_id() const;
      virtual void resize_fe_data();

    };

  }
}
#endif
