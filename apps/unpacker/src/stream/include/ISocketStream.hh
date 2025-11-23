// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__I_SOCKET_STREAM_H
#define HDDAQ__I_SOCKET_STREAM_H

#include "SocketBuf.hh"
#include "input_stream.hh"

namespace hddaq
{
  namespace unpacker
  {
    typedef basic_input_stream<basic_socket_buf, char> ISocketStream;
  }
}
#endif
