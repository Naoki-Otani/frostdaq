// -*- C++ -*-

// Author: Tomonori Takahashi

#include "ThreadState.hh"

#include <iostream>

#include "ScopedLock.hh"

namespace hddaq
{

//______________________________________________________________________________
ThreadState::ThreadState()
  : m_mutex(),
    m_is_detached(false),
    m_state(k_zombie),
    m_command(k_no_command),
    m_state_transition_table()
{
}

//______________________________________________________________________________
ThreadState::~ThreadState()
{
}

//______________________________________________________________________________
void 
ThreadState::hoge() const
{
  std::cout << "#D ThreadState::hoge()" << std::endl;
  return;
}

//______________________________________________________________________________
const ThreadState::e_state&
ThreadState::get_state() const
{
  ScopedLock<Mutex> lock(m_mutex);
  return m_state;
}

//______________________________________________________________________________
void 
ThreadState::initialize_table()
{
  ScopedLock<Mutex> lock(m_mutex);
  if (!m_state_transition_table.empty())
    return;

  // (current state), (command) -> (target state)
  insert(k_idle,    k_no_command, k_idle);
  insert(k_idle,    k_start,      k_running);
  insert(k_idle,    k_stop,       k_idle);
  insert(k_idle,    k_exit,       k_zombie);

  insert(k_running, k_no_command, k_running);
  insert(k_running, k_start,      k_running);
  insert(k_running, k_stop,       k_idle);
  insert(k_running, k_exit,       k_zombie);

  insert(k_zombie,  k_no_command, k_zombie);
  insert(k_zombie,  k_start,      k_running);
  insert(k_zombie,  k_stop,       k_zombie);
  insert(k_zombie,  k_exit,       k_zombie);

  return;
}

//______________________________________________________________________________
void 
ThreadState::insert(e_state   current_state,
		    e_command command,
		    e_state   target_state)
{
  m_state_transition_table
    //[std::make_pair(std::make_pair(current_state, command))] =  target_state;
    [std::make_pair(current_state, command)] =  target_state;
  return;
}

//______________________________________________________________________________
bool 
ThreadState::is_detached() const
{
  ScopedLock<Mutex> lock(m_mutex);
  return m_is_detached;
}

//______________________________________________________________________________
void 
ThreadState::set_detach()
{
  ScopedLock<Mutex> lock(m_mutex);
  m_is_detached = true;
  return;
}

//______________________________________________________________________________
const std::string&
ThreadState::show_command() const
{
  ScopedLock<Mutex> lock(m_mutex);
  static std::map<e_command, std::string> command_table;
  if (command_table.empty()) 
    {
      command_table.insert(std::make_pair(k_no_command, "NOCOMMAND"));
      command_table.insert(std::make_pair(k_start,      "START"));
      command_table.insert(std::make_pair(k_stop,       "STOP"));
      command_table.insert(std::make_pair(k_exit,       "EXIT"));
    }
  return command_table.find(m_command)->second;
}

//______________________________________________________________________________
const std::string&
ThreadState::show_state() const
{
  ScopedLock<Mutex> lock(m_mutex);
  static std::map<e_state, std::string> state_table;
  if (state_table.empty()) 
    {
      state_table.insert(std::make_pair(k_idle,    "IDLE"));
      state_table.insert(std::make_pair(k_running, "RUNNING"));
      state_table.insert(std::make_pair(k_zombie,  "ZOMBIE"));
    }
  return state_table.find(m_state)->second;
}

//______________________________________________________________________________
void 
ThreadState::trans(e_command command)
{
  ScopedLock<Mutex> lock(m_mutex);
  m_command = command;
  m_state = m_state_transition_table[std::make_pair(m_state, m_command)];
  return;
}

}
