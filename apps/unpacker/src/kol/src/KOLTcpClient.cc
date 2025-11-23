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

#include "KOLTcpClient.hh"

#include <cstring>

#include <sys/types.h>

#include "KOLSocketException.hh"

namespace kol
{

//______________________________________________________________________________
TcpClient::TcpClient() 
  : TcpBuffer(PF_INET, SOCK_STREAM, 0)
{
}

//______________________________________________________________________________
TcpClient::TcpClient(const char* host, 
		     int port) 
  : TcpBuffer(PF_INET, SOCK_STREAM, 0)
{
  Start(host, port);
}

//______________________________________________________________________________
TcpClient::TcpClient(const Socket& s, 
		     const char* host,
		     int port) 
  : TcpBuffer(s)
{
  Start(host, port);
}

//______________________________________________________________________________
TcpClient::~TcpClient()
{
}

//______________________________________________________________________________
void
TcpClient::Start(const char* host, 
		 int port)
{
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
    if((err = getaddrinfo(host, 0, &hints, &res)) != 0)
      throw SocketException("TcpClient::getaddrinfo error");
    resaddr = reinterpret_cast<sockaddr_in*>(res->ai_addr);

    ::memset(reinterpret_cast<char*>(&srvaddr), 0, sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    //srvaddr.sin_port = ::htons((u_short)port);
    srvaddr.sin_port = htons(static_cast<u_short>(port));
    srvaddr.sin_addr = resaddr->sin_addr;
    freeaddrinfo(res);
    m_socket.connect(reinterpret_cast<sockaddr*>(&srvaddr), sizeof(srvaddr));
    }
  catch(...)
    {
      m_socket.close();
      throw;
    }
}

}
