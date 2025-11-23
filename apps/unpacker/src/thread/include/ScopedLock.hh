// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__SCOPED_LOCK_H
#define HDDAQ__SCOPED_LOCK_H

#include "Uncopyable.hh"

namespace hddaq
{

  template <typename Mutex_T>
  class ScopedLock 
    : private Uncopyable<ScopedLock<Mutex_T> >
  {
    
  private:
    Mutex_T& m_mutex;
    
  public:
    explicit  ScopedLock(Mutex_T& mutex);
             ~ScopedLock() throw();
    
  };

//______________________________________________________________________________
template <typename Mutex_T>
ScopedLock<Mutex_T>::ScopedLock(Mutex_T& mutex)
  : m_mutex(mutex)
{
  m_mutex.lock();
}

//______________________________________________________________________________
template <typename Mutex_T>
ScopedLock<Mutex_T>::~ScopedLock() throw()
{
  m_mutex.unlock();
}

}
#endif
