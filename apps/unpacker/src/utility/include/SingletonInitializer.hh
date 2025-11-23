// -*- C++ -*-

// Author: Tomonori Takahashi 

#ifndef HDDAQ__SINGLETON_INITIALIZER_H
#define HDDAQ__SINGLETON_INITIALIZER_H

#include "Uncopyable.hh"
#include "Singleton.hh"

namespace hddaq
{

  template <class T>
  class SingletonInitializer
    : private Uncopyable<SingletonInitializer<T> >
  {
    
  public:
    SingletonInitializer();
    
  };

//______________________________________________________________________________
template <class T>
SingletonInitializer<T>::SingletonInitializer()
{
  Singleton<T>::get_instance();
}


}
#endif
