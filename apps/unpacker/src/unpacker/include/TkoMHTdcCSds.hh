// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__TKO_MULTI_HIT_TDC_WITH_COMPARE_SPARSE_DATA_SCAN_UNPACKER_H
#define HDDAQ__TKO_MULTI_HIT_TDC_WITH_COMPARE_SPARSE_DATA_SCAN_UNPACKER_H

#include "TkoMHTdc.hh"

namespace hddaq
{
  namespace unpacker
  {

    class TkoMHTdcCSds
      : public TkoMHTdc,
	private Uncopyable<TkoMHTdcCSds>
    {

    public:
      static const unpacker_type k_type;

    public:
      TkoMHTdcCSds(const unpacker_type& type);
      virtual ~TkoMHTdcCSds();


    protected:
      virtual void decode();

    };

  }
}
#endif
