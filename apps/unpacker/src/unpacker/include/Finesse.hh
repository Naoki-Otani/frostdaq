// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__FINESSE_UNPACKER_H
#define HDDAQ__FINESSE_UNPACKER_H

#include "UnpackerImpl.hh"

namespace hddaq
{
  namespace unpacker
  {

  class Finesse
    : public UnpackerImpl,
      private Uncopyable<Finesse>
  {

  public:
    static const unpacker_type k_type;
    struct Header
    {
      uint32_t m_magic;
      uint32_t m_tag;
    };

    struct Trailer
    {
      uint32_t m_value;
    };

    static const uint32_t k_HEADER_MAGIC         = 0xffaa0000U;
    static const uint32_t k_HEADER_MAGIC1        = 0xffaa0200U;
    static const uint32_t k_HEADER_MAGIC2        = 0xffaa0400U;
    static const uint32_t k_HEADER_MAGIC3        = 0xffea0000U;
    static const uint32_t k_TRAILER_masked       = 0xff550000U;
    static const uint32_t k_LOCAL_TAG_mask       = 0xffffff00U;
    static const uint32_t k_LOCAL_TAG_shift      = 8U;
    static const uint32_t k_LOCAL_TAG_ORIGIN     = 0U;
    static const uint32_t k_EVENT_TAG_mask       = 0x7U;
    static const uint32_t k_EVENT_TAG_shift      = 0U;
    static const uint32_t k_SPILL_TAG_mask       = 0xc0U;
    static const uint32_t k_SPILL_TAG_shift      = 6U;
    static const unsigned int k_min_data_size 
    = (sizeof(Header) + sizeof(Trailer))/sizeof(uint32_t);
    static const unsigned int k_header_size = sizeof(Header)/sizeof(uint32_t);
    static const unsigned int k_trailer_size = sizeof(Trailer)/sizeof(uint32_t);

  protected:
    Header*  m_header;
    Trailer* m_trailer;
    iterator m_body_first;
    iterator m_body_last;

  public:
    Finesse(const unpacker_type& type);
    virtual ~Finesse();


  protected:
    virtual void     check_data_format();
    void             print_header();
    void             print_trailer();
    virtual bool     unpack();
    virtual void     update_tag();

  };

  }
}
#endif
