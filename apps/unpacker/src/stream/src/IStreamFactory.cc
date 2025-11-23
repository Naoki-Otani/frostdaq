// -*- C++ -*-

// Author: Tomonori Takahashi

#include "IStreamFactory.hh"

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
IStreamFactory::IStreamFactory()
{
}

//______________________________________________________________________________
IStreamFactory::~IStreamFactory()
{
//   std::cout << "#D IStreamFactory::~IStreamFactory()" << std::endl;
}

//______________________________________________________________________________
void
IStreamFactory::hoge() const
{
  std::cout << "#D IStreamFactory::hoge()" << std::endl;
  return;
}

//______________________________________________________________________________
bool
IStreamFactory::add_entry(const std::string& id,
			  create_function_t function)
{
//   StaticMutex::ScopedLock lock(mutex);
  return (m_product_table.insert(std::make_pair(id, function))).second;
}

//______________________________________________________________________________
std::istream*
IStreamFactory::create(const std::string& id,
		       const std::string& stream_name,
		       std::ios_base::openmode mode)
{
//   StaticMutex::ScopedLock lock(mutex);
  std::istream* ret = 0;
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
IStreamFactory::remove_entry(const std::string& id)
{
  return (1 == m_product_table.erase(id));
}

  }
}
