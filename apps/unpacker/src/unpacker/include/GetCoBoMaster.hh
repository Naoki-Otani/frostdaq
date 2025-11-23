// -*- C++ -*-

// Author: Shuhei Hayakawa

#ifndef HDDAQ__GET_COBO_MASTER_UNPACKER_H
#define HDDAQ__GET_COBO_MASTER_UNPACKER_H

#include "DAQNode.hh"

namespace hddaq
{
namespace unpacker
{

//_____________________________________________________________________________
class GetCoBoMaster
  : public DAQNode,
    private Uncopyable<GetCoBoMaster>
{

public:
  static const unpacker_type k_type;
  static const uint32_t NumOfAsAd = 4;
  static const uint32_t k_COBO_HEADER_MAGIC = 0x436f426f; // "CoBo"

  struct Header
  {
    uint32_t m_magic;
    uint32_t m_cobo_id;
    uint32_t m_data_size;
    uint32_t m_nblock;
  };
  static const unsigned int k_header_size
  = sizeof(Header)/sizeof(uint32_t);

protected:
  Header*               m_header;
  std::deque<ChildList> m_modules;

public:
  GetCoBoMaster(const unpacker_type& type);
  virtual ~GetCoBoMaster();

protected:
  virtual void do_check();
  virtual void print_header() const;
  virtual bool unpack();

};

}
}

#endif
