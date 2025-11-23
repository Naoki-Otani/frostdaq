// -*- C++ -*-

#ifndef HDDAQ__FIELD_UNPCKER_H
#define HDDAQ__FIELD_UNPCKER_H

#include "DAQNode.hh"

namespace hddaq
{

namespace unpacker
{

  class Unpacker;

  class TeslaMeter
    : public DAQNode,
      private Uncopyable<TeslaMeter>
  {

  public:
    static const unpacker_type k_type;

    struct Format
    {
      uint32_t magic;
      uint32_t id;
      uint32_t length;
      char     data[16];
      uint64_t tv_sec;
      uint64_t tv_nsec;
      uint32_t nread;
      uint32_t nsend;
    };
    
    static const unsigned int k_format_size = sizeof(Format)/sizeof(uint32_t);
    
  private:
    Format* m_data;

  public:
    TeslaMeter(const unpacker_type& type);
    virtual ~TeslaMeter();

    static void print(const Unpacker* u);
    static const Format* get_data(const Unpacker* u);

  protected:
    virtual void print() const;
    virtual bool unpack();

  };

}


}

#endif
