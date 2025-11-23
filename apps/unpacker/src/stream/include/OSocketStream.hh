// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__O_SOCKET_STREAM_H
#define HDDAQ__O_SOCKET_STREAM_H

#include "SocketBuf.hh"
#include "output_stream.hh"

namespace hddaq
{
  namespace unpacker
  {
    typedef basic_output_stream<basic_socket_buf, char> OSocketStream;
  }
}
#endif
