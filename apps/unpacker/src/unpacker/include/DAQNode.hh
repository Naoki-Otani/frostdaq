// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__DAQ_NODE_UNPACKER_H
#define HDDAQ__DAQ_NODE_UNPACKER_H

#include <vector>

#include "UnpackerImpl.hh"

namespace hddaq
{
  namespace unpacker
  {

    class Unpacker;

  class DAQNode
    : public UnpackerImpl,
      private Uncopyable<DAQNode>
  {

  public:
    static const unpacker_type k_type;
    static const unsigned int k_NODE_HEADER_MAGIC        = 0x45564e54U; // "EVNT"
    static const unsigned int k_NODE_EVENT_NUMBER_ORIGIN = 0U;

    enum e_node_header
      {
	k_magic,
	k_data_size,
	k_event_number,
	k_run_number,
	k_node_id,
	k_node_type,
	k_number_of_blocks,
	k_unix_time,
	k_n_node_header
      };
    
    struct Header
    {
      unsigned int m_magic;
      unsigned int m_data_size;
      unsigned int m_event_number;
      unsigned int m_run_number;
      unsigned int m_node_id;
      unsigned int m_node_type;
      unsigned int m_number_of_blocks;
      unsigned int m_unix_time;
    };

    static const unsigned int k_header_size
    = sizeof(Header)/sizeof(unsigned int);

  protected:
    Header*               m_node_header;
    std::deque<Unpacker*> m_child;

  public:
    DAQNode(const unpacker_type& type);
    virtual ~DAQNode();

    static void print_header(const Header* h);

  protected:
    virtual void check_data_format();
    virtual void do_check();
    virtual unsigned int
                 get_header(int data_type) const;
    virtual void print_header() const;
    void         print_node_header() const;
    virtual void print_trailer() const;
    virtual void sort_child();
    virtual bool unpack();
    virtual void update_tag();

  };
  
  }
}
#endif
