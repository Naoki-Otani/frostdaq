// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__I_BZ_FILE_STREAM_H
#define HDDAQ__I_BZ_FILE_STREAM_H

#include "BZFileBuf.hh"
#include "input_stream.hh"

namespace hddaq
{
  namespace unpacker
  {
    typedef basic_input_stream<basic_bz_filebuf, char> IBZFileStream;
  }
}
#endif
