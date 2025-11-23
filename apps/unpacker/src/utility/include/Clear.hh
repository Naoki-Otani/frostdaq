// -*- C++ -*-

// Author: Tomonori Takahashi 

#ifndef HDDAQ__CLEAR_H
#define HDDAQ__CLEAR_H

#include <utility>

namespace hddaq
{
    
    template <typename STLContainer>
    void
//     stl_clear(STLContainer& arg)
//     clear_and_minimize(STLContainer& arg)
    clear(STLContainer& arg)
    {
      arg.clear();
      STLContainer().swap(arg);
      return;
    };


  template <typename STLContainer>
  void
  clear(typename STLContainer::iterator& arg)
  {
    STLContainer().swap(*arg);
    return;
  };



    class Clear
    {
      
    public:
      template <typename T>
      void operator() (T* t);
      template <typename T>
      void operator() (T& t);
      template <typename K, typename V>
      void operator() (std::pair<K, V*>& p);
      template <typename K, typename V>
      void operator() (std::pair<K, V&>& p);
      
    };

//______________________________________________________________________________
template <typename T>
inline
void
Clear::operator() (T* t)
{
//   std::cout << "#D Clear::(T*)" << std::endl;
  if (!t) return;
  t->clear();
  return;
}
  
//______________________________________________________________________________
template <typename T>
inline
void
Clear::operator() (T& t)
{
//   std::cout << "#D Clear::(T&)" << std::endl;
  t.clear();
  return;
}
  
//______________________________________________________________________________
template <typename K, typename V>
inline
void
Clear::operator() (std::pair<K, V*>& p)
{
  if (!p.second) return;
  (p.second)->clear();
  return;
}

//______________________________________________________________________________
template <typename K, typename V>
inline
void
Clear::operator() (std::pair<K, V&>& p)
{
  (p.second).clear();
  return;
}


}
#endif
