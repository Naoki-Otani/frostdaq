// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__TAG_H
#define HDDAQ__TAG_H

namespace hddaq
{
  namespace unpacker
  {
    
    const unsigned int k_FINESSE_MAX_LOCAL_TAG = 0xffffff;
    const unsigned int k_FINESSE_MAX_EVENT_TAG = 7U;
    const unsigned int k_FINESSE_MAX_SPILL_TAG = 3U;
    
    const unsigned int k_SMP_MAX_LOCAL_TAG     = 0xffffU;
    
    const unsigned int k_MTM_EVENT_TAG_ORIGIN  = 1U;
    const unsigned int k_MTM_MAX_EVENT_TAG     = 0xfffU;
    const unsigned int k_MTM_MAX_SPILL_TAG     = 0xffU;
    
    enum e_tag
      {
	k_local,
	k_event,
	k_spill,
	k_n_tag
      };

    enum e_tag_status
      {
	k_event_inherit,
	k_spill_inherit,
	k_n_tag_status
      };

    enum e_tag_type
      {
	k_tag_current,
	k_tag_prev,
	k_tag_slip,
	k_tag_diff,
	k_tag_origin,
	k_tag_max,
	k_n_tag_type
      };

    struct Tag
    {
      int m_local;
      int m_event;
      int m_spill;

      Tag();
      Tag(int local,
	  int event,
	  int spill);
      explicit Tag(int val);
      ~Tag();

    };

    void
    fill_tag(Tag& tag,
	     int value = -1);
  }
}
#endif
