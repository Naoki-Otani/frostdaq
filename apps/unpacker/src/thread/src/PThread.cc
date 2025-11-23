// -*- C++ -*-

// Author: Tomonori Takahashi

#include "PThread.hh"

#include <iostream>

namespace hddaq
{

//______________________________________________________________________________
Thread::Thread()
  : m_name(),
    m_state(),
    m_thread_id(0)
{
  m_state.initialize_table();
}

//______________________________________________________________________________
Thread::Thread(const std::string& name)
  : m_name(name),
    m_state(),
    m_thread_id(0)
{
  m_state.initialize_table();
}

//______________________________________________________________________________
Thread::~Thread()
{
//   std::cout << "#D PThread " << m_name
// 	    << " ::~Thread()" << std::endl;
}

//______________________________________________________________________________
void 
Thread::hoge() const
{
  std::cout << "#D Thread::hoge()" << std::endl;
  return;
}

//______________________________________________________________________________
int 
Thread::cancel()
{
  if (!m_thread_id)
    return -1;
  set_command(ThreadState::k_exit);
//   std::cout << "#D thread " << m_name << " canceled." << std::endl;
  return ::pthread_cancel(m_thread_id);
}

//______________________________________________________________________________
int
Thread::detach()
{
  std::cout << "#D thread " << m_name << " detached." << std::endl;
  return ::pthread_detach(m_thread_id);
}

//______________________________________________________________________________
int
Thread::exit(void* status)
{
  std::cout << "#D thread " << m_name << " exited." << std::endl;
  ::pthread_exit(status);
  return 0;
}

//______________________________________________________________________________
const unsigned long int
Thread::get_id() const
{
  return static_cast<unsigned long int>(m_thread_id);
}

//______________________________________________________________________________
ThreadState::e_state
Thread::get_state() const
{
  return m_state.get_state();
}

//______________________________________________________________________________
bool
Thread::is_detached() const
{
  return m_state.is_detached();
}

//______________________________________________________________________________
bool
Thread::is_idle() const
{
  return (ThreadState::k_idle==m_state.get_state());
}

//______________________________________________________________________________
bool
Thread::is_running() const
{
  return (ThreadState::k_running==m_state.get_state());
}

//______________________________________________________________________________
bool
Thread::is_zombie() const
{
  return (ThreadState::k_zombie==m_state.get_state());
}

//______________________________________________________________________________
int
Thread::join(void** return_status)
{
  set_command(ThreadState::k_exit);
//   std::cout << "#D thread " << m_name << " joined." << std::endl;
  return  ::pthread_join(m_thread_id, return_status);
}

//______________________________________________________________________________
int
Thread::run()
{
  return 0;
}

//______________________________________________________________________________
void
Thread::set_command(ThreadState::e_command command)
{
//   std::cout << "#D Thread::set_command" << std::endl;
  m_state.trans(command);
  return;
}

//______________________________________________________________________________
void
Thread::set_detach()
{
  m_state.set_detach();
  return;
}

//______________________________________________________________________________
const std::string&
Thread::show_command() const
{
  return m_state.show_command();
}

//______________________________________________________________________________
const std::string&
Thread::show_state() const
{
  return m_state.show_state();
}

//______________________________________________________________________________
int
Thread::start(::pthread_attr_t* attr)
{
  set_command(ThreadState::k_start);
  if (is_detached()) 
    {
      attr = new pthread_attr_t;
      pthread_attr_init(attr);
      pthread_attr_setdetachstate(attr, PTHREAD_CREATE_DETACHED);
    }
//   std::cout << "#D thread " << m_name << " started." << std::endl;
  return ::pthread_create(&m_thread_id, 
			  attr,
			  Thread::start_routine,
			  static_cast<void*>(this));
}

//______________________________________________________________________________
void*
Thread::start_routine(void* arg)
{
  Thread* t    = static_cast<Thread*>(arg);
  t->run();
  if (t->is_detached())
    {
      delete t;
      t = 0;
    }
  return arg;
}

//______________________________________________________________________________
bool
Thread::operator==(const ::pthread_t& thread_id)
{
  int result = ::pthread_equal(m_thread_id, thread_id);
  return (0!=result);
}

//______________________________________________________________________________
bool
Thread::operator==(const Thread& other)
{
  return this->operator==(other.get_id());
}

}
