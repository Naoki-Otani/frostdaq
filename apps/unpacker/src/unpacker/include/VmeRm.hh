// -*- C++ -*-

#ifndef HDDAQ__VME_RM_UNPACKER_H
#define HDDAQ__VME_RM_UNPACKER_H

#include "VmeModule.hh"

namespace hddaq
{
namespace unpacker
{

  class VmeRm
    : public VmeModule,
      private Uncopyable<VmeRm>
  {

  public:
    static const unsigned int  k_LOCAL_TAG_ORIGIN = 0x1U;

    static const unpacker_type k_type;
    static const unsigned int  k_LOCK_BIT_mask = 0x80000000U;
    static const unsigned int  k_n_ch = 2;
    enum e_data
      {
	k_event_tag,
	k_spill_tag,
	k_serial,
	k_time,
	k_n_data
      };
    struct data_t
    {
      uint32_t m_event;
      uint32_t m_spill;
      uint32_t m_serial;
      uint32_t m_time;
    };

  private:
    data_t* m_data;
    

  public:
    explicit VmeRm(const unpacker_type& type);
    virtual ~VmeRm();

  protected:
    virtual void check_data_format();
    virtual void decode();
    virtual void resize_fe_data();
    virtual void update_tag();
    
  
  };
}

}

#endif

