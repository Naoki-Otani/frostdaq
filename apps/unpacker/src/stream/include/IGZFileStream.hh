// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__I_GZ_FILE_STREAM_H
#define HDDAQ__I_GZ_FILE_STREAM_H

#include "GZFileBuf.hh"
#include "input_stream.hh"

namespace hddaq
{
  namespace unpacker
  {
    typedef basic_input_stream<basic_gz_filebuf, char> IGZFileStream;
  }
}
#endif
