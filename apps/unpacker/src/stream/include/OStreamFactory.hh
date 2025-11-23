// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__O_STREAM_FACTORY_H
#define HDDAQ__O_STREAM_FACTORY_H

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

  class OStreamFactory
    : private Uncopyable<OStreamFactory>
  {

  public:
    typedef std::ostream* (*create_function_t)(const std::string&,
					       std::ios_base::openmode);

  private:
    friend class Singleton<OStreamFactory>;
    std::map<std::string, create_function_t> m_product_table;

  public:
    ~OStreamFactory();

    void          hoge() const;
    bool          add_entry(const std::string& id,
			    create_function_t function);
    std::ostream* create(const std::string& id,
			 const std::string& stream_name,
			 std::ios_base::openmode mode);
    bool          remove_entry(const std::string& id);

  private:
    OStreamFactory();

  };

//______________________________________________________________________________
  typedef Singleton<OStreamFactory> GOStreamFactory;

  }
}
#endif
