// -*- C++ -*-

// Author: Tomonori Takahashi

#include "StaticMutex.hh"

namespace hddaq
{

//______________________________________________________________________________
int 
StaticMutex::lock()
{
  return ::pthread_mutex_lock(&m_mutex);
}

//______________________________________________________________________________
int 
StaticMutex::trylock()
{
  return ::pthread_mutex_trylock(&m_mutex);
}

//______________________________________________________________________________
int 
StaticMutex::unlock()
{
  return ::pthread_mutex_unlock(&m_mutex);
}

}
