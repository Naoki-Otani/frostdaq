// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__O_STREAM_H
#define HDDAQ__O_STREAM_H

#include <bits/char_traits.h>
#include <ostream>
#include <string>

#include "Uncopyable.hh"

namespace hddaq
{
  namespace unpacker
  {

  class OStream
    : private Uncopyable<OStream>
  {

  public:
    typedef char                   char_type;
    typedef std::char_traits<char> traits_type;
    typedef traits_type::int_type  int_type;
    typedef traits_type::pos_type  pos_type;
    typedef traits_type::off_type  off_type;

    typedef std::ostream::__streambuf_type streambuf_type;
    typedef std::ostream::__ostream_type   ostream_type;

  private:
    std::ostream*   m_stream;
    std::string     m_stream_name;
    std::string     m_stream_type;
    bool            m_is_bad;
    bool            m_is_eof;
    bool            m_is_fail;
    bool            m_is_good;

  public:
     OStream();
     OStream(const std::string& s,
	     std::ios_base::openmode mode 
	     = std::ios_base::out 
	     | std::ios_base::trunc);
    ~OStream();

    bool             bad() const;
    void             close();
    bool             eof() const;
    bool             fail() const;
    ostream_type&    flush();
    bool             good() const;
    bool             is_open() const;
    void             open(const std::string& stream_name, 
			  std::ios_base::openmode mode 
			  = std::ios_base::out
			  | std::ios_base::trunc);
//     stream_type& put();
//     stream_type& seekp(pos_type pos);
//     stream_type& seekp(off_type off, std::ios_base::seekdir way);
//     pos_type     tellp()
    ostream_type&    write(char_type* s, std::streamsize n);

  };

  }
}
#endif
