// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__COPPER_UNPACKER_H
#define HDDAQ__COPPER_UNPACKER_H

#include "DAQNode.hh"

namespace hddaq
{
  namespace unpacker
  {

  class Copper 
    : public DAQNode,
      private Uncopyable<Copper>

  {

  public:
    static const unpacker_type k_type;

    struct Header
    {
      uint32_t m_magic;
      uint32_t m_data_size;
      uint32_t m_finesse_data_size[4];
    };

    struct Trailer
    {
      uint32_t m_magic;
    };
    

  protected:
    Header*                m_header;
    Trailer*               m_trailer;
    
  public:
    Copper(const unpacker_type& type);
    virtual ~Copper();
    
  protected:
    virtual void do_check();
    virtual void print_header() const;
    virtual void print_trailer() const;
    virtual void sort_child();
    virtual bool unpack();

  };

  }
}
#endif
