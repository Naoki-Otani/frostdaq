// -*- C++ -*-

// Author: Tomonori Takahashi


#ifndef HDDAQ__RING_BUFFER_H
#define HDDAQ__RING_BUFFER_H

#include <algorithm>
#include <iostream>
#include <vector>

#include "Delete.hh"
#include "Semaphore.hh"
#include "Mutex.hh"
#include "ScopedLock.hh"
#include "Uncopyable.hh"

namespace hddaq
{

//______________________________________________________________________________
  class RingBufferST
  {
    // Policy class for Single Thread
    
  public:
     RingBufferST(int empty,
		  int full)
    {};
     RingBufferST(Semaphore& empty,
		  Semaphore& full)
    {};
    ~RingBufferST()
    {};
    void clear() const           { return; };
    int  empty_post() const      { return 0; };
    int  empty_wait() const      { return 0; };
    int  full_post() const       { return 0; };
    int  full_wait() const       { return 0; };
    void initialize(int n) const { return; };
    int  lock()                  { return 0; };
    int  trylock()               { return 0; };
    int  trywait_empty()         { return 0; };
    int  trywait_full()          { return 0; };
    int  unlock()                { return 0; };

  };

//______________________________________________________________________________
  class RingBufferMT
  {
    // Policy class for Multi Thread

  public:
    Semaphore m_empty;
    Semaphore m_full;
    Mutex     m_mutex;

  public:
    RingBufferMT(int empty,
		 int full)
      : m_empty(empty),
	m_full(full)
    {};
    RingBufferMT(Semaphore& empty,
		 Semaphore& full)
    : m_empty(empty),
      m_full(full),
      m_mutex()
    {};
    ~RingBufferMT()
    {};

    void clear()           { m_empty = 0; m_full = 0; return; };
    int  empty_post()      { return m_empty.post(); };
    int  empty_wait()      { return m_empty.wait(); };
    int  full_post()       { return m_full.post();  };
    int  full_wait()       { return m_full.wait();  };
    void initialize(int n) { m_empty = n; m_full = 0; return; };
    int  lock()            { return m_mutex.lock(); };
    int  trylock()         { return m_mutex.trylock(); };
    int  trywait_empty()   { return m_empty.trywait(); };
    int  trywait_full()    { return m_full.trywait(); };
    int  unlock()          { return m_mutex.unlock(); };
    
  };


//______________________________________________________________________________
  template 
  <
    typename T, 
    class    ThreadPolicy = RingBufferMT,
    typename Allocator    = std::allocator<T>
  >
  class RingBuffer 
    : private Uncopyable<RingBuffer<T, ThreadPolicy, Allocator> >
  {
    
  public:
    typedef typename std::vector<T, Allocator> buf_type;
    typedef typename buf_type::value_type      value_type;
    typedef typename buf_type::allocator_type  allocator_type;
    typedef typename buf_type::iterator        iterator;
    typedef typename buf_type::const_iterator  const_iterator;
    typedef typename buf_type::reference       reference;
    typedef typename buf_type::const_reference const_reference;
    typedef typename buf_type::size_type       size_type;
    
  private:
    size_type             m_entries;   // number of filled entries 
    buf_type              m_buffer;
    size_type             m_read_position;
    size_type             m_write_position;
    size_type             m_size;
    bool                  m_is_open;
    mutable ThreadPolicy  m_policy;


    // local classes for scoped lock-unlock
    struct EmptySem
    {
      ThreadPolicy& m_tp;
      
      EmptySem(ThreadPolicy& tp)
	: m_tp(tp)
      { m_tp.empty_wait(); };
      ~EmptySem()
      { m_tp.empty_post(); };
    };

    struct FullSem
    {
      ThreadPolicy& m_tp;
      
      FullSem(ThreadPolicy& tp)
	: m_tp(tp)
      { m_tp.full_wait(); };
      ~FullSem()
      { m_tp.full_post(); };
    };
    
  public:
              RingBuffer();
    explicit  RingBuffer(size_type n);
             ~RingBuffer();
    
    void            hoge() const;
    reference       back();
    const_reference back() const;
    iterator        begin();
    const_iterator  begin() const;
    void            clear();
    void            close();
    iterator        end();
    const_iterator  end() const;
    size_type       free_size() const;
    reference       front();
    const_reference front() const;
    size_type       get_entries() const;
    void            initialize();
    bool            is_open() const;
    void            pop();
    void            push();
    void            read(value_type& x);
    void            resize(size_type n, value_type x = value_type());
    size_type       size() const;
    int             trywait_back() const;
    int             trywait_front() const;
    void            write(const value_type& x);

  };

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator>
inline
RingBuffer<T, ThreadPolicy, Allocator>::RingBuffer()
    : m_entries(0),
      m_buffer(),
      m_read_position(0),
      m_write_position(0),
      m_size(0),
      m_policy(0, 0)
{
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator>
inline
RingBuffer<T, ThreadPolicy, Allocator>::RingBuffer(size_type n)
    : m_entries(0),
      m_buffer(),
      m_read_position(0),
      m_write_position(0),
      m_size(0),
      m_is_open(false),
      m_policy(n, 0)
{
  resize(n);
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator>
inline
RingBuffer<T, ThreadPolicy, Allocator>::~RingBuffer()
{
  clear();
  m_policy.clear();
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
void 
RingBuffer<T, ThreadPolicy, Allocator>::hoge() const
{
  std::cout << "RingBuffer::hoge()" << std::endl;
  return;
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
typename RingBuffer<T, ThreadPolicy, Allocator>::reference
RingBuffer<T, ThreadPolicy, Allocator>::back()
{
  EmptySem sem(m_policy);
  return m_buffer[m_write_position];
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
typename RingBuffer<T, ThreadPolicy, Allocator>::const_reference
RingBuffer<T, ThreadPolicy, Allocator>::back() const
{
  EmptySem sem(m_policy);
  return m_buffer[m_write_position];;
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
typename RingBuffer<T, ThreadPolicy, Allocator>::iterator
RingBuffer<T, ThreadPolicy, Allocator>::begin()
{
  return m_buffer.begin();
}


//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
typename RingBuffer<T, ThreadPolicy, Allocator>::const_iterator
RingBuffer<T, ThreadPolicy, Allocator>::begin() const
{
  return m_buffer.begin();
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
void 
RingBuffer<T, ThreadPolicy, Allocator>::clear()
{
  ScopedLock<ThreadPolicy> locker(m_policy);
  m_entries        = 0;
  m_read_position  = 0;
  m_write_position = 0;
  m_size           = 0;
  m_buffer.clear();
  return;
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
void
RingBuffer<T, ThreadPolicy, Allocator>::close()
{
  m_is_open = false;
  return;
}


//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
typename RingBuffer<T, ThreadPolicy, Allocator>::iterator
RingBuffer<T, ThreadPolicy, Allocator>::end()
{
  return m_buffer.end();
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
typename RingBuffer<T, ThreadPolicy, Allocator>::const_iterator
RingBuffer<T, ThreadPolicy, Allocator>::end() const
{
  return m_buffer.end();
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator>
inline
typename RingBuffer<T, ThreadPolicy, Allocator>::size_type
RingBuffer<T, ThreadPolicy, Allocator>::free_size() const
{
  ScopedLock<ThreadPolicy> locker(m_policy);
  return m_size - m_entries;
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
typename RingBuffer<T, ThreadPolicy, Allocator>::reference
RingBuffer<T, ThreadPolicy, Allocator>::front()
{
  FullSem sem(m_policy);
  return m_buffer[m_read_position];
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
typename RingBuffer<T, ThreadPolicy, Allocator>::const_reference
RingBuffer<T, ThreadPolicy, Allocator>::front() const
{
  FullSem sem(m_policy);
  return m_buffer[m_read_position];
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator>
inline
typename RingBuffer<T, ThreadPolicy, Allocator>::size_type
RingBuffer<T, ThreadPolicy, Allocator>::get_entries() const
{
  return m_entries;
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
void 
RingBuffer<T, ThreadPolicy, Allocator>::initialize()
{
  ScopedLock<ThreadPolicy> locker(m_policy);
  m_entries         = 0;
  m_policy.initialize(m_size);
  m_read_position   = 0;
  m_write_position  = 0;
  m_is_open = true;
  return;
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
bool
RingBuffer<T, ThreadPolicy, Allocator>::is_open() const
{
  return m_is_open;
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
void 
RingBuffer<T, ThreadPolicy, Allocator>::pop()
{
  ScopedLock<ThreadPolicy> locker(m_policy);
  m_policy.full_wait();
  --m_entries;
  m_read_position = (m_read_position+1) % m_size;
  m_policy.empty_post();
  return;
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
void 
RingBuffer<T, ThreadPolicy, Allocator>::push()
{
  ScopedLock<ThreadPolicy> locker(m_policy);
  m_policy.empty_wait();
  ++m_entries;
  m_write_position = (m_write_position+1) % m_size;
  m_policy.full_post();
  return;
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
void 
RingBuffer<T, ThreadPolicy, Allocator>::read(value_type& x)
{
  ScopedLock<ThreadPolicy> locker(m_policy);
  m_policy.full_wait();
  x = m_buffer[m_read_position];
  --m_entries;
  m_read_position = (m_read_position+1) % m_size;
  m_policy.empty_post();
  return;
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
void 
RingBuffer<T, ThreadPolicy, Allocator>::resize(size_type size, 
					       value_type x)
{
  ScopedLock<ThreadPolicy> locker(m_policy);
  m_size = size;
  m_buffer.resize(size, x);
  return;
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
typename RingBuffer<T, ThreadPolicy, Allocator>::size_type
RingBuffer<T, ThreadPolicy, Allocator>::size() const
{
  ScopedLock<ThreadPolicy> locker(m_policy);
  return m_size;
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
int
RingBuffer<T, ThreadPolicy, Allocator>::trywait_back() const
{
  return m_policy.trywait_empty();
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
int
RingBuffer<T, ThreadPolicy, Allocator>::trywait_front() const
{
  return m_policy.trywait_full();
}

//______________________________________________________________________________
template <typename T, class ThreadPolicy, typename Allocator> 
inline
void 
RingBuffer<T, ThreadPolicy, Allocator>::write(const value_type& x)
{
  ScopedLock<ThreadPolicy> locker(m_policy);
  m_policy.empty_wait();
  m_buffer[m_write_position] = x;
  ++m_entries;
  m_write_position = (m_write_position+1) % m_size;
  m_policy.full_post();
  return;
}

}
#endif

