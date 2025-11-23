// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__UNCOPYABLE_H
#define HDDAQ__UNCOPYABLE_H

namespace hddaq
{

    template <class T>
    class Uncopyable 
    {
    
    protected:
      Uncopyable()
      {};
      ~Uncopyable()
      {};
    
    private:
      Uncopyable(const Uncopyable&);

      T& operator=(const T&);

    };
  
}
#endif
