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

//////////////////////////////////////////
//                                      //
// exception class for socket function. //
//                                      //
//////////////////////////////////////////

#ifndef KOL__SOCKET_EXCEPTION_H
#define KOL__SOCKET_EXCEPTION_H

#include <exception>
#include <string>

namespace kol {

  class SocketException : public std::exception
  {

  private:
    int         m_reason;
    std::string m_msg;

  public:
    explicit  SocketException(const std::string& msg);
    virtual  ~SocketException() throw();

    int                 reason() const throw();
    virtual const char* what() const throw();
    
  };
}

#endif
