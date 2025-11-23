// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__I_STREAM_H
#define HDDAQ__I_STREAM_H

#include <bits/char_traits.h>
#include <istream>
#include <string>

#include "Uncopyable.hh"

namespace hddaq
{
  namespace unpacker
  {

    const std::string k_stream_type_socket     = "socket";
    const std::string k_stream_type_bzip2_file = ".bz2";
    const std::string k_stream_type_gzip_file  = ".gz";
    const std::string k_stream_type_dat_file   = ".dat";
    const std::string k_stream_type_std_cin    = "std::cin";

  class IStream
    : private Uncopyable<IStream>
  {

  public:
    typedef char                        char_type;
    typedef std::char_traits<char_type> traits_type;
    typedef traits_type::int_type       int_type;
    typedef traits_type::pos_type       pos_type;
    typedef traits_type::off_type       off_type;

    typedef std::istream::__streambuf_type streambuf_type;
    typedef std::istream::__istream_type   istream_type;

  private:
    std::istream*   m_stream;
    std::string     m_stream_name;
    std::string     m_stream_type;
    bool            m_is_bad;
    bool            m_is_eof;
    bool            m_is_fail;
    bool            m_is_good;
    std::streamsize m_gcount;

  public:
     IStream();
     IStream(const std::string& stream_name = "",
	     std::ios_base::openmode mode = std::ios_base::in);
    ~IStream();

    bool             bad() const;
    void             close();
    bool             eof() const;
    bool             fail() const;
    std::streamsize  gcount() const;
//     int_type get();
//     stream_type& get(char_type* s, std::streamsize n, char_type delim);
//     stream_type& get(char_type* s, std::streamsizen);
//     stream_type& get(streambuf_type& stream_buf, char_type delim);
//     stream_type& get(streambuf_type& stream_buf);
//     stream_type& getline(char_type* s, std::streamsize n, char_type delim);
//     stream_type& getline(char_type* s, std::streamsize n);
    const std::string& get_stream_type() const;
    bool             good() const;
    istream_type&    ignore();
    istream_type&    ignore(std::streamsize n);
    istream_type&    ignore(std::streamsize n,
			    int_type delim /*= traits_type::eof()*/);
    bool             is_open() const;
    void             open(const std::string& stream_name,
			  std::ios_base::openmode mode = std::ios_base::in);
//     int_type     peek();
//     stream_type& putback();
    istream_type&    read(char_type* s, std::streamsize n);
//     std::streamsize readsome(char_type* s, std::streamsize n);
//     stream_type& seekg(pos_type pos);
//     stream_type& seekg(off_type off, std::ios_base::seekdir way);
    int              sync();
//     stream_type& unget();
    pos_type         tellg();

  };

  }
}
#endif
