// -*- C++ -*-

// Author: Tomonori Takahashi 

#ifndef HDDAQ__FACTORY_ERROR_EXIT_H
#define HDDAQ__FACTORY_ERROR_EXIT_H

#include <cstdlib>

namespace hddaq
{

  template 
  <
    typename Product,
    typename ID
  >
  class FactoryErrorExit
  {
    
  protected:
    Product* handle_unknown_type(const ID& id);
    
  };
  
//______________________________________________________________________________
template <typename Product, typename ID>
inline
Product*
FactoryErrorExit<Product, ID>::handle_unknown_type(const ID& id)
{
  std::exit(0);
  return reinterpret_cast<Product*>(0);
}


}
#endif
