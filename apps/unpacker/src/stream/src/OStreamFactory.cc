// -*- C++ -*-

// Author: Tomonori Takahashi

#include "OStreamFactory.hh"

#include <iostream>

#include <cstdlib>

// #include "StaticMutex.hh"

namespace hddaq
{
  namespace unpacker
  {
// //______________________________________________________________________________
// namespace
// {
//   StaticMutex mutex = g_HDDAQ__STATIC_MUTEX_INITIALIZER;
// }

//______________________________________________________________________________
OStreamFactory::OStreamFactory()
{
}

//______________________________________________________________________________
OStreamFactory::~OStreamFactory()
{
//   std::cout << "#D OStreamFactory destructor called" << std::endl;
}

//______________________________________________________________________________
void
OStreamFactory::hoge() const
{
  std::cout << "#D OStreamFactory::hoge()" << std::endl;
  return;
}

//______________________________________________________________________________
bool
OStreamFactory::add_entry(const std::string& id,
			  create_function_t function)
{
//   StaticMutex::ScopedLock lock(mutex);
  return (m_product_table.insert(std::make_pair(id, function))).second;
}

//______________________________________________________________________________
std::ostream*
OStreamFactory::create(const std::string& id,
		       const std::string& stream_name,
		       std::ios_base::openmode mode)
{
//   StaticMutex::ScopedLock lock(mutex);
  std::ostream* ret = 0;
  std::map<std::string, create_function_t>::iterator i
    = m_product_table.find(id);
  
  if (m_product_table.end() != i)
    {
      ret = (i->second)(stream_name, mode);
    }
  else 
    {
      std::cerr << "#E unknown stream type : " << id << std::endl;
      std::exit(0);
    }
  return ret;
}

//______________________________________________________________________________
bool
OStreamFactory::remove_entry(const std::string& id)
{
  return (1 == m_product_table.erase(id));
}

  }
}
