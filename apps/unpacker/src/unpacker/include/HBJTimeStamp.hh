// -*- C++ -*-

// Author: Kenji Hosomi for E13

#ifndef HDDAQ__HBJ_TIME_STAMP_UNPACKER_H
#define HDDAQ__HBJ_TIME_STAMP_UNPACKER_H

#include "VmeModule.hh"

namespace hddaq
{
  namespace unpacker
  {
    
    class HBJTimeStamp
      : public VmeModule,
	private Uncopyable<HBJTimeStamp>
    {

    public:
      static const unpacker_type k_type;
      static const unsigned int k_n_ch = 1U;
      static const unsigned int k_n_data_type = 1U;

    public:
      HBJTimeStamp(const unpacker_type& type);
      virtual ~HBJTimeStamp();
      
    protected:
      virtual void decode();
      virtual void resize_fe_data();

    };
    
  }
}
#endif
