// -*- C++ -*-

#ifndef HDDAQ__VVME_CAMAC_RM_UNPACKER_H
#define HDDAQ__VVME_CAMAC_RM_UNPACKER_H

#include "VmeModule.hh"

namespace hddaq
{
namespace unpacker
{

  class VVmeCamacRm
    : public VmeModule,
      private Uncopyable<VVmeCamacRm>
  {

  public:
    static const unpacker_type k_type;
    static const unsigned int  k_LOCK_BIT_mask = 0x8000U;
    static const unsigned int  k_n_ch = 2;
    enum e_data
      {
	k_event_tag,
	k_spill_tag,
	k_2ndAcc,
	k_FastClear,
	k_n_data
      };
    struct data_t
    {
      uint32_t m_event;
      uint32_t m_spill;
      uint32_t m_input_reg;
    };

  private:
    data_t* m_data;
    

  public:
    explicit VVmeCamacRm(const unpacker_type& type);
    virtual ~VVmeCamacRm();

  protected:
    virtual void check_data_format();
    virtual void decode();
    virtual void resize_fe_data();
    virtual void update_tag();
    
  
  };
}

}

#endif

