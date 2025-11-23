// -*- C++ -*-

// Author: Tomonori Takahashi 

#ifndef HDDAQ__SINGLETON_H
#define HDDAQ__SINGLETON_H

#include "Uncopyable.hh"

namespace hddaq
{

  template <class T>
  class Singleton
    : private Uncopyable<Singleton<T> >
  {
    
  public:
    typedef T* pointer;
    typedef const T* const_pointer;
    typedef T& reference;
    typedef const T& const_reference;
    typedef T value_type;

     Singleton();
    ~Singleton();

    static reference get_instance();

  };

//______________________________________________________________________________
template <class T>
inline
typename Singleton<T>::reference
Singleton<T>::get_instance()
{
  static T g_instance;
  return g_instance;
}

}
#endif
