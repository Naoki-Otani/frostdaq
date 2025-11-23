// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__UNPACKER_FACTORY_H
#define HDDAQ__UNPACKER_FACTORY_H

#include "Factory.hh"
#include "Singleton.hh"
#include "UnpackerImpl.hh"

namespace hddaq
{
  namespace unpacker
  {

  class UnpackerFactory 
    : public  
    Factory
    <
     UnpackerImpl, 
     UnpackerImpl::unpacker_type,
     UnpackerImpl* (*)(const UnpackerImpl::unpacker_type&)
    >,
      private Uncopyable<UnpackerFactory>
  {

  private:    
    friend class Singleton<UnpackerFactory>;

  public:
    virtual ~UnpackerFactory();
    
    virtual void              hoge(const std::string& arg="") const;
    virtual const std::string get_name() const;

  private:
    UnpackerFactory();
        
  };

//______________________________________________________________________________
  typedef Singleton<UnpackerFactory> GUnpackerFactory;
  UnpackerImpl*
  create_unpacker(const UnpackerImpl::unpacker_type& type);

  }
}
#endif
