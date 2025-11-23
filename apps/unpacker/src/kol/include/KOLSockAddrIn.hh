// -*- C++ -*-

//  @file   kolsocket.h
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

///////////////////////////////////////////////////////////
//                                                       //
// SockAddrIn : class for internet socket address class. //
//                                                       //
///////////////////////////////////////////////////////////

#ifndef KOL__SOCKADDRIN_H
#define KOL__SOCKADDRIN_H

#include <netinet/in.h> // for struct sockaddr_in

namespace kol {
  class SockAddrIn
  {

  protected:
    struct sockaddr_in m_saddr;

  public:
             SockAddrIn(char* host, 
			int port);
    virtual ~SockAddrIn();

    const struct sockaddr_in* Address();

  };
}
#endif
