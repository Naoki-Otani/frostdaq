// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__INPUT_STREAM_H
#define HDDAQ__INPUT_STREAM_H

#include <istream>
#include <bits/char_traits.h>

namespace hddaq
{
  namespace unpacker
  {

  template 
  <
    template <typename, typename> class StreamBuf,
    typename CharT, 
    typename Traits = std::char_traits<CharT> 
  >
  class basic_input_stream
    : public std::basic_istream<CharT, Traits>
  {

  public:
    typedef CharT                          char_type;
    typedef Traits                         traits_type;
    typedef typename traits_type::int_type int_type;
    typedef typename traits_type::pos_type pos_type;
    typedef typename traits_type::off_type off_type;

    typedef StreamBuf<char_type, traits_type>          buffer_type;
    typedef std::basic_istream<char_type, traits_type> istream_type;

  private:
    buffer_type m_buffer;

  public:
              basic_input_stream();
    explicit  basic_input_stream(const char* s,
				 std::ios_base::openmode mode 
				 = std::ios_base::in);
             ~basic_input_stream();

    void          close();
    bool          is_open();
    bool          is_open() const;
    void          open(const char* s,
		       std::ios_base::openmode mode = std::ios_base::in);
    buffer_type*  rdbuf() const;

  };

//______________________________________________________________________________
template 
<
  template <typename, typename> class StreamBuf,
  typename CharT, 
  typename Traits
>
inline
basic_input_stream<StreamBuf, CharT, Traits>::basic_input_stream()
  : istream_type(),
    m_buffer()
{
  this->init(&m_buffer);
}

//______________________________________________________________________________
template 
<
  template <typename, typename> class StreamBuf,
  typename CharT, 
  typename Traits
>
inline
basic_input_stream<StreamBuf, CharT, Traits>::
basic_input_stream(const char* s,
		  std::ios_base::openmode mode)
  : istream_type(),
    m_buffer()
{
  this->init(&m_buffer);
  this->open(s, mode);
}

//______________________________________________________________________________
template 
<
  template <typename, typename> class StreamBuf,
  typename CharT, 
  typename Traits
>
inline
basic_input_stream<StreamBuf, CharT, Traits>::~basic_input_stream()
{  
}

//______________________________________________________________________________
template 
<
  template <typename, typename> class StreamBuf,
  typename CharT, 
  typename Traits
>
inline
void
basic_input_stream<StreamBuf, CharT, Traits>::close()
{
  if (!m_buffer.close())
    this->setstate(std::ios_base::failbit);
  return;
}

//______________________________________________________________________________
template
<
  template <typename, typename> class StreamBuf,
  typename CharT,
  typename Traits
>
inline
bool
basic_input_stream<StreamBuf, CharT, Traits>::is_open()
{
  return m_buffer.is_open();
}

//______________________________________________________________________________
template
<
  template <typename, typename> class StreamBuf,
  typename CharT, 
  typename Traits
>
inline
bool
basic_input_stream<StreamBuf, CharT, Traits>::is_open() const
{
  return m_buffer.is_open();
}

//______________________________________________________________________________
template
<
  template <typename, typename> class StreamBuf,
  typename CharT,
  typename Traits
>
inline
void
basic_input_stream<StreamBuf, CharT, Traits>::open(const char* s,
						   std::ios_base::openmode mode)
{
  if (!m_buffer.open(s, mode | std::ios_base::in))
    this->setstate(std::ios_base::failbit);
  else 
    this->clear();
  return;
}

//______________________________________________________________________________
template
<
  template <typename, typename> class StreamBuf,
  typename CharT,
  typename Traits
>
inline
typename basic_input_stream<StreamBuf, CharT, Traits>::buffer_type*
basic_input_stream<StreamBuf, CharT, Traits>::rdbuf() const
{
  return const_cast<buffer_type*>(&m_buffer);
}
  
  }
}
#endif
