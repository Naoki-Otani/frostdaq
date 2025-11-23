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

#include "KOLSockAddrIn.hh"

#include <cstring> // for  memset

#include <netdb.h>  // for  struct addrinfo
#include <sys/types.h>

namespace kol
{

//______________________________________________________________________________
SockAddrIn::SockAddrIn(char* host, 
		       int port)
  : m_saddr()
{
    struct sockaddr_in* resaddr;
    struct addrinfo hints;
    struct addrinfo* res;
    int err;

    res = 0;
    ::memset(reinterpret_cast<char*>(&m_saddr), 0, sizeof(m_saddr));
    if((host == 0) || host[0] == 0)
    {
      m_saddr.sin_family = AF_INET;
      //m_saddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
      //m_saddr.sin_port = ::htons((u_short)port);
      m_saddr.sin_addr.s_addr = htonl(INADDR_ANY);
      m_saddr.sin_port = htons((u_short)port);
    }
    else
    {
      ::memset(reinterpret_cast<char*>(&hints), 0, sizeof(hints));

      hints.ai_family = PF_INET;
      hints.ai_socktype = 0;
      hints.ai_protocol = 0;

      if((err = getaddrinfo(host, 0, &hints, &res)) == 0)
      {
        resaddr = reinterpret_cast<sockaddr_in*>(res->ai_addr);
        m_saddr.sin_family = AF_INET;
        //m_saddr.sin_port = ::htons((u_short)port);
	m_saddr.sin_port = htons((u_short)port);
        m_saddr.sin_addr = resaddr->sin_addr;
        freeaddrinfo(res);
      }
    }
}

//______________________________________________________________________________
SockAddrIn::~SockAddrIn()
{
}

//______________________________________________________________________________
const struct sockaddr_in* 
SockAddrIn::Address()
{
  return &m_saddr;
}

}
