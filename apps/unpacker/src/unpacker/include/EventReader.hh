// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__EVENT_READER_H
#define HDDAQ__EVENT_READER_H

#include <stdint.h>
#include <string>
#include <vector>

#include "DAQNode.hh"
#include "Uncopyable.hh"

namespace hddaq
{
  namespace unpacker
  {

  class IStream;

  class EventReader
    : private Uncopyable<EventReader>
  {

  public:
    typedef std::vector<uint32_t>                 event_buf_t;
    typedef std::vector<uint32_t>::iterator       iterator;
    typedef std::vector<uint32_t>::const_iterator const_iterator;
    typedef std::vector<uint32_t>::size_type      size_type;

  private:
    std::string                         m_stream_name;
    event_buf_t                         m_buffer;
    iterator                            m_begin;
    iterator                            m_end;
    IStream*                            m_stream;
    DAQNode::Header*                    m_header;
    IStream*                            m_bookmark;

  public:
     EventReader();
    ~EventReader();

    void               hoge(const std::string& arg="") const;
    void               clear();
    void               close();
    void               dump_header() const;
    void               dump_in_binary() const;
    void               dump_in_decimal() const;
    void               dump_in_hexadecimal() const;
    bool               eof() const;
    unsigned int       get_bookmark(int event_number);
    unsigned int       get_daq_root_event_number() const;
    unsigned int       get_daq_root_run_number() const;
    unsigned int       get_root_id() const;
    const std::string& get_stream_type() const;
    bool               is_open() const;
    void               open(const std::string& stream_name);
    bool               read(bool skip_flag=false);
    bool               read_multi_events(int n_events);
    void               set_bookmark(const std::string& bookmark_name);
    int                skip(int n_skip);
    uint64_t           tellg();
    bool               unpack();

  };

  }
}
#endif
