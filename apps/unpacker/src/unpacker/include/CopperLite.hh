// -*- C++ -*-

// Author: Hitoshi Sugimura

#ifndef HDDAQ__COPPERLITE_UNPACKER_H
#define HDDAQ__COPPERLITE_UNPACKER_H

#include "DAQNode.hh"

namespace hddaq
{
  namespace unpacker
  {

  class CopperLite 
    : public DAQNode,
      private Uncopyable<CopperLite>

  {

  public:
    static const uint32_t k_COPPERLITE_HEADER_MAGIC  = 0x7fff000aU;
    static const uint32_t k_COPPERLITE_TRAILER_MAGIC = 0x00030000U;
    static const uint32_t k_n_finesse                = 4;

    static const unpacker_type k_type;

    struct Header
    {
      uint32_t m_magic;
      uint32_t m_slot_id;
      // data size of finesse card 
      // from the finesse header to the finesse trailer (!! unit is byte !!)
      uint32_t m_length; 
      uint32_t m_id;
    };

    struct Trailer
    {
      uint32_t m_magic;
    };

  protected:
    Header*                m_header;
    Trailer*               m_trailer;
    
  public:
    CopperLite(const unpacker_type& type);
    virtual ~CopperLite();
    
  protected:
    //    virtual void decode();
    //    virtual void do_check();
    virtual void print_header() const;
    virtual void print_trailer() const;
    //    virtual void resize_fe_data();
    virtual void sort_child();
    virtual bool unpack();


  };

  }
}
#endif
