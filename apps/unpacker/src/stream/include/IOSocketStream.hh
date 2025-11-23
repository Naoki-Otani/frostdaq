// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__IO_SOCKET_STREAM_H
#define HDDAQ__IO_SOCKET_STREAM_H

#include <istream>
#include <bits/char_traits.h>

#include "SocketBuf.hh"

namespace hddaq
{
  namespace unpacker
  {
  
  template <typename CharT, typename Traits = std::char_traits<CharT> >
  class basic_socket_stream : public std::basic_iostream<CharT, Traits>
  {

  public:
    typedef CharT                          char_type;
    typedef Traits                         traits_type;
    typedef typename traits_type::int_type int_type;
    typedef typename traits_type::pos_type pos_type;
    typedef typename traits_type::off_type off_type;

    typedef basic_socket_buf<char_type, traits_type>      buffer_type;
    typedef std::basic_ios<char_type, traits_type>        ios_type;
    typedef std::basic_iostream<char_type, traits_type>   iostream_type;

  private:
    buffer_type m_buffer;

  public:
              basic_socket_stream();
              basic_socket_stream(kol::Socket* socket,
				  std::ios_base::openmode mode
				  = std::ios_base::in
				  | std::ios_base::out);
    explicit  basic_socket_stream(const char* s,
				  std::ios_base::openmode mode 
				  = std::ios_base::in
				  | std::ios_base::out);
             ~basic_socket_stream();
    
    void         close();
    bool         is_open();
    bool         is_open() const;
    void         open(const char* s,
		      std::ios_base::openmode mode 
		      = std::ios_base::in | std::ios_base::out);
    void         open(kol::Socket* socket,
		      std::ios_base::openmode mode
		      = std::ios_base::in | std::ios_base::out);
    buffer_type* rdbuf() const;
    void         set_socket(kol::Socket* socket,
			    std::ios_base::openmode mode
			    = std::ios_base::in | std::ios_base::out);

  };
  
//______________________________________________________________________________
template <typename CharT, typename Traits>
basic_socket_stream<CharT, Traits>::basic_socket_stream()
  : iostream_type(),
    m_buffer()
{
  this->init(&m_buffer);
}
  
//______________________________________________________________________________
template <typename CharT, typename Traits>
basic_socket_stream<CharT, Traits>::
basic_socket_stream(const char* s,
		    std::ios_base::openmode mode)
  : iostream_type(NULL),
    m_buffer()
{
  this->init(&m_buffer);
  this->open(s, mode);
}							 

//______________________________________________________________________________
template <typename CharT, typename Traits>
basic_socket_stream<CharT, Traits>::
basic_socket_stream(kol::Socket* socket, 
                    std::ios_base::openmode mode)
  : iostream_type(NULL),
    m_buffer()
{
  this->init(&m_buffer);
  this->open(socket, mode);
}							 

//______________________________________________________________________________
template <typename CharT, typename Traits>
basic_socket_stream<CharT, Traits>::~basic_socket_stream()
{
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
void
basic_socket_stream<CharT, Traits>::close()
{
  if (!m_buffer.close())
    this->setstate(std::ios_base::failbit);
  return;
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
bool
basic_socket_stream<CharT, Traits>::is_open()
{
  return m_buffer.is_open();
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
bool
basic_socket_stream<CharT, Traits>::is_open() const
{
  return m_buffer.is_open();
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
void
basic_socket_stream<CharT, Traits>::open(const char* s,
					 std::ios_base::openmode mode)
{
  if (m_buffer.open(s, mode))
    this->setstate(std::ios_base::failbit);
  else
    this->clear();
  return;
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
void
basic_socket_stream<CharT, Traits>::open(kol::Socket* socket,
					 std::ios_base::openmode mode)
{
  if (m_buffer.open(socket, mode))
    this->setstate(std::ios_base::failbit);
  else
    this->clear();
  return;
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
typename basic_socket_stream<CharT, Traits>::buffer_type*
basic_socket_stream<CharT, Traits>::rdbuf() const
{
  return const_cast<buffer_type*>(&m_buffer);
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
void
basic_socket_stream<CharT, Traits>::set_socket(kol::Socket* socket,
					       std::ios_base::openmode mode)
{
  open(socket, mode);
  return;
}


//______________________________________________________________________________
typedef basic_socket_stream<char> IOSocketStream;

  }
}
#endif
