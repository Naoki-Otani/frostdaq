// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__BARRIER_H
#define HDDAQ__BARRIER_H

#include <string>

#include <pthread.h>

namespace hddaq
{
  class Barrier
  {

  public:
    typedef ::pthread_barrier_t     barrier_t;
    typedef ::pthread_barrierattr_t attr_t;

  private:
    barrier_t m_barrier;
    

  public:
    Barrier();
    Barrier(int n,
	    attr_t* attr=0);
    ~Barrier();

    void hoge(const std::string& arg="") const;
    int  destroy();
    int  initialize(int n,
		    attr_t* attr=0);
    int  wait();

  };
  
}

#endif
