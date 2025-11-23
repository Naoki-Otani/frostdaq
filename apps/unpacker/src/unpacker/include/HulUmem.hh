// -*- C++ -*-

// Author: Shuhei Hayakawa

/**
 * Data structure of HUL Scaler
 *
 * Header (3 words)
 *
 *  Header1 : [31: 0] Magic word 0xffff4ca1
 *  Header2 : [31:24] 0xff
 *            [23:12] 0
 *            [11: 0] Number of Word
 *  Header3 : [31:24] 0xff
 *            [23:20] 0
 *            [19:16] Tag
 *            [15: 0] Self-counter
 *
 * Data ( RM 1 word + Scaler 96 word )
 *
 *  RM     : [31:24] 0xf9
 *           [23:22] 0
 *           [21:21] Lock Bit
 *           [20:20] Spill Increment
 *           [19:12] Spill Number
 *           [11: 0] Event Number
 *  Scaler : [31:28] Scaler Block
 *           [27: 0] Counter
 *
 * Header : 3 words
 * RM     : 0/1
 * Scaler : 32 words * (0~4) blocks
 * Total  : 3~132 words
 *
 */

#ifndef HDDAQ__HULUMEM_UNPACKER_HH
#define HDDAQ__HULUMEM_UNPACKER_HH

#include "DAQNode.hh"

namespace hddaq
{

namespace unpacker
{

class HulUmem : public DAQNode, public Uncopyable<HulUmem>
{
public:
  static const unpacker_type k_type;
  static const uint32_t      k_n_one_block =  32U; // per 1 block
  static const uint32_t      k_n_channel   = 128U; // max channel
  enum e_data_type
    {
      k_adc,
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
  static const uint32_t k_HEADER_MAGIC    = 0xffffc3e3;

  // Header 2
  static const uint32_t k_EVSIZE_MASK     = 0x7ffU;
  static const uint32_t k_EVSIZE_SHIFT    = 0U;

  // Header 3
  static const uint32_t k_HRM_MASK          = 0x1U;
  static const uint32_t k_HRM_SHIFT         = 23U;
  static const uint32_t k_J0TAG_EVENT_MASK  = 0x7U;
  static const uint32_t k_J0TAG_EVENT_SHIFT = 16U;
  static const uint32_t k_J0TAG_SPILL_MASK  = 0x1U;
  static const uint32_t k_J0TAG_SPILL_SHIFT = 19U;
  static const uint32_t k_EVCOUNTER_MASK  = 0xffffU;
  static const uint32_t k_EVCOUNTER_SHIFT = 0U;

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
  static const uint32_t k_rm_sinc_mask  = 0x1U;
  static const uint32_t k_rm_sinc_shift = 20U;
  static const uint32_t k_rm_lock_mask  = 0x1U;
  static const uint32_t k_rm_lock_shift = 21U;

  // UMEM -------------------------------------------------------------------
  static const uint32_t k_ADC_HEADER_MAGIC = 0x1U;
  static const uint32_t k_adc_header_mask  = 0x1U;
  static const uint32_t k_adc_header_shift = 15U;
  static const uint32_t k_wrdcnt_mask       = 0x3U;
  static const uint32_t k_wrdcnt_shift      = 11U;
  static const uint32_t k_vsn_mask          = 0xffU;
  static const uint32_t k_vsn_shift         = 0U;

  static const uint32_t k_subaddr_mask   = 0x3U;
  static const uint32_t k_subaddr_shift  = 13U;
  static const uint32_t k_adc_data_mask  = 0x1fffU;
  static const uint32_t k_adc_data_shift = 0U;

private:
  Header*  m_header;
  iterator m_body_first;
  bool     m_has_rm;
  //uint32_t m_n_scaler_block;

public:
  HulUmem( const unpacker_type& type );
  virtual ~HulUmem();

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
