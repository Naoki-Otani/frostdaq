// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__THREAD_STATE_H
#define HDDAQ__THREAD_STATE_H

#include <string>
#include <map>

#include "Mutex.hh"

namespace hddaq
{

  class ThreadState 
  {
    
  public:
    enum e_state
      {
	k_idle,
	k_running,
	k_zombie
      };

    enum e_command
      {
	k_no_command,
	k_start,
	k_stop,
	k_exit
      };

  private:
    mutable Mutex m_mutex;
    bool          m_is_detached;
    e_state       m_state;
    e_command     m_command;
    std::map<std::pair<e_state, e_command>, e_state> 
                  m_state_transition_table;

  public:
     ThreadState();
    ~ThreadState();

    void               hoge() const;
    const e_state&     get_state() const;
    void               initialize_table();
    bool               is_detached() const;
    void               set_detach();
    const std::string& show_command() const;
    const std::string& show_state() const;
    void               trans(e_command command);
    
  private:
    void insert(e_state   current_state,
		e_command command,
		e_state   target_state);

  };

}
#endif
