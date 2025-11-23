// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__MEMBER_FUNCTION_THREAD_H
#define HDDAQ__MEMBER_FUNCTION_THREAD_H

#include "PThread.hh"
#include "Uncopyable.hh"

#include <deque>

namespace hddaq
{

  template
  <
    typename T,
    typename Ret_T = void,
    typename Arg_T = void
  >
  class MemFuncThread
    : public Thread,
      private Uncopyable<MemFuncThread<T, Ret_T, Arg_T> >
  {

  public:
    typedef T           value_type;
    typedef value_type* pointer;
    typedef Ret_T       return_type;
    typedef Arg_T       arg_type;
    typedef return_type (value_type::* mem_func_type)(arg_type);

  private:
    pointer       m_p;
    mem_func_type m_mem_func;
    arg_type&     m_arg;
    return_type   m_ret;

  public:
    MemFuncThread(pointer p,
		  mem_func_type mem_func,
		  arg_type& arg = arg_type());
    virtual ~MemFuncThread();

    pointer     get() const;
    return_type get_result() const;
    virtual int run();

  };


//______________________________________________________________________________
template <typename T, typename Ret_T, typename Arg_T>
inline
MemFuncThread<T, Ret_T, Arg_T>::MemFuncThread(pointer p,
					      mem_func_type mem_func,
					      arg_type& arg)
  : Thread(),
    m_p(p),
    m_mem_func(mem_func),
    m_arg(arg),
    m_ret()
{
}

//______________________________________________________________________________
template <typename T, typename Ret_T, typename Arg_T>
inline
MemFuncThread<T, Ret_T, Arg_T>::~MemFuncThread()
{
}

//______________________________________________________________________________
template <typename T, typename Ret_T, typename Arg_T>
inline
typename MemFuncThread<T, Ret_T, Arg_T>::pointer
MemFuncThread<T, Ret_T, Arg_T>::get() const
{
  return m_p;
}

//______________________________________________________________________________
template <typename T, typename Ret_T, typename Arg_T>
inline
typename MemFuncThread<T, Ret_T, Arg_T>::return_type
MemFuncThread<T, Ret_T, Arg_T>::get_result() const
{
  return m_ret;
}

//______________________________________________________________________________
template <typename T, typename Ret_T, typename Arg_T>
inline
int
MemFuncThread<T, Ret_T, Arg_T>::run()
{
  m_ret = (m_p->*m_mem_func)(m_arg);
  return 0;
}

//______________________________________________________________________________
// template specialization
//______________________________________________________________________________

template
<
  typename T, 
  typename Ret_T
>
class MemFuncThread<T, Ret_T, void>
  : public Thread,
    private Uncopyable<MemFuncThread<T, Ret_T, void> >
{

public:
  typedef T                   value_type;
  typedef value_type*         pointer;
  typedef Ret_T               return_type;
  typedef Ret_T (value_type::* mem_func_type)(void);

private:
  pointer       m_p;
  mem_func_type m_mem_func;
  return_type   m_ret;

public:
  MemFuncThread(pointer p,
		mem_func_type mem_func);
  virtual ~MemFuncThread();

  pointer     get() const;
  return_type get_result() const;
  virtual int run();

};

//______________________________________________________________________________
template <typename T, typename Ret_T>
inline
MemFuncThread<T, Ret_T, void>::MemFuncThread(pointer p,
					     mem_func_type mem_func)
  : Thread(),
    m_p(p),
    m_mem_func(mem_func),
    m_ret()
{
}

//______________________________________________________________________________
template <typename T, typename Ret_T>
inline
MemFuncThread<T, Ret_T, void>::~MemFuncThread()
{
}

//______________________________________________________________________________
template <typename T, typename Ret_T>
inline
typename MemFuncThread<T, Ret_T, void>::pointer
MemFuncThread<T, Ret_T, void>::get() const
{
  return m_p;
}

//______________________________________________________________________________
template <typename T, typename Ret_T>
inline
typename MemFuncThread<T, Ret_T, void>::return_type
MemFuncThread<T, Ret_T, void>::get_result() const
{
  return m_ret;
}

//______________________________________________________________________________
template <typename T, typename Ret_T>
inline
int
MemFuncThread<T, Ret_T, void>::run()
{
  m_ret = (m_p->*m_mem_func)();
  return 0;
}


//______________________________________________________________________________
// template specialization
//______________________________________________________________________________

template
<
  typename T, 
  typename Arg_T
>
class MemFuncThread<T, void, Arg_T>
  : public Thread,
    private Uncopyable<MemFuncThread<T, void, Arg_T> >
{

public:
  typedef T                   value_type;
  typedef value_type*         pointer;
  typedef Arg_T               arg_type;
  typedef void (value_type::* mem_func_type)(arg_type);

private:
  pointer       m_p;
  mem_func_type m_mem_func;
  arg_type&     m_arg;

public:
  MemFuncThread(pointer p,
		mem_func_type mem_func,
		arg_type& arg = arg_type());
  virtual ~MemFuncThread();

  pointer     get() const;
  virtual int run();

};

//______________________________________________________________________________
template <typename T, typename Arg_T>
inline
MemFuncThread<T, void, Arg_T>::MemFuncThread(pointer p,
					     mem_func_type mem_func,
					     arg_type& arg)
  : Thread(),
    m_p(p),
    m_mem_func(mem_func),
    m_arg(arg)
{
}

//______________________________________________________________________________
template <typename T, typename Arg_T>
inline
MemFuncThread<T, void, Arg_T>::~MemFuncThread()
{
}

//______________________________________________________________________________
template <typename T, typename Arg_T>
inline
typename MemFuncThread<T, void, Arg_T>::pointer
MemFuncThread<T, void, Arg_T>::get() const
{
  return m_p;
}

//______________________________________________________________________________
template <typename T, typename Arg_T>
inline
int
MemFuncThread<T, void, Arg_T>::run()
{
  (m_p->*m_mem_func)(m_arg);
  return 0;
}

//______________________________________________________________________________
// template specialization
//______________________________________________________________________________

template <typename T>
class MemFuncThread<T, void, void>
  : public Thread,
    private Uncopyable<MemFuncThread<T, void, void> >
{

public:
    typedef T                   value_type;
    typedef value_type*         pointer;
    typedef void (value_type::* mem_func_type)(void);

private:
  pointer       m_p;
  mem_func_type m_mem_func;

public:
  MemFuncThread(pointer p,
		mem_func_type mem_func);
  virtual ~MemFuncThread();

  pointer     get() const;
  virtual int run();

};

//______________________________________________________________________________
template <typename T>
inline
MemFuncThread<T, void, void>::MemFuncThread(pointer p,
					    mem_func_type mem_func)
  : Thread(),
    m_p(p),
    m_mem_func(mem_func)
{
}

//______________________________________________________________________________
template <typename T>
inline
MemFuncThread<T, void, void>::~MemFuncThread()
{
}

//______________________________________________________________________________
template <typename T>
inline
typename MemFuncThread<T, void, void>::pointer
MemFuncThread<T, void, void>::get() const
{
  return m_p;
}

//______________________________________________________________________________
template <typename T>
inline
int
MemFuncThread<T, void, void>::run()
{
  (m_p->*m_mem_func)();
  return 0;
}

//______________________________________________________________________________
//______________________________________________________________________________
template
< 
  typename T, 
  typename Iterator
>
void
for_each(Iterator first,
	 Iterator last,
	 typename MemFuncThread<T>::mem_func_type mem_func)
{
  typedef MemFuncThread<T> Method;
  typedef typename std::deque<Method*> ThreadList;
  typedef typename ThreadList::iterator ThreadIterator;

  ThreadList tlist;
  for (Iterator i = first; i!=last; ++i)
    {
      Method* t = new Method(*i, mem_func);
      t->start();
      tlist.push_back(t);
    }

  ThreadIterator tend = tlist.end();
  for (ThreadIterator i = tlist.begin(); i!=tend; ++i)
    {
      Method* t = *i;
      if (t)
	{
	  t->join();
	  delete t;
	  t = 0;
	}
    }
  return;
}


}
#endif
