// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__CONDITION_H
#define HDDAQ__CONDITION_H

//////////////////////////////////////
//                                  //
// POSIX condition variable wrapper //
//                                  //
//////////////////////////////////////

#include <pthread.h>
#include <sys/types.h>

namespace hddaq
{

  class Mutex;
    
  class Condition 
  {
    
  private:
    ::pthread_cond_t m_condition;
    Mutex&           m_mutex; 
    
  public:
     Condition(Mutex& mutex, 
	       const ::pthread_condattr_t* attr=NULL);
    ~Condition();
    
    int broadcast();
    int signal();
    int timed_wait(__time_t sec, 
		   long int nsec);
      int wait();
    
  };
  
}
#endif
