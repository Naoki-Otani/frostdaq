// -*- C++ -*-

// Author: Tomonori Takahashi 

#ifndef HDDAQ__O_BZ_FILE_STREAM_H
#define HDDAQ__O_BZ_FILE_STREAM_H

#include "BZFileBuf.hh"
#include "output_stream.hh"

namespace hddaq
{
  namespace unpacker
  {
    typedef basic_output_stream<basic_bz_filebuf, char> OBZFileStream;
  }
}
#endif
