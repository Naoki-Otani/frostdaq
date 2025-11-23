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

#ifndef KOL__TCP_SERVER_H
#define KOL__TCP_SERVER_H

#include "KOLSocket.hh"
#include "KOLTcpSocket.hh"

namespace kol {

  class TcpServer : public TcpBuffer
  {

  public:
             TcpServer();
             TcpServer(int port, int backlog=5);
             TcpServer(const Socket& s, int port, int backlog=5);
    virtual ~TcpServer();

    virtual TcpSocket accept();
    void              Start(int port, int backlog);

  //private:
  //  void Start(int port, int backlog);
  };

}

#endif
