// -*- C++ -*-

// Author: Tomonori Takahashi

#include "Semaphore.hh"

namespace hddaq
{

//______________________________________________________________________________
Semaphore::Semaphore(unsigned int value)
  : m_sem(),
    m_pshared(0)
{
  ::sem_init(&m_sem, m_pshared, value);
}

//______________________________________________________________________________
Semaphore::~Semaphore()
{
  ::sem_destroy(&m_sem);
}

//______________________________________________________________________________
void
Semaphore::initialize(unsigned int value)
{
  ::sem_init(&m_sem, m_pshared, value);
  return;
}

//______________________________________________________________________________
int 
Semaphore::post()
{
  return ::sem_post(&m_sem);
}

//______________________________________________________________________________
void
Semaphore::set_process_shared(int pshared)
{
  m_pshared = pshared;
  return;
}

//______________________________________________________________________________
int 
Semaphore::trywait()
{
  return ::sem_trywait(&m_sem);
}

//______________________________________________________________________________
int 
Semaphore::wait()
{
  return ::sem_wait(&m_sem);
}

}
