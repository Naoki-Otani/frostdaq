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

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// BSD socket library wrapper.                                          //
//                                                                      //
//  This is a wrapper class for BSD socket library.                     //
//  The main purpose of this class is to hide the socket descriptor.    //
//  Socket descriptor is automatically closed when desctructed.         //
//  The descritor is copied by using duplicate system call (i.e., dup() //
//  for Linux, DuplicateHandle() for Windows) when necessary.           //
//                                                                      //
//  No exception is thrown from this class. A member function bad()     //
//  can be used to check the validity of the socket descriptor.         //
//                                                                      //
//  @author Hirofumi Fujii (KEK online-electronics group) (c)2006       //
//                                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef KOL__SOCKET_H
#define KOL__SOCKET_H

#include <netdb.h>

#include "KOLtypes.hh"

namespace kol {

  class Socket
  {

  public:
    static const socket_t invalid = -1;
    static const int      sockerr = -1;

  protected:
    socket_t m_sd;

  public:
             Socket();
             Socket(const Socket& from);
             Socket(int domain, 
		    int type, 
		    int protocol=0);
    virtual ~Socket();
    Socket& operator=(const Socket& from);

    Socket  accept(struct sockaddr* addr=0, 
		   socklen_t* addrlen=0);
    int     bind(const struct sockaddr* addr, 
		 socklen_t len);
    int     close();
    int     connect(const struct sockaddr* addr, 
		    socklen_t len);
    int     create(int domain, 
		   int type, 
		   int protocol=0);
    int     getpeername(struct sockaddr* name,
			socklen_t* namelen) const;
    int     getsockname(struct sockaddr* name,
			socklen_t* namelen) const;
    int     getsockopt(int level,
		       int optname,
		       void* optval,
		       socklen_t* optlen) const;
    int     ioctl(int request,
		  void* argp);
    int     listen(int backlog);
    int     recv(void* buf, 
		 size_t len, 
		 int flags=0);
    int     recvfrom(void* buf, 
		     size_t len, 
		     int flags, 
		     struct sockaddr* from, 
		     socklen_t* fromlen);
    int     send(const void* buf, 
		 size_t len, 
		 int flags=0);
    int     sendto(const void* buf, 
		   size_t len, int flags, 
		   const struct sockaddr* to, 
		   socklen_t tolen);
    int     setsockopt(int level,
		       int optname,
		       const void* optval,
		       socklen_t optlen);
    int     shutdown(int how=SHUT_RDWR);
    

  protected:
    Socket(socket_t sd);

  private:
    static int      CloseSocket(socket_t s) throw();
    static socket_t DuplicateSocket(socket_t s) throw();
    static int      IoctlSocket(socket_t s, 
				int request, 
				void* argp) throw();

  };

}
#endif

