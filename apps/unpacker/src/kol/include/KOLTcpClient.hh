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

#ifndef KOL__TCP_CLIENT_H
#define KOL__TCP_CLIENT_H

#include "KOLSocket.hh"
#include "KOLTcpBuffer.hh"

namespace kol {

  class TcpClient : public TcpBuffer
  {

  public:
             TcpClient();
             TcpClient(const char* host, int port);
             TcpClient(const Socket& s, const char* host, int port);
    virtual ~TcpClient();

    void Start(const char* host, int port);

  //private:
  //  void Start(const char* host, int port);

  };

}

#endif
