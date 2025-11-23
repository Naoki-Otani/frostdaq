// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__VME_SMP_UNPACKER_H
#define HDDAQ__VME_SMP_UNPACKER_H

#include <vector>
#include <map>

#include "VmeModule.hh"

namespace hddaq
{
  namespace unpacker
  {

    class Unpacker;

  class VmeSmp
    : public VmeModule,
      private Uncopyable<VmeSmp>
  {

  public:
    static const unpacker_type k_type;
    struct register_type
    {
      uint32_t m_star;  // (R/W) command (CMR;W)/ status (STAR;R) register
      uint32_t m_snccr; // (R/W) serial number counter control register
      uint32_t m_sncr;  // (R)   serial number counter register 
      uint32_t m_dsr;   // (R)   data size register
      uint32_t m_enr;   // (R)   event number counter register
      uint32_t m_bcr;   // (R/W) buffer control register
      uint32_t m_scr;   // (R/W) switch control register
      uint32_t m_icr;   // (R/W) interrupt control register
      uint32_t m_ivr;   // (R/W) interrupt vector register
    };

    enum e_register
      {
	k_STAR,
	k_SNCCR,
	k_SNCR,
	k_DSR,
	k_ENR,
	k_BCR,
	k_SCR,
	k_ICR,
	k_IVR,
	k_n_register
      };

  // CMR (W)
  // N/U | F[10:7] | RSET[6:6] | GO[5:5] | PSEL[4:0]
  //
  // STAR (R)
  // N/U | TOUT[15:15] | N/U | CBUF[13:13] | TKO FULL[12:12] | VME FULL [11:11]
  // | F[10:7] | XQ[6:6] | SEQ BUSY[5:5] | PSEL[4:0]
  //
  // SNCCR (R/W)
  // N/U | CNT CLR[1:1] | CNT AUTO CLR [0:0]
  //
  // SNCR (R)
  // N/U | SNUM[15:0]
  // 
  // DSR (R)
  // OVER FLOW[31:31] | SIZE[30:0]
  //
  // ENR (R)
  // N/U | ENUM[15:0]
  //
  // BCR (R/W)
  // N/U | CBUF[9:9] | SWITCH[8:8] | FLAG BIT 0[7:7] | FLG0 AUTO CLR[6:6]
  // | SW ENA[5:5] | SW AUTO CLR[4:4] | N/U | REQ MASK[1:0]
  //
  // SCR (R/W)
  // N/U | MAX DATA SIZE[19:12] | N/U | MAX EV NUM[9:2] | N/U
  //
  // ICR (R/W)
  // N/U | FLAG BIT 1[7:7] | FLG1 AUTO CLR[6:6] | INT ENA[5:5] 
  // | INT AUTO CLR [4:4] | N/U | IRQ LEVEL[2:0]
  //
  // IVR (R/W)
  // N/U | Interrupt Vector[7:0]

  // word
  // | MA [31:27] | SA [26:16] | DATA [15:0] |

    static const uint32_t k_SWITCH_TRAILER_MAGIC        = 0xffffffffU;
    static const uint32_t k_LOCAL_TAG_shift             = 16U;
    static const uint32_t k_LOCAL_TAG_mask              = 0xffff0000U;
    static const uint32_t k_LOCAL_TAG_ORIGIN            = 1U;
    static const uint32_t k_SDS_N_WORD_mask             = 0xffffU; 
    static const uint32_t k_MA_shift                    = 27U;//11+16      
    static const uint32_t k_MA_mask                     = 0xf8000000U;
    static const uint32_t k_SA_shift                    = 16U;
    static const uint32_t k_SA_mask                     = 0x7ff0000U;
    static const uint32_t k_DATA_mask                   = 0xffffU;
    static const uint32_t k_MAGIC_MODULE_DOWN_BIT       = 0xffU;
    static const uint32_t k_MAGIC_MODULE_DOWN_BIT_shift = 24U;
    static const uint32_t k_MODULE_DOWN_BIT_mask        = 0xffffffU;

    static const unsigned int k_n_tko = 24;

    struct Header
    {
      unsigned int m_event_id;
    };
    static const unsigned int k_header_size = sizeof(Header)/sizeof(uint32_t);
    enum e_event
      {
        k_event_normal = 0,
        k_event_first  = 1,
        k_event_last   = 2,
        k_n_event      = 3
      };


  protected:
    Header*                m_smp_header;
    uint32_t               m_sds_serial_number;
    uint32_t               m_sds_n_word;
    uint32_t               m_module_down;
    uint32_t               m_switch_trailer_magic;
    register_type*         m_register;
    std::vector<Unpacker*> m_tko;
    iterator               m_sds_begin;
    iterator               m_sds_end;

  public:
    VmeSmp(const unpacker_type& type);
    virtual ~VmeSmp();

    
  protected:
    virtual void check_data_format();
    virtual void decode();
    void         dump_data16() const;
    virtual const std::map<std::string, int>&
                 get_data_id() const;
    virtual void resize_fe_data();
    virtual bool set_tko_data(Unpacker* u,
			      unsigned int ma,
			      const iterator& data_first,
			      const iterator& data_last);
    virtual void sort_child();
    virtual bool unpack();
    virtual bool unpack_body();
    virtual bool unpack_header();
    virtual bool unpack_trailer();
    virtual void update_tag();

  };

  }
}
#endif
