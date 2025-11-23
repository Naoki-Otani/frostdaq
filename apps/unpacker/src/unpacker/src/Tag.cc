// -*- C++ -*-

// Author: Tomonori Takahashi

#include "Tag.hh"
#include "defines.hh"

#include <algorithm>

namespace hddaq
{
namespace unpacker
{

//______________________________________________________________________________
  Tag::Tag()
    : m_local(defines::k_unassigned),
      m_event(defines::k_unassigned),
      m_spill(defines::k_unassigned)
  {
  }

//______________________________________________________________________________
  Tag::Tag(int local,
	   int event,
	   int spill)
    : m_local(local),
      m_event(event),
      m_spill(spill)
  {
  }

//______________________________________________________________________________
  Tag::Tag(int val)
    : m_local(val),
      m_event(val),
      m_spill(val)
  {
  }

//______________________________________________________________________________
  Tag::~Tag()
  {
  }

//______________________________________________________________________________
    void
    fill_tag(Tag& tag,
	     int value)
    {
      int* tag_begin = reinterpret_cast<int*>(&tag);
      int* tag_end   = tag_begin + sizeof(Tag)/sizeof(int);
      std::fill(tag_begin, tag_end, value);
      return;
    }
}
}
