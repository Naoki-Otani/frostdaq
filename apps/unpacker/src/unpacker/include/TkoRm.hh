// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__TKO_TRIGGER_RECEIVER_MODULE_UNPACKER_H
#define HDDAQ__TKO_TRIGGER_REVEIVER_MODULE_UNPACKER_H

#include <vector>

#include "Tko.hh"

namespace hddaq
{
  namespace unpacker
  {
    
  class TkoRm
    : public Tko,
      private Uncopyable<TkoRm>
  {
    
  public:
    static const unpacker_type k_type;
    static const unsigned int k_LOCK_BIT_mask = 0x8000U;

    enum e_data
      {
	k_event_tag,
	k_spill_tag,
	k_debug1,
	k_debug2,
	k_n_data
      };

  public:
    TkoRm(const unpacker_type& type);
    virtual ~TkoRm();

  protected:
    virtual void check_data_format();
    virtual void resize_fe_data();
//     virtual void set_data(unsigned int data);
    virtual void update_tag(); 

  };
    
  }
}

#endif
