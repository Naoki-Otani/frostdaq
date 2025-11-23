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

#include "KOLSocketException.hh"

namespace kol
{

//______________________________________________________________________________
SocketException::SocketException(const std::string& msg) 
  : m_reason(0),
    m_msg(msg)
{
}

//______________________________________________________________________________
SocketException::~SocketException() throw()
{
}

//______________________________________________________________________________
int 
SocketException::reason() const throw()
{
  return m_reason;
}

//______________________________________________________________________________
const char*
SocketException::what() const throw()
{
  return m_msg.c_str();
}

}
