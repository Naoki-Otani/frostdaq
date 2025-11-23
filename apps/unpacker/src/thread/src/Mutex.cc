// -*- C++ -*-

// Author: Tomonori Takahashi

#include "Mutex.hh"

#include <iostream>

namespace hddaq
{

//______________________________________________________________________________
Mutex::Mutex(const ::pthread_mutexattr_t* attr)
  : m_mutex()
{
  ::pthread_mutex_init(&m_mutex, attr);
}

//______________________________________________________________________________
Mutex::~Mutex()
{
  ::pthread_mutex_destroy(&m_mutex);
//   std::cout << "#D Mutex::~Mutex()" << std::endl;
}

//______________________________________________________________________________
int 
Mutex::lock()
{
  return ::pthread_mutex_lock(&m_mutex);
}

//______________________________________________________________________________
int 
Mutex::trylock()
{
  return ::pthread_mutex_trylock(&m_mutex);
}

//______________________________________________________________________________
int 
Mutex::unlock()
{
  return ::pthread_mutex_unlock(&m_mutex);
}

}
