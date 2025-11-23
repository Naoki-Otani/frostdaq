// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__STD_PAIR_DUMP_H
#define HDDAQ__STD_PAIR_DUMP_H

#include <utility>
#include "std_ostream.hh"

namespace hddaq
{

  struct PairDump
  {

  public:
    PairDump();
    ~PairDump();

    template <typename T1, typename T2>
    void operator()(const std::pair<T1, T2>& p) const;

  };

//______________________________________________________________________________
inline PairDump::PairDump()
{
}

//______________________________________________________________________________
inline PairDump::~PairDump()
{
  cout.flush();
}

//______________________________________________________________________________
template <typename T1, typename T2>
void
PairDump::operator()(const std::pair<T1, T2>& p) const
{
  cout << " "  
       << p.first << ", " 
       << p.second  << "\n";
  return;
}
  

}
#endif
