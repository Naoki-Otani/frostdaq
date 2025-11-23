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
 * Data ( RM 1 word + Fera 0-24 word + Coin 4 word + Tag 1 word )
 *
 *  RM     : [31:24] 0xf9
 *           [23:22] 0
 *           [21:21] Lock Bit
 *           [20:20] Spill Increment
 *           [19:12] Spill Number
 *           [11: 0] Event Number
 *  Fera   : [31:24] 0xfe
 *           [20:16] Channel
 *           [10: 0] Data
 *  Coin   : [31:24] 0xfc
 *           [18:16] Coin Block
 *           [15: 0] Channel
 *  Tag    : [31:24] 0xfa
 *           [ 2: 0] Trigger Flag
 *
 * Header : 3 words
 * RM     : 0/1
 * Fera   : 0~24 words
 * Coin   : 4 words
 * Tag    : 1 words
 * Total  : 8~33 words
 *
 */

#ifndef HDDAQ__HUL_MASSTRIG_FERA_UNPACKER_HH
#define HDDAQ__HUL_MASSTRIG_FERA_UNPACKER_HH

#include "DAQNode.hh"

namespace hddaq
{

namespace unpacker
{

class HulMassTrigFera : public DAQNode, public Uncopyable<HulMassTrigFera>
{
public:
  static const unpacker_type k_type;
  static const uint32_t      k_n_channel = 64U; // max channel
  enum e_data_type
    {
      k_fera_data,
      k_coin_data,
      k_tag_data,
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

  static const uint32_t k_header_size     = sizeof(Header)/sizeof(uint32_t);
  static const uint32_t k_HEADER_MAGIC    = 0xffff3a55U;

  static const uint32_t k_EVCOUNTER_MASK  = 0xffffU;
  static const uint32_t k_EVCOUNTER_SHIFT = 0U;
  static const uint32_t k_EVSIZE_MASK     = 0x7ffU;
  static const uint32_t k_EVSIZE_SHIFT    = 0U;
  static const uint32_t k_EVTAG_MASK      = 0xfU;
  static const uint32_t k_EVTAG_SHIFT     = 16U;

  static const uint32_t k_LOCAL_TAG_ORIGIN = 0U;
  static const uint32_t k_MAX_LOCAL_TAG    = 0xffffU;
  static const uint32_t k_MAX_EVENT_TAG    = 0xfU;

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

  // Fera/Coin/Tag data
  static const uint32_t k_data_magic_mask  = 0xffU;
  static const uint32_t k_data_magic_shift = 24U;

  static const uint32_t k_fera_magic       = 0xfeU;
  static const uint32_t k_fera_channel_mask  = 0x1fU;
  static const uint32_t k_fera_channel_shift = 16U;
  static const uint32_t k_fera_data_mask  = 0x7ffU;
  static const uint32_t k_fera_data_shift = 0U;

  static const uint32_t k_coin_magic       = 0xfcU;
  static const uint32_t k_coin_block_mask  = 0x3U;
  static const uint32_t k_coin_block_shift = 16U;
  static const uint32_t k_coin_channel_mask  = 0xffffU;
  static const uint32_t k_coin_channel_shift = 0U;
  static const uint32_t k_n_one_block = 16U; // per one block

  static const uint32_t k_tag_magic = 0xfaU;
  static const uint32_t k_tag_mask  = 0x7U;
  static const uint32_t k_tag_shift = 0U;


private:
  Header*  m_header;
  iterator m_body_first;
  bool     m_has_rm;

public:
  HulMassTrigFera( const unpacker_type& type );
  virtual ~HulMassTrigFera();

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
