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

#include "KOLTcpSocket.hh"

namespace kol
{

//______________________________________________________________________________
TcpSocket::TcpSocket()
{
}

//______________________________________________________________________________
TcpSocket::TcpSocket(const Socket& s) 
  : TcpBuffer(s)
{
}

//______________________________________________________________________________
TcpSocket::~TcpSocket()
{
}

}
