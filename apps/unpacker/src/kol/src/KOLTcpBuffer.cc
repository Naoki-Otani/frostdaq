// -*- C++ -*-
//
// @file   koltcp.cc
// @brief  
// @author Hirofumi Fujii <hirofumi.fujii@kek.jp>
// @date   Mon Nov 27 05:54:48 2006 UTC
//
// $Id: koltcp.h,v 1.1.1.1 2007/03/28 07:50:17 cvs Exp $
// $Log: koltcp.h,v $
// Revision 1.1.1.1  2007/03/28 07:50:17  cvs
// prototype-1.2
//
// Revision 1.1.1.1  2007/01/31 13:37:53  kensh
// Initial version.
//

#include "KOLTcpBuffer.hh"

namespace kol
{

//______________________________________________________________________________
TcpBuffer::TcpBuffer() 
  : m_socket(),
    m_gcount(),
    m_iostate(),
    m_rbufmax(),
    m_rbuflen(),
    m_rbuf(),
    m_sbufmax(),
    m_sbuflen(),
    m_sbufnxt(),
    m_sbuf()
{
  initparams();
}

//______________________________________________________________________________
TcpBuffer::TcpBuffer(const Socket& s)
  : m_socket(s),
    m_gcount(),
    m_iostate(),
    m_rbufmax(),
    m_rbuflen(),
    m_rbuf(),
    m_sbufmax(),
    m_sbuflen(),
    m_sbufnxt(),
    m_sbuf()
{
  initparams();
}

//______________________________________________________________________________
TcpBuffer::TcpBuffer(int domain, 
		     int type,
		     int protocol) 
  :  m_socket( domain, type, protocol ),
     m_gcount(),
     m_iostate(),
     m_rbufmax(),
     m_rbuflen(),
     m_rbuf(),
     m_sbufmax(),
     m_sbuflen(),
     m_sbufnxt(),
     m_sbuf()
{
  initparams();
}

//______________________________________________________________________________
TcpBuffer::~TcpBuffer()
{
  m_socket.close();
}

//______________________________________________________________________________
void
TcpBuffer::initparams()
{
  m_gcount = 0;
  m_iostate = goodbit;
  m_rbufmax = bufsize;
  m_rbuflen = 0;
  m_rbufnxt = 0;
  m_sbufmax = bufsize;
  m_sbuflen = 0;
  m_sbufnxt = 0;
}

//______________________________________________________________________________
int
TcpBuffer::close()
{
  flush();
  m_socket.close();
  return 0;
}

//______________________________________________________________________________
TcpBuffer&
TcpBuffer::flush()
{
  if(m_sbufnxt > 0)
  {
    send_all(m_sbuf, m_sbufnxt);
    m_sbufnxt = 0;
    m_sbuflen = 0;
  }
  return *this;
}

//______________________________________________________________________________
int
TcpBuffer::get()
{
  int c;

  if(m_rbufnxt >= m_rbuflen)
  {
    m_rbuflen = 0;
    m_rbufnxt = 0;
    int n = m_socket.recv(m_rbuf, m_rbufmax);
    if(n <= 0)
    {
      m_gcount = 0;
      m_iostate |= eofbit;
      return std::char_traits<char>::eof();
    }
    m_rbuflen = n;
  }
  c = (static_cast<int>(m_rbuf[m_rbufnxt++])) & 255;
  m_gcount = 1;
  return c;
}

//______________________________________________________________________________
TcpBuffer&
TcpBuffer::getline(char* buf, 
		   std::streamsize maxlen)
{
  int delim = 0x0a;

  m_gcount = 0;
  if((buf == 0) || (maxlen == 0))
  {
    m_iostate |= failbit;
    return *this;
  }
  int nget = 0;
  int n = 0;
  buf[n] = 0;
  int nmax = static_cast<int>(maxlen) - 1;
  if(nmax <= 0)
  {
    m_iostate |= failbit;
    return *this;
  }
  int c;
  while((c = get()) != std::char_traits<char>::eof())
  {
    ++nget;
    if(c == delim)
    {
      buf[n] = 0;
      m_gcount = nget;
      return *this;
    }
    buf[n++] = (c & 255);
    if(n >= nmax)
    {
      buf[nmax] = 0;
      m_iostate |= failbit;
      m_gcount = nget;
      return *this;
    }
  }
  m_iostate |= eofbit;
  m_gcount = nget;
  if(m_gcount == 0)
    m_iostate |= failbit;
  buf[n] = 0;
  return *this;
}

//______________________________________________________________________________
int
TcpBuffer::getpeername(struct sockaddr* name, 
		       socklen_t* namelen) const
{
  return m_socket.getpeername(name, namelen);
}

//______________________________________________________________________________
int
TcpBuffer::getsockname(struct sockaddr* name,
		       socklen_t* namelen) const
{
  return m_socket.getsockname(name, namelen);
}

//______________________________________________________________________________
int
TcpBuffer::getsockopt(int level, 
		      int optname,
		      void* optval,
		      socklen_t* optlen) const
{
  return m_socket.getsockopt(level, optname, optval, optlen);
}

//______________________________________________________________________________
TcpBuffer&
TcpBuffer::ignore(std::streamsize len)
{
  m_gcount = 0;
  if(len == 0)
    return *this;

  std::streamsize n = 0;
  for(n = 0; n < len; n++)
  {
    int c;
    if((c = get()) == std::char_traits<char>::eof())
      break;
  }
  m_gcount = n;
  return *this;
}

//______________________________________________________________________________
TcpBuffer&
TcpBuffer::put(int c)
{
  if(m_sbufnxt >= m_sbufmax)
  {
    send_all(m_sbuf, m_sbufnxt);
    m_sbuflen = 0;
    m_sbufnxt = 0;
  }
  if(bad())
    return *this;
  m_sbuf[m_sbufnxt++] = (c & 255);
  return *this;
}

//______________________________________________________________________________
TcpBuffer&
TcpBuffer::read(unsigned char* buf,
		std::streamsize len)
{
  m_gcount = 0;
  if((buf == 0) || (len == 0))
    return *this;
  std::streamsize n = 0;

  if(m_rbuflen == 0)
  {
    int nr = recv_all(buf,
		      static_cast<int>(len));
    m_gcount = nr;
    return *this;
  }

  for(n = 0; n < len; n++)
  {
    int c;
    if((c = get()) == std::char_traits<char>::eof())
      break;
    *buf++ = (c & 255);
  }
  m_gcount = n;
  return *this;
}

//______________________________________________________________________________
int
TcpBuffer::recv_all(unsigned char* buf,
		    int nbytes)
{
  int nleft;
  int nrecv;

  nleft = nbytes;
  while( nleft > 0 )
  {
    nrecv = m_socket.recv( buf, nleft );
    if( nrecv < 0 )
    {
      m_iostate |= badbit;
      return nrecv;
    }
    else if( nrecv == 0 )
    {
      m_iostate |= eofbit;
      break;
    }
    nleft -= nrecv;
    buf += nrecv;
  }
  return (nbytes - nleft);
}

//______________________________________________________________________________
int
TcpBuffer::send_all(const unsigned char* buf,
		    int nbytes)
{
  int nleft;
  int nsend;

  nleft = nbytes;
  while( nleft > 0 )
  {
    nsend = m_socket.send(buf, nleft);
    if( nsend <= 0 )
    {
      m_iostate |= badbit;
      break;
    }
    nleft -= nsend;
    buf += nsend;
  }
  return (nbytes - nleft);
}

//______________________________________________________________________________
int
TcpBuffer::setsockopt(int level, 
		      int optname,
		      const void* optval,
		      socklen_t optlen)
{
  return m_socket.setsockopt(level, optname, optval, optlen);
}

//______________________________________________________________________________
int
TcpBuffer::shutdown(int how)
{
  return m_socket.shutdown(how);
}

//______________________________________________________________________________
TcpBuffer&
TcpBuffer::write(const void* buf,
		 std::streamsize len)
{
  if((buf == 0) || (len == 0))
    return *this;
//  size_t n = 0;
  unsigned char* p 
    = reinterpret_cast<unsigned char*>(const_cast<void*>(buf));
//  for(n = 0; n < len; n++)
//  {
//    int c = *p++;
//    if(put((c & 255)) == std::char_traits<char>::eof())
//      break;
//  } 
//  return n;
  flush();
  send_all(p, static_cast<int>(len));
  return *this;
}

}
