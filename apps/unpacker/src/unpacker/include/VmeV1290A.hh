// -*- C++ -*-

#ifndef HDDAQ__VMEV1290A_UNPCKER_H
#define HDDAQ__VMEV1290A_UNPCKER_H

#include "Uncopyable.hh"
#include "VmeModule.hh"

namespace hddaq
{
namespace unpacker
{

  class VmeV1290A
    : public VmeModule,
      private Uncopyable<VmeV1290A>
  {

  public:
    static const unpacker_type k_type;
    static const unsigned int  k_n_ch     = 32;
    static const unsigned int  k_n_module = 3;

    struct data_t
    {
      uint32_t m_adc;
    };

    enum e_data
      {
	k_data_module1        = 0x00000001U,
	k_data_module2        = 0x00414443U
      };
    
    enum e_data_type
      {
	k_adc,
	k_n_data_type
      };


  private:
    int module_id;

  public:
    VmeV1290A(const unpacker_type& type);
    virtual ~VmeV1290A();

  protected:
    virtual void decode();
    virtual void resize_fe_data();
  };
}
}
#endif
