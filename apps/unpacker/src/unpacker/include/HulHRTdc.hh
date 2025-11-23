// -*- C++ -*-

/**
 * Data structure of HUL HRTdc
 *
 * Header (3 words)
 *
 *  Header1 : [31: 0] Magic word 0xffff80eb
 *  Header2 : [31:24] 0xff
 *            [23:16] 0
 *            [15:15] Overflow bit
 *            [14:12] 0
 *            [10: 0] Number of Word
 *  Header3 : [31:24] 0xff
 *            [23:20] 0
 *            [19:16] Tag
 *            [15: 0] Self-counter
 *
 * Data ( HR-TDC block1 + HR-TDC block2 )
 * Body word always follow this data order.
 *
 * HR-TDC data block1
 * HeaderA : [31:24] 0xfa00
 *           [15:14] 0
 *           [13:13] Block over flow
 *           [12:12] Stop data out
 *           [11:11] Fine count through
 *           [10: 0] Block Number of word
 *
 *  HR-TDC : [31:29] Data type (0x6:leading, 0x5:trailing, 0x4:common stop)
 *           [28:24] Channel
 *           [23: 0] TDC value
 *
 * LH-TDC data block2
 * HeaderB : [31:24] 0xfb00
 *           [15:14] 0
 *           [13:13] Block over flow
 *           [12:12] Stop data out
 *           [11:11] Fine count through
 *           [10: 0] Block Number of word
 *
 *  HR-TDC : Same as the block1
 *
 * Header      : 3 words
 * Sub hearder : 2
 * TDC         : Variable (16 hit/ch/event in maximum)
 * TDC         : Common stop (optional)
 * Total       : 5 - 2055
 *
 */

#ifndef HDDAQ__HULHRTDC_UNPACKER_HH
#define HDDAQ__HULHRTDC_UNPACKER_HH

#include "DAQNode.hh"

namespace hddaq
{

namespace unpacker
{

class HulHRTdc : public DAQNode, public Uncopyable<HulHRTdc>
{
public:
  static const unpacker_type k_type;
  static const uint32_t      k_n_one_block =  32U; // per 1 block
  static const uint32_t      k_n_channel   =  64U;
  enum e_data_type
    {
      k_leading,
      k_trailing,
      k_stop,
      k_overflow,
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
  static const uint32_t k_HEADER_MAGIC_1  = 0xffff800bU;
  static const uint32_t k_HEADER_MAGIC_2  = 0xffff80ebU;

  // Header 2
  static const uint32_t k_OVERFLOW_MASK   = 0x1U;
  static const uint32_t k_OVERFLOW_SHIFT  = 15U;
  static const uint32_t k_EVSIZE_MASK     = 0xfffU;
  static const uint32_t k_EVSIZE_SHIFT    = 0U;

  // Header 3
  static const uint32_t k_J0TAG_EVENT_MASK  = 0x7U;
  static const uint32_t k_J0TAG_EVENT_SHIFT = 16U;
  static const uint32_t k_J0TAG_SPILL_MASK  = 0x1U;
  static const uint32_t k_J0TAG_SPILL_SHIFT = 19U;
  static const uint32_t k_EVCOUNTER_MASK    = 0xffffU;
  static const uint32_t k_EVCOUNTER_SHIFT   = 0U;

  // TAG -----------------------------------------------------------
  static const uint32_t k_LOCAL_TAG_ORIGIN = 0U;
  static const uint32_t k_MAX_LOCAL_TAG     = 0xffffU;
  // HRM
  static const uint32_t k_MAX_EVENT_HRM_TAG = 0xfffU;
  static const uint32_t k_MAX_SPILL_HRM_TAG = 0xfU;
  // J0
  static const uint32_t k_MAX_EVENT_J0_TAG  = 0x7U;
  static const uint32_t k_MAX_SPILL_J0_TAG  = 0x1U;

  // Data block ----------------------------------------------------
  static const uint32_t k_block_header_mask  = 0xffffU;
  static const uint32_t k_block_header_shift = 16U;

  // HR-TDC block --------------------------------------------------
  static const uint32_t k_BLOCK1_HEADER_MAGIC = 0xfa00;
  static const uint32_t k_BLOCK2_HEADER_MAGIC = 0xfb00;

  static const uint32_t k_block_nword_mask   = 0x3ffU;
  static const uint32_t k_block_nword_shift  = 0U;

  static const uint32_t k_LEADING_MAGIC      = 0x6U;
  static const uint32_t k_TRAILING_MAGIC     = 0x5U;
  static const uint32_t k_STOP_MAGIC         = 0x4U;
  static const uint32_t k_magic_mask         = 0x7U;
  static const uint32_t k_magic_shift        = 29U;
  static const uint32_t k_ch_mask            = 0x1fU;
  static const uint32_t k_ch_shift           = 24U;
  static const uint32_t k_data_mask          = 0xffffffU;
  static const uint32_t k_data_shift         = 0U;

private:
  Header*  m_header;
  iterator m_body_first;

public:
  HulHRTdc( const unpacker_type& type );
  virtual ~HulHRTdc();

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
