// -*- C++ -*-

// Author: Tomonori Takahashi

////////////////////////////////////////////////////////////////////////////////
// POSIX thread wrapper class:
// 1. Override "run()". 
// 2. Call "start()". 
////////////////////////////////////////////////////////////////////////////////

#ifndef HDDAQ__THREAD_H
#define HDDAQ__THREAD_H

#include <string>

#include <pthread.h>

#include "ThreadState.hh"
#include "Uncopyable.hh"

namespace hddaq
{
  
  class Thread 
    : private Uncopyable<Thread>
  {
    
  protected:
    std::string  m_name;
    ThreadState  m_state;
    ::pthread_t  m_thread_id;
    
  public:
    Thread();
    explicit  Thread(const std::string& name);
    virtual  ~Thread();
    
    virtual void            hoge() const;
    virtual int             cancel();
    virtual int             detach();
    virtual int             exit(void* status);
    const unsigned long int get_id() const;
    ThreadState::e_state    get_state() const;
    bool                    is_detached() const;
    bool                    is_idle() const;
    bool                    is_running() const;
    bool                    is_zombie() const;
    virtual int             join(void** return_status=0);
    virtual int             run();
    void                    set_command(ThreadState::e_command command);
    void                    set_detach();
    const std::string&      show_command() const;
    const std::string&      show_state() const;
    virtual int             start(::pthread_attr_t* attr=0);
    static void*            start_routine(void* arg);
    
    bool                    operator==(const ::pthread_t& thread_id);
    bool                    operator==(const Thread&);

  };
  
}
#endif
