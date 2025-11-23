// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__STATIC_MUTEX_H
#define HDDAQ__STATIC_MUTEX_H

////////////////////////////////////////////////////////
//                                                                 
// C compatible structure  (or POD-type) MUST NOT HAVE
//    constructor
//    destructor 
//    copy constructor
//    assignment operator
//    base class
//    virtual function
//    protected member
//    private member
//
////////////////////////////////////////////////////////

#include <pthread.h>

#include "ScopedLock.hh"

namespace hddaq
{

#define g_HDDAQ__STATIC_MUTEX_INITIALIZER {PTHREAD_MUTEX_INITIALIZER}

  class StaticMutex 
  {
    
  public:
    //    typedef ScopedLock<StaticMutex> ScopedLock;
    
    ::pthread_mutex_t m_mutex;
    
    int lock();
    int trylock();
    int unlock();
    
  };
  
}
#endif
