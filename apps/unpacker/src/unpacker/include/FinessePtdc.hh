// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__FINESSE_PIPELINE_TDC_UNPACKER_H
#define HDDAQ__FINESSE_PIPELINE_TDC_UNPACKER_H

// channel assign
// front panel    FPGA
//           0      15
//           1      14
//           2      13
//           3      12
//           4      11
//           5      10
//           6       9
//           7       8
//           8       7
//           9       6
//          10       5
//          11       4
//          12       3
//          13       2
//          14       1
//          15       0
//          16      31
//          17      30
//          18      29
//          19      28
//          20      27
//          21      26
//          22      25
//          23      24
//          24      23
//          25      22
//          26      21
//          27      20
//          28      19
//          29      18
//          30      17
//          31      16

#include "Finesse.hh"

namespace hddaq
{
  namespace unpacker
  {

  class FinessePtdc
    : public Finesse,
      private Uncopyable<FinessePtdc>
  {

  public:
    static const unpacker_type k_type;
    static const unsigned int k_n_ch             = 32U;
    static const unsigned int k_n_depth          = 256U;
    static const unsigned int k_ch_mask          = 0x001f0000U;
    static const unsigned int k_ch_shift         = 16U;
    //    static const unsigned int k_data_mask        = 0xffffU;
    static const unsigned int k_data_mask        = 0x3ffU; // E13 mod
    static const unsigned int k_empty_flag_mask  = 0x200000U;
    static const unsigned int k_empty_flag_shift = 21U;

    enum e_data_type
      {
	k_leading,
	//	k_leading_empty,
	k_n_data_type
      };


  public:
    FinessePtdc(const unpacker_type& type);
    virtual ~FinessePtdc();

  protected:
    virtual uint32_t calc_check_sum() const;
    virtual void     decode();
    virtual const std::map<std::string, int>&
                     get_data_id() const;
    virtual void     resize_fe_data();

  };
    
  }
}
#endif
