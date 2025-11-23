// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__VME_CAEN_V820_UNPACKER_H
#define HDDAQ__VME_CAEN_V820_UNPACKER_H

#include "VmeModule.hh"

namespace hddaq
{
  namespace unpacker
  {
    
    class VmeCaenV820
      : public VmeModule,
	private Uncopyable<VmeCaenV820>
    {

    public:
      static const unpacker_type k_type;
      static const unsigned int k_n_ch = 32U;
      static const unsigned int k_n_data_type = 1U;

    public:
      VmeCaenV820(const unpacker_type& type);
      virtual ~VmeCaenV820();
      
    protected:
      virtual void decode();
      virtual void resize_fe_data();

    };
    
  }
}
#endif
