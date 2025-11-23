// -*- C++ -*-

// Author: Tomonori Takahashi

#include "Barrier.hh"

#include <iostream>

#include "ScopedLock.hh"

namespace hddaq
{

//______________________________________________________________________________
Barrier::Barrier()
  : m_barrier()
{
}

//______________________________________________________________________________
Barrier::Barrier(int n,
		 attr_t* attr)
  : m_barrier()
{
  initialize(n, attr);
}

//______________________________________________________________________________
Barrier::~Barrier()
{
  destroy();
}

//______________________________________________________________________________
int
Barrier::destroy()
{
  return ::pthread_barrier_destroy(&m_barrier);
}

//______________________________________________________________________________
void
Barrier::hoge(const std::string& arg) const
{
  std::cout << "#D Barrier::hoge(" << arg << ")" << std::endl;
  return;
}

//______________________________________________________________________________
int
Barrier::initialize(int n,
		    attr_t* attr)
{
  return ::pthread_barrier_init(&m_barrier, attr, n);
}

//______________________________________________________________________________
int
Barrier::wait()
{
  return ::pthread_barrier_wait(&m_barrier);
}


}
