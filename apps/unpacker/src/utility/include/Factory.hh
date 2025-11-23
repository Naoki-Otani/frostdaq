// -*- C++ -*-

// Author: Tomonori Takahashi 

#ifndef HDDAQ__FACTORY_H
#define HDDAQ__FACTORY_H

#include <iostream>
#include <string>
#include <map>

#include <cstdlib>

#include "FactoryErrorExit.hh"
#include "FactoryErrorNullPointer.hh"
// #include "StaticMutex.hh"
// #include "Mutex.hh"
// #include "ScopedLock.hh"

namespace hddaq
{

  template 
  <
    typename Product,
    typename ID,
    typename Creator = Product* (*)(),
    template <typename, typename>
//       class FactoryErrorPolicy = FactoryErrorExit
      class FactoryErrorPolicy = FactoryErrorNullPointer
  >
  class Factory
    : public FactoryErrorPolicy<Product, ID>
  {
    
  public:
    typedef Product product_type;
    typedef ID      id_type;
    typedef Creator creator_type;

  private:
    typename std::map<ID, Creator> m_product_table;
//     Mutex                          m_mutex;
    
  public:
             Factory();
    virtual ~Factory();

    virtual void hoge() const;
    bool         add_entry(ID& id,
			   Creator creator);
    bool         add_entry(const ID& id,
			   Creator creator);
    Product*     create(ID& id);
    Product*     create(const ID& id);
    Creator&     get_creator(ID& id);
    Creator&     get_creator(const ID& id);
    virtual const std::string 
                 get_name() const = 0;
    bool         remove_entry(ID& id);
    bool         remove_entry(const ID& id);

  };

//______________________________________________________________________________
template 
<
 typename Product, 
 typename ID, 
 typename Creator, 
 template <typename, typename> class FactoryErrorPolicy
>
inline
Factory<Product, ID, Creator, FactoryErrorPolicy>::Factory()
  : FactoryErrorPolicy<Product, ID> (),
    m_product_table()//, m_mutex()
{
}

//______________________________________________________________________________
template 
<
 typename Product, 
 typename ID, 
 typename Creator, 
 template <typename, typename> class FactoryErrorPolicy
>
inline
Factory<Product, ID, Creator, FactoryErrorPolicy>::~Factory()
{
}

//______________________________________________________________________________
template 
<
 typename Product, 
 typename ID, 
 typename Creator, 
 template <typename, typename> class FactoryErrorPolicy
>
inline
void 
Factory<Product, ID, Creator, FactoryErrorPolicy>::hoge() const
{
  std::cout << "#D Facotry::hoge()" << std::endl;
  return;
}

//______________________________________________________________________________
template 
<
 typename Product, 
 typename ID, 
 typename Creator, 
 template <typename, typename> class FactoryErrorPolicy
>
inline
bool
Factory<Product, ID, Creator, FactoryErrorPolicy>::add_entry(ID& id, 
							     Creator creator)
{
//   ScopedLock<Mutex> lock(m_mutex);
  return (m_product_table.insert(std::make_pair(id, creator))).second;
}


//______________________________________________________________________________
template 
<
 typename Product, 
 typename ID, 
 typename Creator, 
 template <typename, typename> class FactoryErrorPolicy
>
inline
bool
Factory<Product, ID, Creator, FactoryErrorPolicy>::add_entry(const ID& id, 
							     Creator creator)
{
//   ScopedLock<Mutex> lock(m_mutex);
  return (m_product_table.insert(std::make_pair(id, creator))).second;
}

//______________________________________________________________________________
template 
<
 typename Product, 
 typename ID, 
 typename Creator, 
 template <typename, typename> class FactoryErrorPolicy
>
inline
Product*
Factory<Product, ID, Creator, FactoryErrorPolicy>::create(ID& id)
{
//   ScopedLock<Mutex> lock(m_mutex);
  typename std::map<ID, Creator>::iterator i = m_product_table.find(id);
  
  if (i!=m_product_table.end()) 
    {
      return (i->second)();
    }
  std::cerr << "#E " << get_name() << "::create()\n "
	    << " got unknown id : " << id << std::endl;
  return handle_unknown_type(id);
}

//______________________________________________________________________________
template 
<
 typename Product, 
 typename ID, 
 typename Creator, 
 template <typename, typename> class FactoryErrorPolicy
>
inline
Product*
Factory<Product, ID, Creator, FactoryErrorPolicy>::create(const ID& id)
{
//   ScopedLock<Mutex> lock(m_mutex);
  typename std::map<ID, Creator>::iterator i = m_product_table.find(id);
  
  if (i==m_product_table.end()) 
    {
      std::cerr << "#E " << get_name() << "::create()\n "
		<< " got unknown type : " << id << std::endl;
      return handle_unknown_type(id);
    }

  return (i->second)();
}

//______________________________________________________________________________
template 
<
 typename Product, 
 typename ID, 
 typename Creator, 
 template <typename, typename> class FactoryErrorPolicy
>
inline
Creator&
Factory<Product, ID, Creator, FactoryErrorPolicy>::get_creator(ID& id)
{
  typename std::map<ID, Creator>::iterator i = m_product_table.find(id);
  
  if (i==m_product_table.end()) 
    {
      std::cerr << "#E " << get_name() << "::get_creator()\n "
		<< " got unknown type : " << id << std::endl;
      std::exit(1);
    }

  return i->second;
}


//______________________________________________________________________________
template 
<
 typename Product, 
 typename ID, 
 typename Creator, 
 template <typename, typename> class FactoryErrorPolicy
>
inline
Creator&
Factory<Product, ID, Creator, FactoryErrorPolicy>::get_creator(const ID& id)
{
  typename std::map<ID, Creator>::iterator i = m_product_table.find(id);
  
  if (i==m_product_table.end()) 
    {
      std::cerr << "#E " << get_name() << "::get_creator()\n "
		// << " got unknown type : \n"
		// << "id = " << id << "\n"
		// << "m_product_tablr.find(id) = " << m_product_table.find(id) << "\n"
		// << "m_product_table.end() = " << m_product_table.end() << "\n"
		<< std::endl;
      std::exit(1);
    }

  return i->second;
}

//______________________________________________________________________________
template 
<
 typename Product, 
 typename ID, 
 typename Creator, 
 template <typename, typename> class FactoryErrorPolicy
>
inline
bool
Factory<Product, ID, Creator, FactoryErrorPolicy>::remove_entry(ID& id)
{
  return (1==m_product_table.erase(id));
}

//______________________________________________________________________________
template 
<
 typename Product, 
 typename ID, 
 typename Creator, 
 template <typename, typename> class FactoryErrorPolicy
>
inline
bool
Factory<Product, ID, Creator, FactoryErrorPolicy>::remove_entry(const ID& id)
{
  return (1==m_product_table.erase(id));
}

}
#endif
