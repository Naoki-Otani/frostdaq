// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__I_STREAM_FACTORY_H
#define HDDAQ__I_STREAM_FACTORY_H

#include <bits/ios_base.h>
#include <iosfwd>
#include <map>
#include <string>

#include "Singleton.hh"
#include "Uncopyable.hh"


namespace hddaq
{
  namespace unpacker
  {

  class IStreamFactory
    : private Uncopyable<IStreamFactory>
  {

  public:
    typedef std::istream* (*create_function_t)(const std::string&,
					       std::ios_base::openmode);

  private:
    friend class Singleton<IStreamFactory>;
    std::map<std::string, create_function_t> m_product_table;

  public:
    ~IStreamFactory();

    void          hoge() const;
    bool          add_entry(const std::string& id,
			    create_function_t function);
    std::istream* create(const std::string& id,
			 const std::string& stream_name,
			 std::ios_base::openmode mode);
    bool          remove_entry(const std::string& id);

  private:
    IStreamFactory();

  };

//______________________________________________________________________________
  typedef Singleton<IStreamFactory> GIStreamFactory;

  }
}
#endif
