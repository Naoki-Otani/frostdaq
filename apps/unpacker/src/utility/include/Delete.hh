// -*- C++ -*-

// Author: Tomonori Takahashi 

#ifndef HDDAQ__DELETE_H
#define HDDAQ__DELETE_H

#include <utility>

namespace hddaq
{

    class Delete 
    {
      
    public:
      template <typename T>
      static void destroy(T& t);

      template <typename T>
      void operator() (T*& t) const;
      
      template <typename K, typename V>
      void operator() (std::pair<K, V*>& p) const;
      

    };

//______________________________________________________________________________
template <typename T>
inline
void
Delete::destroy(T& t)
{
  Delete ()(t);
  return;
}

//______________________________________________________________________________
template<typename T>
inline
void 
Delete::operator()(T*& t) const
{
  if (t)
    {
      delete t;          
      t = 0;
    }
  return;
}


//______________________________________________________________________________
template<typename K, typename V>
inline
void 
Delete::operator()(std::pair<K, V*>& p) const
{
//   std::cout << "#D delete (2) called " << std::endl;
  operator()(p.second);
  return;
}

  
}
#endif

