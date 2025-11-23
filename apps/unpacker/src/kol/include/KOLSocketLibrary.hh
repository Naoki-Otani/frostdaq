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

///////////////////////////
//                       //
// socket library loader //
//                       //
///////////////////////////

#ifndef KOL__SOCKET_LIBRARY_H
#define KOL__SOCKET_LIBRARY_H

#include "KOLtypes.hh"

namespace kol {
  class SocketLibrary
  {

  private:
    bool m_libloaded;
    
  public:
             SocketLibrary();
    virtual ~SocketLibrary();

    bool isloaded() const throw();
  };
}
#endif
