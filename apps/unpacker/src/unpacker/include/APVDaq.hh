// -*- C++ -*-

#ifndef HDDAQ__APVDAQ_UNPCKER_H
#define HDDAQ__APVDAQ_UNPCKER_H

#include "Uncopyable.hh"
#include "VmeModule.hh"

namespace hddaq
{
namespace unpacker
{

  class APVDaq
    : public VmeModule,
      private Uncopyable<APVDaq>
  {

  public:
    static const unpacker_type k_type;
    static const unsigned int  k_n_ch     = 384;
    static const unsigned int  k_n_module = 4;

    struct data_t
    {
      //      uint32_t m_clock;
//       uint16_t m_n_neg_edge;
//       uint16_t m_n_pos_edge;
      uint32_t m_adc;
    };

    enum e_data
      {
	k_data_trailer        = 0x44535300U,
	k_data_tdc_flag       = 0x00535344U,
	k_data_counter_mask   = 0x0fffffffU,
	k_data_module_id      = 0x0c000000U,
	k_data_module_id_shift= 28,
	k_data_error_mask     = 0x3ff,
	k_data_adc_mask       = 0x3ff,
	k_data_adc_shift      = 10,
	k_data_ch_mask        = 0x7f,
	k_data_ch_shift       = 20,
	k_data_sp_mask        = 0x1f,
	k_data_sp_shift       = 27
      };
    
    enum e_data_type
      {
	k_adc,
	k_n_data_type
      };


  private:
    int module_id;

  public:
    APVDaq(const unpacker_type& type);
    virtual ~APVDaq();

  protected:
    virtual void decode();
    virtual void resize_fe_data();
  };
}
}
#endif
