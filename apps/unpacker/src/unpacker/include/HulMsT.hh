// -*- C++ -*-

/**
 * Data structure of HUL MsT
 *
 * Header (3 words)
 *
 *  Header1 : [31: 0] Magic word 0xffff20d1
 *  Header2 : [31:24] 0xff
 *            [23:16] 0
 *            [15:15] Overflow bit
 *            [14:12] 0
 *            [10: 0] Number of Word
 *  Header3 : [31:24] 0xff
 *            [23:23] HRM enable bit (HRM exists)
 *            [22:20] 0
 *            [19:16] Tag
 *            [15: 0] Self-counter
 *
 * Data ( RM 1 word + MsT 1 word + HR-TDC block + LR-TDC block1 + LR-TDC block2 )
 * Body word always follow this data order.
 *
 *  RM     : [31:24] 0xf9
 *           [23:22] 0
 *           [21:21] Lock Bit
 *           [20:20] Spill Increment
 *           [19:12] Spill Number
 *           [11: 0] Event Number
 *
 *  MsT    : [31:16] 0xf200
 *           [15: 7] 0
 *           [ 6: 6] No decision
 *           [ 5: 5] Level2
 *           [ 4: 4] Fast clear
 *           [ 3: 3] MsT Consolation Accept
 *           [ 2: 2] MsT Final clear
 *           [ 1: 1] MsT Accept
 *           [ 0: 0] MsT Clear
 *
 * HR-TDC data block
 * HeaderA : [31:24] 0xf800
 *           [15:14] 0
 *           [13:13] Block over flow
 *           [12:12] Stop data out
 *           [11:11] Fine count through
 *           [10: 0] Block Number of word
 *
 *  HR-TDC : [31:29] 0x6
 *           [28:24] Channel
 *           [23: 0] TDC value
 *
 * LR-TDC data block1
 * HeaderB : [31:24] 0xfc10
 *           [15:14] 0
 *           [13:13] Block over flow
 *           [12:12] Stop data out
 *           [11:11] Fine count through
 *           [10: 0] Block Number of word
 *
 *  LR-TDC : [31:24] Data header (0xcc:leading)
 *           [23:23] 0
 *           [22:16] Channel
 *           [15:11] 0
 *           [10: 0] TDC value
 *
 * LR-TDC data block2
 * HeaderC : [31:24] 0xfc20
 *           [15:14] 0
 *           [13:13] Block over flow
 *           [12:12] Stop data out
 *           [11:11] Fine count through
 *           [10: 0] Block Number of word
 *
 *  LR-TDC : Same as the block1
 *
 * Header      : 3 words
 * RM          : 0/1
 * MsT         : 1
 * Sub hearder : 3
 * TDC         : Variable (16 hit/ch/event in maximum)
 * Total       : 7(8) - 1544
 *
 */

#ifndef HDDAQ__HULMST_UNPACKER_HH
#define HDDAQ__HULMST_UNPACKER_HH

#include "DAQNode.hh"

namespace hddaq
{

namespace unpacker
{

class HulMsT : public DAQNode, public Uncopyable<HulMsT>
{
public:
  static const unpacker_type k_type;
  static const uint32_t      k_n_one_block =  32U; // per 1 block
  static const uint32_t      k_n_channel   =  64U;
  enum e_data_type
    {
      k_hr_leading,
      k_lr_leading,
      k_mst_clear,
      k_mst_accept,
      k_mst_final_clear,
      k_mst_consolation_accept,
      k_mst_fast_clear,
      k_mst_level2,
      k_mst_no_decision,
      k_rm_event,
      k_rm_spill,
      k_rm_sinc,
      k_rm_lock,
      k_n_data_type
    };

  // Definition of header
  struct Header
  {
    uint32_t m_magic_word;
    uint32_t m_event_size;
    uint32_t m_event_counter;
  };

  // Event header -------------------------------------------------
  // Header 1
  static const uint32_t k_header_size     = sizeof(Header)/sizeof(uint32_t);
  static const uint32_t k_HEADER_MAGIC    = 0xffff20d1U;

  // Header 2

  static const uint32_t k_OVERFLOW_MASK   = 0x1U;
  static const uint32_t k_OVERFLOW_SHIFT  = 15U;
  static const uint32_t k_EVSIZE_MASK     = 0xfffU;
  static const uint32_t k_EVSIZE_SHIFT    = 0U;

  // Header 3
  static const uint32_t k_HRM_MASK        = 0x1U;
  static const uint32_t k_HRM_SHIFT       = 23U;
  static const uint32_t k_EVTAG_MASK      = 0xfU;
  static const uint32_t k_EVTAG_SHIFT     = 16U;
  static const uint32_t k_EVCOUNTER_MASK  = 0xffffU;
  static const uint32_t k_EVCOUNTER_SHIFT = 0U;

  // TAG -----------------------------------------------------------
  static const uint32_t k_LOCAL_TAG_ORIGIN = 0U;
  static const uint32_t k_MAX_LOCAL_TAG    = 0xffffU;
  static const uint32_t k_MAX_EVENT_TAG    = 0xfU;

  // HRM -----------------------------------------------------------
  static const uint32_t k_rm_data_index  = 4;

  static const uint32_t k_rm_magic       = 0xf9U;
  static const uint32_t k_rm_magic_mask  = 0xffU;
  static const uint32_t k_rm_magic_shift = 24U;

  static const uint32_t k_rm_event_mask  = 0xfffU;
  static const uint32_t k_rm_event_shift = 0U;
  static const uint32_t k_rm_spill_mask  = 0xffU;
  static const uint32_t k_rm_spill_shift = 12U;
  static const uint32_t k_rm_sinc_mask   = 0x1U;
  static const uint32_t k_rm_sinc_shift  = 20U;
  static const uint32_t k_rm_lock_mask   = 0x1U;
  static const uint32_t k_rm_lock_shift  = 21U;

  // MsT -----------------------------------------------------------
  static const uint32_t k_MST_MAGIC        = 0xf200;
  static const uint32_t k_mst_header_mask  = 0xffffU;
  static const uint32_t k_mst_header_shift = 16U;

  static const uint32_t k_mst_no_decision_mask         = 0x1U;
  static const uint32_t k_mst_no_decision_shift        = 6U;
  static const uint32_t k_mst_level2_mask              = 0x1U;
  static const uint32_t k_mst_level2_shift             = 5U;
  static const uint32_t k_mst_fast_clear_mask          = 0x1U;
  static const uint32_t k_mst_fast_clear_shift         = 4U;
  static const uint32_t k_mst_consolation_accept_mask  = 0x1U;
  static const uint32_t k_mst_consolation_accept_shift = 3U;
  static const uint32_t k_mst_final_clear_mask         = 0x1U;
  static const uint32_t k_mst_final_clear_shift        = 2U;
  static const uint32_t k_mst_accept_mask              = 0x1U;
  static const uint32_t k_mst_accept_shift             = 1U;
  static const uint32_t k_mst_clear_mask               = 0x1U;
  static const uint32_t k_mst_clear_shift              = 0U;

  // Data block ----------------------------------------------------
  static const uint32_t k_block_header_mask  = 0xffffU;
  static const uint32_t k_block_header_shift = 16U;

  // HR-TDC block --------------------------------------------------

  static const uint32_t k_HR_HEADER_MAGIC = 0xf800;

  static const uint32_t k_hr_nword_mask   = 0x3ffU;
  static const uint32_t k_hr_nword_shift  = 0U;

  static const uint32_t k_HR_MAGIC        = 0x6U;
  static const uint32_t k_hr_magic_mask   = 0x7U;
  static const uint32_t k_hr_magic_shift  = 29U;
  static const uint32_t k_hr_ch_mask      = 0x3fU;
  static const uint32_t k_hr_ch_shift     = 24U;
  static const uint32_t k_hr_data_mask    = 0xffffffU;
  static const uint32_t k_hr_data_shift   = 0U;

  // LR-TDC block --------------------------------------------------
  static const uint32_t k_LR1_HEADER_MAGIC = 0xfc10;
  static const uint32_t k_LR2_HEADER_MAGIC = 0xfc20;

  static const uint32_t k_lr_nword_mask   = 0x3ffU;
  static const uint32_t k_lr_nword_shift  = 0U;

  static const uint32_t k_LR_MAGIC        = 0xccU;
  static const uint32_t k_lr_magic_mask   = 0xffU;
  static const uint32_t k_lr_magic_shift  = 24U;
  static const uint32_t k_lr_ch_mask      = 0x3fU;
  static const uint32_t k_lr_ch_shift     = 16U;
  static const uint32_t k_lr_data_mask    = 0x7ffU;
  static const uint32_t k_lr_data_shift   = 0U;

private:
  Header*  m_header;
  iterator m_body_first;
  bool     m_has_rm;

public:
  HulMsT( const unpacker_type& type );
  virtual ~HulMsT();

protected:
  virtual void decode();
  virtual void check_data_format();
  virtual void resize_fe_data();
  virtual bool unpack();
  virtual void update_tag();

};

}

}

#endif
