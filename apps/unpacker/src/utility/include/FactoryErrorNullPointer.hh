// -*- C++ -*-

// Author: Tomonori Takahashi 

#ifndef HDDAQ__FACTORY_ERROR_NULL_POINTER_H
#define HDDAQ__FACTORY_ERROR_NULL_POINTER_H

namespace hddaq
{

  template 
  <
    typename Product,
    typename ID
  >
  class FactoryErrorNullPointer
  {
    
  protected:
    Product* handle_unknown_type(const ID& id);
    
  };
  
//______________________________________________________________________________
template <typename Product, typename ID>
Product*
FactoryErrorNullPointer<Product, ID>::handle_unknown_type(const ID& id)
{
  return reinterpret_cast<Product*>(0);
}


}
#endif
