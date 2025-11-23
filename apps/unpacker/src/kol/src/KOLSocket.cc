// -*- C++ -*-

//  @file   kolsocket.cc
//  @brief  
//  @author Hirofumi Fujii <hirofumi.fujii@kek.jp>
//  @date   Mon Nov 27 05:54:48 2006 UTC
// 
//  $Id: kolsocket.h,v 1.1.1.1 2007/03/28 07:50:17 cvs Exp $
//  $Log: kolsocket.h,v $
//  Revision 1.1.1.1  2007/03/28 07:50:17  cvs
//  prototype-1.2
// 
//  Revision 1.1.1.1  2007/01/31 13:37:53  kensh
//  Initial version.

#include "KOLSocket.hh"

#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>

#include "KOLSocketException.hh"

namespace kol
{

//______________________________________________________________________________
Socket::Socket()
  : m_sd()
{
  m_sd = invalid;
}

//______________________________________________________________________________
Socket::Socket(int domain,
	       int type,
	       int protocol)
  : m_sd()
{
  if((m_sd = ::socket(domain, type, protocol)) == invalid)
    throw SocketException("Socket::Socket error");
}

//______________________________________________________________________________
Socket::Socket(const Socket& from)
  : m_sd()
{
  m_sd = invalid;
  if(from.m_sd != invalid)
  {
    if((m_sd = DuplicateSocket(from.m_sd)) == invalid)
      throw SocketException("Socket::DuplicateSocket error");
  }
}

//______________________________________________________________________________
Socket::Socket(socket_t sd)
  : m_sd(sd)
{
}

//______________________________________________________________________________
Socket::~Socket()
{
  close();
}

//______________________________________________________________________________
Socket& 
Socket::operator=(const Socket& from)
{
  if(&from == this)
    return *this;
  close();
  if(from.m_sd != -1)
    m_sd = DuplicateSocket(from.m_sd);
  return *this;
}

//______________________________________________________________________________
Socket
Socket::accept(struct sockaddr* addr, 
	       socklen_t* addrlen)
{
  int sd;
  if((sd = ::accept(m_sd, addr, addrlen)) == sockerr)
    throw SocketException("Socket::accept error");
  return Socket(sd);
}

//______________________________________________________________________________
int
Socket::bind(const struct sockaddr* addr, 
	     socklen_t len)
{
  if( ::bind(m_sd, addr, len) == sockerr)
    throw SocketException("Socket::bind error");
  return 0;
}

//______________________________________________________________________________
int
Socket::close()
{
  int retval = sockerr;
  if(m_sd != invalid)
  {
    retval = CloseSocket(m_sd);
    m_sd = invalid;
  }
  return retval;
}

//______________________________________________________________________________
int
Socket::connect(const struct sockaddr* addr, 
		socklen_t len)
{
  if( ::connect(m_sd, addr, len) == sockerr)
    throw SocketException("Socket::connect error");
  return 0;
}
 
//______________________________________________________________________________
int
Socket::create(int domain, 
	       int type,
	       int protocol)
{
  if(m_sd != invalid)
    CloseSocket(m_sd);
  if((m_sd = ::socket(domain, type, protocol)) == invalid)
    throw SocketException("Socket::create error");
  return 0;
}

//______________________________________________________________________________
int
Socket::getpeername(struct sockaddr* name, 
		    socklen_t* namelen) const
{
  return ::getpeername(m_sd, name, namelen);
}

//______________________________________________________________________________
int
Socket::getsockname(struct sockaddr* name,
		    socklen_t* namelen) const
{
  return ::getsockname(m_sd, name, namelen);
}

//______________________________________________________________________________
int
Socket::getsockopt(int level,
		   int optname,
		   void* optval, 
		   socklen_t* optlen) const
{
  return ::getsockopt(m_sd,
		      level,
		      optname,
		      reinterpret_cast<sockmsg_t*>(optval),
		      optlen);
}

//______________________________________________________________________________
int
Socket::ioctl(int request,
	      void* argp)
{
  return IoctlSocket(m_sd, request, argp);
}

//______________________________________________________________________________
int
Socket::listen(int backlog)
{
  if( ::listen(m_sd, backlog) == sockerr)
    throw SocketException("Socket::listen error");
  return 0;
}

//______________________________________________________________________________
int
Socket::recv(void* buf, 
	     size_t len,
	     int flags)
{
  int n;
  if((n = ::recv(m_sd, 
		 reinterpret_cast<sockmsg_t*>(buf),
		 len,
		 flags))
     == sockerr)
    throw SocketException("Socket::recv error");
  return n;
}

//______________________________________________________________________________
int
Socket::recvfrom(void* buf,
		 size_t len,
		 int flags,
		 struct sockaddr* from,
		 socklen_t* fromlen)
{
  int n;
  if((n = ::recvfrom(m_sd, 
		     reinterpret_cast<sockmsg_t*>(buf), 
		     len, 
		     flags, 
		     from,
		     fromlen))
     == sockerr)
    throw SocketException("Socket::recvfrom error");
  return n;
}

//______________________________________________________________________________
int
Socket::send(const void* buf, 
	     size_t len,
	     int flags)
{
  int n;
  if((n = ::send(m_sd, 
		 reinterpret_cast<const sockmsg_t*>(buf),
		 len, 
		 flags))
     == sockerr)
    throw SocketException("Socket::send error");
  return n;
}

//______________________________________________________________________________
int
Socket::sendto(const void* buf,
	       size_t len, int flags,
	       const struct sockaddr* to,
	       socklen_t tolen)
{
  int n;
  if((n = ::sendto(m_sd, 
		   reinterpret_cast<const sockmsg_t*>(buf),
		   len, flags, 
		   to, 
		   tolen))
     == sockerr)
    throw SocketException("Socket::sendto error");
  return n;
}

//______________________________________________________________________________
int
Socket::setsockopt(int level,
		   int optname, 
		   const void* optval,
		   socklen_t optlen)
{
  return ::setsockopt(m_sd,
		      level, 
		      optname, 
		      reinterpret_cast<const sockmsg_t*>(optval),
		      optlen);
}

//______________________________________________________________________________
int
Socket::shutdown(int how)
{
  int n;
  if((n = ::shutdown(m_sd, how)) == sockerr)
    throw SocketException("Socket::shutdown error");
  return n;
}

//______________________________________________________________________________
int 
Socket::CloseSocket(socket_t s) throw()
{
  if(s == invalid)
    return sockerr;
  return ::close(s);
}

//______________________________________________________________________________
socket_t
Socket::DuplicateSocket(socket_t s) throw()
{
  if(s == invalid)
    return s;
  socket_t t;
  t = ::dup(s);
  return t;
}

//______________________________________________________________________________
int 
Socket::IoctlSocket(socket_t s, 
		    int request, 
		    void* argp) throw()
{
  if(s == invalid)
    return sockerr;
  return ::ioctl(s, request, argp);
}

}
