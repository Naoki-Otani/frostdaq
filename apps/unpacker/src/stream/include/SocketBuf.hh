// -*- C++ -*-

// Author: Tomonori Takahashi

// seek functions are not implemented

#ifndef HDDAQ__SOCKET_BUF_H
#define HDDAQ__SOCKET_BUF_H

#include <bits/char_traits.h>
#include <iterator>
#include <sstream>
#include <streambuf>
#include <string>
#include <iostream>

#include <cerrno>
#include <cstdlib>
#include <cstring>

#include "KOLSocket.hh"
#include "KOLSocketException.hh"
#include "Tokenizer.hh"
//#include "create_socket.hh"

namespace hddaq
{
  namespace unpacker
  {

  template<typename CharT, typename Traits = std::char_traits<CharT> >
  class basic_socket_buf 
    : public std::basic_streambuf<CharT, Traits>
  {
  
    static const unsigned int k_Buffer_Size = 8912;

  public:
    typedef CharT                     	        char_type;
    typedef Traits                    	        traits_type;
    typedef typename traits_type::int_type      int_type;
    typedef typename traits_type::pos_type 	pos_type;
    typedef typename traits_type::off_type 	off_type;
    
    typedef std::basic_streambuf<char_type, traits_type>  streambuf_type;
    typedef basic_socket_buf<char_type, traits_type>      socketbuf_type;
    
    friend class std::ios_base; // For sync_with_stdio.
    
  protected:
    kol::Socket*            m_socket;
    std::ios_base::openmode m_mode;
    char_type*              m_recvbuf;
    std::size_t             m_recvbuf_size;
    bool                    m_recvbuf_allocated;  
    char_type*              m_sendbuf;
    std::size_t             m_sendbuf_size;
    bool                    m_sendbuf_allocated;
    bool                    m_reading;
    bool                    m_writing;
    char_type	            m_pback; 
    char_type*              m_pback_cur_save;
    char_type*              m_pback_end_save;
    bool		    m_pback_init; 
//     char_type*           m_ext_buf;
//     std::streamsize      m_ext_buf_size;
//     const char*	    m_ext_next;
//     char*		    m_ext_end;
    
  public:
             basic_socket_buf();
    virtual ~basic_socket_buf();
    
    socketbuf_type* close() throw();
    bool            is_open() const throw();
    socketbuf_type* open(const char* s, 
			 std::ios_base::openmode mode);
    socketbuf_type* open(kol::Socket* socket,
			 std::ios_base::openmode mode);
    void            set_socket(kol::Socket* socket, 
			       std::ios_base::openmode mode);
    
  protected:
    void                    allocate_internal_buffer();
    bool                    convert_to_external(char_type* s, 
						std::streamsize n);
    void                    create_pback();
    void                    destroy_internal_buffer() throw();
    void                    destroy_pback() throw();
    virtual int_type        overflow(int_type c = Traits::eof());
//     virtual int_type        pbackfail(int_type c = Traits::eof());
//     pos_type                seek(off_type off, 
// 				 std::ios_base::seekdir way);
//     virtual pos_type        seekoff(off_type off, 
// 				    std::ios_base::seekdir way,
// 				    std::ios_base::openmode mode
// 				    = std::ios_base::in | std::ios_base::out);
//     virtual pos_type        seekpos(pos_type pos,
// 				    std::ios_base::openmode mode 
// 				    = std::ios_base::in | std::ios_base::out);
    virtual streambuf_type* setbuf(char_type* s,
				   std::streamsize n);
    void                    set_buffer(std::streamsize off);
    virtual std::streamsize showmanyc();
    virtual int             sync();
    bool                    terminate_output();
    virtual int_type        underflow();
//     virtual std::streamsize  xsgetn(char_type* s, std::streamsize n);
//     virtual std::streamsize  xsputn(const char_type* s, std::streamsize n);

  private:
    kol::Socket* create_socket(const std::string& host,
			       int port);
    int          recv();

  };

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
basic_socket_buf<CharT, Traits>::basic_socket_buf()
  : streambuf_type(),
    m_socket(0),
    m_mode(std::ios_base::openmode(0)),
    m_recvbuf(0),
    m_recvbuf_size(k_Buffer_Size),
    m_recvbuf_allocated(false),
    m_sendbuf(0),
    m_sendbuf_size(k_Buffer_Size),
    m_sendbuf_allocated(false),
    m_reading(false),
    m_writing(false),
    m_pback(),
    m_pback_cur_save(0),
    m_pback_end_save(0),
    m_pback_init(false)
//     m_ext_buf(0),
//     m_ext_buf_size(0),
//     m_ext_next(0),
//     m_ext_end(0)
{
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
basic_socket_buf<CharT, Traits>::~basic_socket_buf()
{
  this->close();
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
void
basic_socket_buf<CharT, Traits>::allocate_internal_buffer()
{
  if (m_mode & std::ios_base::in)
    if (!m_recvbuf_allocated && !m_recvbuf)
      {
	m_recvbuf = new char_type[m_recvbuf_size];
	m_recvbuf_allocated = true;
      }
  if (m_mode & std::ios_base::out)
    if (!m_sendbuf_allocated && !m_sendbuf)
      {
	m_sendbuf = new char_type[m_sendbuf_size];
	m_sendbuf_allocated = true;
      }
  return;
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
typename basic_socket_buf<CharT, Traits>::socketbuf_type*
basic_socket_buf<CharT, Traits>::close() throw()
{
  socketbuf_type* ret=0;
  if (this->is_open())
    {
      if (0==this->sync()) 
	{
	  if (0==m_socket->shutdown())
	    ret = this;
	  if (m_socket)
	    {
	      delete m_socket;
	      m_socket = 0;
	    }
	}
    }
  return ret;
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
bool
basic_socket_buf<CharT, Traits>::convert_to_external(char_type* s,
						     std::streamsize n)
{
  std::streamsize elen;
  std::streamsize plen;
  elen = m_socket->send(reinterpret_cast<char*>(s),
			n);
  plen = n;
  return (elen == plen);
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
void
basic_socket_buf<CharT, Traits>::create_pback()
{
  if (!m_pback_init)
    {
      m_pback_cur_save = this->gptr();
      m_pback_end_save = this->egptr();
      this->setg(&m_pback, &m_pback, &m_pback + 1);
      m_pback_init = true;
    }
  return;
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
kol::Socket*
basic_socket_buf<CharT, Traits>::create_socket(const std::string& host,
					       int port)
{
  // client side
//   std::cout << "#D host = " << host 
// 	    << ", port = "  << port
// 	    << std::endl;
  kol::Socket* client_socket = new kol::Socket(PF_INET, SOCK_STREAM, 0);
  try
    {
      struct sockaddr_in  srvaddr;
      struct sockaddr_in* resaddr;
      struct addrinfo     hints;
      struct addrinfo*    res;
      int                 err;
      
      res = 0;
      ::memset(reinterpret_cast<char*>(&hints), 0, sizeof(hints));
      hints.ai_family   = PF_INET;
      hints.ai_socktype = SOCK_STREAM;
      hints.ai_protocol = 0;
      if((err = ::getaddrinfo(host.c_str(), 0, &hints, &res)) != 0)
	throw kol::SocketException("#E @ create_socket  ::getaddrinfo error");
      resaddr = reinterpret_cast<sockaddr_in*>(res->ai_addr);
      
      ::memset(reinterpret_cast<char*>(&srvaddr), 0, sizeof(srvaddr));
      srvaddr.sin_family = AF_INET;
      //srvaddr.sin_port = ::htons((u_short)port);
      srvaddr.sin_port = htons(static_cast<u_short>(port));
      srvaddr.sin_addr = resaddr->sin_addr;
      ::freeaddrinfo(res);
      client_socket->connect(reinterpret_cast<sockaddr*>(&srvaddr),
			     sizeof(srvaddr));
      return client_socket;
    }
  catch(const kol::SocketException& socket_exception)
    {
      std::cout << socket_exception.what() << std::endl;
      if (client_socket)
	{
	  client_socket->close();
	  delete client_socket;
	  client_socket = 0;
	}
      return client_socket;
    }
  catch(...)
    {
      if (client_socket)
	{
	  client_socket->close();
	  delete client_socket;
	  client_socket = 0;
	}
      throw;
    }
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
void
basic_socket_buf<CharT, Traits>::destroy_internal_buffer() throw()
{
  if (m_recvbuf_allocated)
    {
      if (m_recvbuf)
	{
	  delete [] m_recvbuf;
	  m_recvbuf = 0;
	}
      m_recvbuf_allocated = false;
    }

  if (m_sendbuf_allocated)
    {
      if (m_sendbuf)
	{
	  delete [] m_sendbuf;
	  m_sendbuf = 0;
	}
      m_sendbuf_allocated = false;
    }

//   delete [] m_ext_buf;
//   m_ext_buf = 0;
//   m_ext_buf_size = 0;
//   m_ext_next     = 0;
//   m_ext_end      = 0;
  return;
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
void
basic_socket_buf<CharT, Traits>::destroy_pback() throw()
{
  if (m_pback_init)
    {
      m_pback_cur_save += (this->gptr() != this->eback());
      this->setg(m_recvbuf, m_pback_cur_save, m_pback_end_save);
      m_pback_init = false;
    }
  return;
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
bool
basic_socket_buf<CharT, Traits>::is_open() const throw()
{
  return (0 != m_socket);
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
typename basic_socket_buf<CharT, Traits>::socketbuf_type*
basic_socket_buf<CharT, Traits>::open(const char* s, 
				      std::ios_base::openmode mode)
{
  socketbuf_type* ret = 0;
  if (!this->is_open())
    {
      std::string input = s;
//       std::cout << "#D input = " << input << std::endl;
      Tokenizer::result_type host_port;
      Tokenizer::tokenize(input, host_port, ":");
      if (2 != host_port.size())
	{
	  std::cerr << "#E ";
	  std::copy(host_port.begin(), host_port.end(),
		    std::ostream_iterator<std::string> (std::cout, " "));
	  std::cerr << " :: invalid name as hostname:port" << std::endl;
	  std::exit(0);
	}
      std::stringstream port_info(host_port.back());
      std::string hostname = host_port.front();
      int         port;
      port_info >> port;
      std::cout << "#D connect to hostname = " << hostname
		<< ", port = " << port << std::endl;
//       if (std::ios_base::in & mode)
// 	{
// 	  std::cout << "#D create isocket (client socket) @ " 
// 		    << hostname << ":" << port << std::endl;
// 	  m_socket = create_socket(hostname, port);
// 	}
//       else if (std::ios_base::out & mode)
// 	{
// 	  std::cout << "#D create osocket (server socket) : " 
// 		    << port << std::endl;
// 	  m_socket = create_socket(port);
// 	}
      if ((std::ios_base::in | std::ios_base::out) & mode)
 	{
//  	  std::cout << "#D create iosocket (client socket) @ " 
//  		    << hostname << ":" << port << std::endl;
 	  m_socket = create_socket(hostname, port);

// 	  struct timeval timeoutv;
// 	  timeoutv.tv_sec  = 1;
// 	  timeoutv.tv_usec = 0;
// 	  m_socket->setsockopt(SOL_SOCKET, SO_RCVTIMEO,
// 			       &timeoutv, sizeof(timeoutv));
 	}
    }

  
  if (this->is_open())
    {
      m_mode    = mode;
      allocate_internal_buffer();
      m_reading = false;
      m_writing = false;
      set_buffer(-1);

      ret = this;
    }
  return ret;
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
typename basic_socket_buf<CharT, Traits>::socketbuf_type*
basic_socket_buf<CharT, Traits>::open(kol::Socket* socket,
				      std::ios_base::openmode mode) 
{
  socketbuf_type* ret = 0;
  m_socket = socket;
  if (this->is_open())
    {
      m_mode    = mode;
      allocate_internal_buffer();
      m_reading = false;
      m_writing = false;
      set_buffer(-1);

      ret = this;
    }
  return ret;
}

//______________________________________________________________________________
template <typename CharT, typename Traits> 
inline
typename basic_socket_buf<CharT, Traits>::int_type 
basic_socket_buf<CharT, Traits>::overflow(int_type c)
{
//   std::cout << "#D buf size = " << m_buf_size << std::endl;
  int_type ret = traits_type::eof();
  const bool test_eof = traits_type::eq_int_type(c, ret);
  const bool test_out = m_mode & std::ios_base::out;
  if (test_out && !m_reading)
    {
      if (this->pbase() < this->pptr())
	{
	  if (!test_eof)
	    {
	      *this->pptr() = traits_type::to_char_type(c);
	      this->pbump(1);
	    }
	  if (convert_to_external(this->pbase(),
				  this->pptr() - this->pbase()))
	    {
	      set_buffer(0);
	      ret = traits_type::not_eof(c);
	    }
	}
      else if (m_sendbuf_size > 1)
	{	
	  set_buffer(0);
	  m_writing = true;
	  if (!test_eof)
	    {
	      *this->pptr() = traits_type::to_char_type(c);
	      this->pbump(1);
	    }
	  ret = traits_type::not_eof(c);
	}
      else
	{
	  char_type conv = traits_type::to_char_type(c);
	  if (test_eof || convert_to_external(&conv, 1))
	    {
	      m_writing = true;
	      ret = traits_type::not_eof(c);
	    }
	}
    }
  return ret;
}

//______________________________________________________________________________
// template <typename CharT, typename Traits>
// typename basic_socket_buf<CharT, Traits>::int_type
// basic_socket_buf<CharT, Traits>::pbackfail(int_type c)
// {
//   int_type ret = traits_type::eof();
//   const bool test_in = m_mode & std::ios_base::in;
//   if (test_in && !m_writing)
//     {
//       const bool test_pb  = m_pback_init;
//       const bool test_eof = traits_type::eq_int_type(c, ret);
//       int_type tmp;
//       if (this->eback() < this->gptr())
// 	{
// 	  this->gbump(-1);
// 	  tmp = traits_type::to_int_type(*this->gptr());
// 	}
//       else if (this->seekoff(-1, std::ios_base::cur) != pos_type(off_type(-1)))
// 	{
// 	  tmp = this->underflow();
// 	  if (traits_type::eq_int_type(tmp, ret))
// 	    return ret;
// 	}
//       else 
// 	{
// 	  return ret;
// 	}

//       if (!test_eof && traits_type::eq_int_type(c, tmp))
// 	ret = c;
//       else if (test_eof)
// 	ret = traits_type::not_eof(c);
//       else if (!test_pb)
// 	{
// 	  create_pback();
// 	  m_reading = true;
// 	  *this->gptr() = traits_type::to_char_type(c);
// 	  ret = c;
// 	}
//     }
//   return ret;
// }

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
int
basic_socket_buf<CharT, Traits>::recv()
{
  const std::size_t buf_len = (m_recvbuf_size > 1) ? (m_recvbuf_size - 1) : 1;
  std::streamsize nbytes_read = 0;
//   try 
//     {
      nbytes_read = m_socket->recv(reinterpret_cast<char*>(this->eback()),
				   buf_len);
//     }
//   catch (kol::SocketException& e)
//     {
//       if (e.reason() == EWOULDBLOCK)
// 	{
// 	  std::cerr << "#E SocketBuf::recv()\n retry" << std::endl;
// 	  nbytes_read = recv();
// 	}
//     }
  return nbytes_read;
}


//______________________________________________________________________________
// template <typename CharT, typename Traits>
// typename basic_socket_buf<CharT, Traits>::pos_type
// basic_socket_buf<CharT, Traits>::seek(off_type off,
// 				      std::ios_base::seekdir way)
// {
//   pos_type ret = pos_type(off_type(-1));
//   return ret;
// }

//______________________________________________________________________________
// template <typename CharT, typename Traits>
// typename basic_socket_buf<CharT, Traits>::pos_type
// basic_socket_buf<CharT, Traits>::seekoff(off_type off,
// 					 std::ios_base::seekdir way,
// 					 std::ios_base::openmode mode)
// {
//   int width = 0;
//   pos_type ret = pos_type(off_type(-1));
//   const bool test_fail = (0 != off && width <=0);
//   if (this->is_open() && !test_fail)
//     {
//       destroy_pback();
//       off_type computed_off = off * width;
//       if (m_reading && way == std::ios_base::cur)
// 	{
// 	  computed_off += this->gptr() - this->egptr();
// 	}
//       ret = seek(computed_off, way);
//     }
//   return ret;
// }

//______________________________________________________________________________
// template <typename CharT, typename Traits>
// typename basic_socket_buf<CharT, Traits>::pos_type
// basic_socket_buf<CharT, Traits>::seekpos(pos_type pos,
// 					 std::ios_base::openmode mode)
// {
//   pos_type ret = pos_type(off_type(-1));
//   if (this->is_open())
//     {
//       destroy_pback();
//       ret = seek(off_type(pos), std::ios_base::beg);
//     }
//   return ret;
// }

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
typename basic_socket_buf<CharT, Traits>::streambuf_type*
basic_socket_buf<CharT, Traits>::setbuf(char_type* s, 
					std::streamsize n)
{
  if (m_mode & std::ios_base::in){
    if (!this->is_open()){
      if (0==s && 0==n){
	m_recvbuf_size = 1;
      }else if (s && n > 0){
	m_recvbuf      = s;
	m_recvbuf_size = n;
      }
    }
      
  }
  if (m_mode & std::ios_base::out){
    if (!this->is_open()){
      if (0==s && 0==n){
	m_sendbuf_size = 1;
      }else if (s && n > 0){
	m_sendbuf      = s;
	m_sendbuf_size = n;
      }
    }
  }
  
  return this;
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
void
basic_socket_buf<CharT, Traits>::set_buffer(std::streamsize off)
{
  const bool test_in  = m_mode & std::ios_base::in;
  const bool test_out = m_mode & std::ios_base::out;
  
  if (test_in && off > 0)
    this->setg(m_recvbuf, m_recvbuf, m_recvbuf + off);
  else
    this->setg(m_recvbuf, m_recvbuf, m_recvbuf);
      
  if (test_out && off == 0 && m_sendbuf_size > 1 )
    this->setp(m_sendbuf, m_sendbuf + m_sendbuf_size - 1);
  else
    this->setp(0, 0);
  return;
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
void
basic_socket_buf<CharT, Traits>::set_socket(kol::Socket* socket,
					    std::ios_base::openmode mode)
{
  open(socket, mode);
  return;
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
std::streamsize
basic_socket_buf<CharT, Traits>::showmanyc()
{
  std::streamsize ret = -1;
  const bool test_in = m_mode & std::ios_base::in;
  if (test_in && this->is_open())
    ret = this->egptr() - this->gptr();
  return ret;
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
int
basic_socket_buf<CharT, Traits>::sync()
{
  int ret = 0;
  if (this->pbase() < this->pptr())
    {
      const int_type tmp = this->overflow();
      if (traits_type::eq_int_type(tmp, traits_type::eof()))
	ret = -1;
    }
  return ret;
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
bool
basic_socket_buf<CharT, Traits>::terminate_output()
{
  bool test_valid = true;

  if (this->pbase() < this->pptr())
    {
      const int_type tmp = this->overflow();
      if (traits_type::eq_int_type(tmp, traits_type::eof()))
	test_valid = false;
    }
  
  if (m_writing && test_valid)
    {
      const int_type tmp = this->overflow();
      if (traits_type::eq_int_type(tmp, traits_type::eof()))
	test_valid = false;
    }
  return test_valid;
}

//______________________________________________________________________________
template <typename CharT, typename Traits>
inline
typename basic_socket_buf<CharT, Traits>::int_type
basic_socket_buf<CharT, Traits>::underflow()
{
//   std::cout << "#D buf size = " << m_buf_size << std::endl;
  int_type ret = traits_type::eof();
  const bool test_in = m_mode & std::ios_base::in;
  if (test_in && !m_writing)
    {
      destroy_pback();
      if (this->gptr() < this->egptr())
	return traits_type::to_int_type(*this->gptr());

//       const std::size_t buf_len = (m_buf_size > 1) ? (m_buf_size - 1) : 1;
      bool got_eof = false;
      std::streamsize nbytes_read = 0;
//       nbytes_read = m_socket->recv(reinterpret_cast<char*>(this->eback()),
// 				   buf_len);

      nbytes_read = recv();

      if (0==nbytes_read)
	got_eof = true;

      if (nbytes_read>0)
	{
	  set_buffer(nbytes_read);
	  m_reading = true;
	  ret = traits_type::to_int_type(*this->gptr());
	}
      else if (got_eof)
	{
	  set_buffer(-1);
	  m_reading = false;
	}
    }
  return ret;
}

//______________________________________________________________________________
typedef basic_socket_buf<char> SocketBuf;

  }
}
#endif
