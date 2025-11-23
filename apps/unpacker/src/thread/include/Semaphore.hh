// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__SEMAPHORE_H
#define HDDAQ__SEMAPHORE_H

////////////////////////////////////////////////
//                                            //
// POSIX semaphore wrapper class              //
// wait() decrements m_sem                    //
// post() increments m_sem                    //
//                                            //
// when m_sem==0,                             //
//   sem.wait() causes lock                   //
//   sem.trywait() doesn't lock and return -1 //
// when m_sem>=1,                             // 
//   sem.wait()    doesn't lock and return 0  // 
//   sem.trywait() doesn't lock and return 0  // 
////////////////////////////////////////////////

#include <semaphore.h>

namespace hddaq
{

  class Semaphore 
  {
    
  private:
    ::sem_t m_sem;
    int     m_pshared;
    
  public:
     Semaphore(unsigned int value); 
    ~Semaphore();
    
    void initialize(unsigned int value);
    int  post();
    void set_process_shared(int pshared);
    int  trywait();
    int  wait();

  };

}
#endif
