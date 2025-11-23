// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__MUTEX_H
#define HDDAQ__MUTEX_H

#include <pthread.h>

#include "ScopedLock.hh"

namespace hddaq
{

  class Mutex 
  {
    
    friend class Condition;

    //  public:
    //    typedef ScopedLock<Mutex> ScopedLock;
    
  private:
    ::pthread_mutex_t m_mutex;
    
  public:
    explicit  Mutex(const ::pthread_mutexattr_t* attr=NULL);
             ~Mutex();

    int lock();
    int trylock();
    int unlock();
      
  };

}
#endif
