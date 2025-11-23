// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__UNPACKER_STD_OSTREAM_H
#define HDDAQ__UNPACKER_STD_OSTREAM_H

#include <iostream>

namespace hddaq
{

  // if cerr or cout is used without namespace, it means that
  // it belongs not to std but to namespace hddaq::unpacker
  extern std::ostream cerr;
  extern std::ostream cout;
  extern std::ostream tag_summary;

  void
  set_cerr(const std::string& name);

  void
  set_cout(const std::string& name);

  void
  set_tag_summary(const std::string& name);

}

#endif
