// -*- C++ -*-

#ifndef HDDAQ__SOY_TIME_STAMP_UNPCKER_H
#define HDDAQ__SOY_TIME_STAMP_UNPCKER_H

#include "DAQNode.hh"

namespace hddaq
{
namespace unpacker
{

  class SoyTimeStamp
    : public DAQNode,
      private Uncopyable<SoyTimeStamp>
  {

  public:
    static const unpacker_type k_type;
    static const unsigned int  k_n_ch = 1;

    struct data_t
    {
      uint32_t m_clock;
//       uint16_t m_n_neg_edge;
//       uint16_t m_n_pos_edge;
      uint32_t m_edge;
    };

    enum e_data
      {
	k_data_slip_bit       = 0x80000000U,
	k_data_slip_shift     = 28,
	k_data_slip_mask      = 0x80000000U,
	k_data_counter_mask   = 0x0fffffffU,
	k_data_pos_edge_mask  = 0xffff0000U,
	k_data_pos_edge_shift = 16U,
	k_data_neg_edge_mask  = 0x0000ffffU,
	k_data_neg_edge_shift = 0U
      };
    
    enum e_data_type
      {
	k_data_type_counter  = 0,
	k_data_type_slip     = 1,
	k_data_type_pos_edge = 2,
	k_data_type_neg_edge = 3,
	k_n_data_type        = 4
      };


  private:
    data_t* m_data;

  public:
    SoyTimeStamp(const unpacker_type& type);
    virtual ~SoyTimeStamp();

  protected:
    virtual void decode();
    virtual void resize_fe_data();
    virtual bool unpack();



  };
}

}


#endif

