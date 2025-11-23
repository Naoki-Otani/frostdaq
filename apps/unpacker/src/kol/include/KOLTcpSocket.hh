// -*- C++ -*-
//
// @file   koltcp.h
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

#ifndef KOL__TCP_SOCKET_H
#define KOL__TCP_SOCKET_H

#include "KOLSocket.hh"
#include "KOLTcpBuffer.hh"

namespace kol {

  class TcpSocket : public TcpBuffer
  {

  public:
             TcpSocket();
             TcpSocket(const Socket& s);
    virtual ~TcpSocket();

  };

}

#endif
