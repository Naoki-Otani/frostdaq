// -*- C++ -*-

// Author: Tomonori Takahashi

#include "Condition.hh"

#include <time.h>

#include "Mutex.hh"

namespace hddaq
{

//______________________________________________________________________________
Condition::Condition(Mutex& mutex, 
		     const ::pthread_condattr_t* attr)
  : m_condition(),
    m_mutex(mutex)
{
  ::pthread_cond_init(&m_condition, attr);
}

//______________________________________________________________________________
Condition::~Condition()
{
  ::pthread_cond_destroy(&m_condition);
}

//______________________________________________________________________________
int
Condition::broadcast()
{
  return ::pthread_cond_broadcast(&m_condition);
}

//______________________________________________________________________________
int
Condition::signal()
{
  return ::pthread_cond_signal(&m_condition);
}

//______________________________________________________________________________
int
Condition::timed_wait(__time_t second,
		      long int nano_second)
{
  ::timespec t = { second, nano_second };
  return ::pthread_cond_timedwait(&m_condition,
				  &(m_mutex.m_mutex),
				  &t);
}

//______________________________________________________________________________
int
Condition::wait()
{
  return ::pthread_cond_wait(&m_condition, 
			     &(m_mutex.m_mutex));
}

}
