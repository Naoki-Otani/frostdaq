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

#include "KOLTcpServer.hh"

#include <cstring>

#include <sys/types.h>

namespace kol
{

//______________________________________________________________________________
TcpServer::TcpServer() 
  : TcpBuffer(PF_INET, SOCK_STREAM, 0) 
{
}

//______________________________________________________________________________
TcpServer::TcpServer(int port, int backlog)
  : TcpBuffer(PF_INET, SOCK_STREAM, 0) 
{
  int on = 1;
  m_socket.setsockopt(SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
  Start(port, backlog);
}

//______________________________________________________________________________
TcpServer::TcpServer(const Socket& s, int port, int backlog) 
  : TcpBuffer(s)
{
  Start(port, backlog);
}

//______________________________________________________________________________
TcpServer::~TcpServer()
{
}

//______________________________________________________________________________
TcpSocket
TcpServer::accept()
{
  return TcpSocket(m_socket.accept());
}

//______________________________________________________________________________
void
TcpServer::Start(int port, int backlog)
{
  struct sockaddr_in addr;
  ::memset(reinterpret_cast<char*>(&addr), 0, sizeof(addr));
  addr.sin_family = AF_INET;
  //addr.sin_addr.s_addr = ::htonl(INADDR_ANY);
  //addr.sin_port = ::htons((u_short)port);
  addr.sin_addr.s_addr = htonl(INADDR_ANY);
  addr.sin_port = htons(static_cast<u_short>(port));
  if(m_socket.bind(reinterpret_cast<const sockaddr*>(&addr),
		   sizeof(addr))
     == -1)
    return;
  m_socket.listen(backlog);
}

}
