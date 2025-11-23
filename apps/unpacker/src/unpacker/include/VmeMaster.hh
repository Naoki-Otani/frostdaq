// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__VME_MASTER_UNPACKER_H
#define HDDAQ__VME_MASTER_UNPACKER_H

#include "DAQNode.hh"

namespace hddaq
{
  namespace unpacker
  {
    class VmeMaster
      : public DAQNode,
	private Uncopyable<VmeMaster>
    {

    public:
      static const unpacker_type k_type;

      struct Header
      {
	uint32_t m_magic;
	uint32_t m_data_size;
	uint32_t m_nblock;
      };
      static const unsigned int k_header_size
      = sizeof(Header)/sizeof(uint32_t);
      
    protected:
      Header* m_header;
      std::deque<ChildList> m_modules;
      
    public:
      VmeMaster(const unpacker_type& type);
      virtual ~VmeMaster();
      
    protected:
      virtual void do_check();
      virtual void print_header() const;
      virtual bool unpack();
      
    };
    
  }
}
#endif
