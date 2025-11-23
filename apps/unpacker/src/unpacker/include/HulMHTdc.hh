// -*- C++ -*-

/**
 * Data structure of HUL MH-TDC
 *
 * Header (3 words)
 *
 *  Header1 : [31: 0] Magic word 0xffff30cc
 *  Header2 : [31:24] 0xff
 *            [23:16] 0
 *            [15:15] Overflow bit
 *            [14:13] 0
 *            [12: 0] Number of Word
 *  Header3 : [31:24] 0xff
 *            [23:23] HRM enable bit (HRM exists)
 *            [22:20] 0
 *            [19:16] Tag
 *            [15: 0] Self-counter
 *
 * Data ( RM 1 word + TDC block (Leading/Trailing) )
 *
 *  RM     : [31:24] 0xf9
 *           [23:22] 0
 *           [21:21] Lock Bit
 *           [20:20] Spill Increment
 *           [19:12] Spill Number
 *           [11: 0] Event Number
 *
 * TDC     : [31:24] Data header (0xcc:leading, 0xcd:trailing)
 *           [23:23] 0
 *           [22:16] Channel
 *           [15:14] 0
 *           [13: 0] TDC value
 *
 * Header : 3 words
 * RM     : 0/1
 * TDC    : Variable (16 hit/ch/event in maximum)
 * Total  : 3~4099 words
 *
 */

#ifndef HDDAQ__HULMHTDC_UNPACKER_HH
#define HDDAQ__HULMHTDC_UNPACKER_HH

#include "DAQNode.hh"

namespace hddaq
{

namespace unpacker
{

class HulMHTdc : public DAQNode, public Uncopyable<HulMHTdc>
{
public:
  static const unpacker_type k_type;
  static const uint32_t      k_n_one_block =  32U; // per 1 block
  static const uint32_t      k_n_channel   = 128U; // max channel
  enum e_data_type
    {
      k_leading,
      k_trailing,
      k_overflow,
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

  // Event header ----------------------------------------------------------
  // Header 1
  static const uint32_t k_header_size     = sizeof(Header)/sizeof(uint32_t);
  static const uint32_t k_HEADER_MAGIC    = 0xffff30ccU;

  // Header 2
  static const uint32_t k_OVERFLOW_MASK   = 0x1U;
  static const uint32_t k_OVERFLOW_SHIFT  = 15U;
  static const uint32_t k_EVSIZE_MASK     = 0x1fffU;
  static const uint32_t k_EVSIZE_SHIFT    = 0U;

  // Header 3
  static const uint32_t k_HRM_MASK          = 0x1U;
  static const uint32_t k_HRM_SHIFT         = 23U;
  static const uint32_t k_J0TAG_EVENT_MASK  = 0x7U;
  static const uint32_t k_J0TAG_EVENT_SHIFT = 16U;
  static const uint32_t k_J0TAG_SPILL_MASK  = 0x1U;
  static const uint32_t k_J0TAG_SPILL_SHIFT = 19U;
  static const uint32_t k_EVCOUNTER_MASK    = 0xffffU;
  static const uint32_t k_EVCOUNTER_SHIFT   = 0U;

  // TAG -------------------------------------------------------------------
  static const uint32_t k_LOCAL_TAG_ORIGIN  = 0U;
  static const uint32_t k_MAX_LOCAL_TAG     = 0xffffU;
  // HRM
  static const uint32_t k_MAX_EVENT_HRM_TAG = 0xfffU;
  static const uint32_t k_MAX_SPILL_HRM_TAG = 0xfU;
  // J0
  static const uint32_t k_MAX_EVENT_J0_TAG  = 0x7U;
  static const uint32_t k_MAX_SPILL_J0_TAG  = 0x1U;

  // HRM -------------------------------------------------------------------
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

  // TDC -------------------------------------------------------------------
  static const uint32_t k_LEADING_MAGIC     = 0xcc;
  static const uint32_t k_TRAILING_MAGIC    = 0xcd;
  static const uint32_t k_data_header_mask  = 0xffU;
  static const uint32_t k_data_header_shift = 24U;

  static const uint32_t k_tdc_ch_mask    = 0x7fU;
  static const uint32_t k_tdc_ch_shift   = 16U;
  static const uint32_t k_tdc_data_mask  = 0x3fffU;
  static const uint32_t k_tdc_data_shift = 0U;

private:
  Header*  m_header;
  iterator m_body_first;
  bool     m_has_rm;

public:
  HulMHTdc( const unpacker_type& type );
  virtual ~HulMHTdc();

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
