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

#ifndef KOL__TCP_BUFFER_H
#define KOL__TCP_BUFFER_H

#include <string>

#include "KOLSocket.hh"

namespace kol {

  class TcpBuffer
  {

  private:
    enum { bufsize = 1024 };

  protected:
    Socket          m_socket;
    std::streamsize m_gcount;
    int             m_iostate;
    size_t          m_rbufmax;
    size_t          m_rbuflen;
    size_t          m_rbufnxt;
    unsigned char   m_rbuf[bufsize];
    size_t          m_sbufmax;
    size_t          m_sbuflen;
    size_t          m_sbufnxt;
    unsigned char   m_sbuf[bufsize];

  private:
    enum { goodbit = 0, eofbit = 1, failbit = 2, badbit = 4 };

  public:
             TcpBuffer();
             TcpBuffer(const Socket& s);
             TcpBuffer(int domain, 
		       int type,
		       int protocol=0);
    virtual ~TcpBuffer();

    bool            bad() const 
    { return ((m_iostate & badbit) != 0); }
    virtual int     close();
    bool            eof() const
    { return ((m_iostate & eofbit) != 0); }
    bool            fail() const
    { return ((m_iostate & (failbit | badbit)) != 0); }
    TcpBuffer&      flush();
    std::streamsize gcount() const 
    { return m_gcount; }
    virtual int     get();
    TcpBuffer&      getline(char* buf, 
			    std::streamsize maxlen);
    int             getpeername(struct sockaddr* name,
				socklen_t* namelen) const;
    int             getsockname(struct sockaddr* name,
				socklen_t* namelen) const;
    int             getsockopt(int level,
			       int optname,
			       void* optval, 
			       socklen_t* optlen) const;
    bool            good() const 
    { return (m_iostate == goodbit); }
    TcpBuffer&      ignore(std::streamsize len=1);
    TcpBuffer&      put(int c);
    TcpBuffer&      read(unsigned char* buf, 
			 std::streamsize len);
    virtual int     shutdown(int how=SHUT_RDWR);
    int             setsockopt(int level,
			       int optname,
			       const void* optval,
			       socklen_t optlen);
    TcpBuffer&      write(const void* buf,
			  std::streamsize len);

    operator void*() const
    { if(fail()) return 0;return (void*)this; }
    bool            operator!() const
    { return fail(); }

  protected:
    void initparams();
    int  recv_all(unsigned char* buf,
		  int nbytes);
    int  send_all(const unsigned char* buf,
		  int nbytes);
    int  sync();

  };

}
#endif
