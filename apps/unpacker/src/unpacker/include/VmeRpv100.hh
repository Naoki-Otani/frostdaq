// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__VME_RPV_100_UNPACKER_H
#define HDDAQ__VME_RPV_100_UNPACKER_H

#include "VmeModule.hh"

namespace hddaq
{
namespace unpacker
{
  
  class VmeRpv100
    : public VmeModule,
      private Uncopyable<VmeRpv100>
  {

  public:
    static const unpacker_type k_type;

    struct register_type
    {
      uint16_t m_data[8][2];
    };
    static const unsigned int k_n_ch = 8U;
    
    enum e_data_type
      {
	k_each,
	k_pair,
	k_n_data_type
      };

  public:
    VmeRpv100(const unpacker_type& type);
    virtual ~VmeRpv100();

  protected:
    virtual void decode();
    virtual const std::map<std::string, int>&
                 get_data_id() const;
    virtual void resize_fe_data();
    
  };

}
}
#endif
