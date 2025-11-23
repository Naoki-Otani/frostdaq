// -*- C++ -*-

// Author: Rintaro Kurata
/***************************** Data structure of RAYRAW *****************************

 Header(3 words) + Data(Flash ADC & TDC)
 
    Header1 : [31: 0] Magic word (0xffff0160)

    Header2 : [31:20] 0xff0
              [19:19] 0
              [18:18] Event Overflow
              [17: 0] Number of Word w/o Header

    Header3 : [31:24] 0xff
              [23:23] 0 (enRM)
              [22:20] 0
              [19:16] 0 (trmTag)
              [15: 0] Self-counter (Event number)

        TDC : [31:24] Magic word (Leading: 0xcc, Trailing: 0xcd)
              [23:23] 0
              [22:16] Channel
              [15:15] 0
              [14: 0] TDC data (LSB : 1/(4*300MHz) = 0.8333 ns)

  Flash ADC : [31:28] Magic word (0xa)
              [27:23] Channel
              [22:21] 0
              [20:10] Coarse-counter (LSB : 1/75MHz = 13.3333 ns)
              [ 9: 0] ADC data

 
  Header    : 3 words
  TDC       : 0 - ???
  Flash ADC : depends on window size
  Total     : 3 -

  *********************** End of data structure of RAYRAW ***********************/



#ifndef HDDAQ__RAYRAW_UNPACKER_HH
#define HDDAQ__RAYRAW_UNPACKER_HH

#include "DAQNode.hh"

namespace hddaq
{

  namespace unpacker
  {

    class Rayraw : public DAQNode, public Uncopyable<Rayraw>
    {
    public:
      static const unpacker_type k_type;
      static const uint32_t      k_n_one_block  = 8;   //Channel per 1 ASIC
      static const uint32_t      k_n_channel    = 32;  //Channel per 1 board
      enum e_data_type
	{
	  k_fadc,
	  k_crs_cnt,
	  k_leading,
	  k_trailing,
	  k_overflow,
	  k_n_data_type
	};

      // Definition of Header
      struct Header
      {
	uint32_t m_magic_word;
	uint32_t m_event_size;
	uint32_t m_event_counter;
      };


      // Event Header -------------------------------------------------
      // Header 1
      static const uint32_t k_header_size      = sizeof(Header)/sizeof(uint32_t);
      static const uint32_t k_HEADER_MAGIC     = 0xffff0160U;
					       
      // Header 2			       
      static const uint32_t k_OVERFLOW_MASK    = 0x1U;
      static const uint32_t k_OVERFLOW_SHIFT   = 18U;
      static const uint32_t k_EVSIZE_MASK      = 0x3ffffU;
      static const uint32_t k_EVSIZE_SHIFT     = 0U;

      // Header 3
      static const uint32_t k_ENABLE_RM_MASK   = 0x1U;
      static const uint32_t k_ENABLE_RM_SHIFT  = 23U;
      static const uint32_t k_TRM_TAG_MASK     = 0xfU;
      static const uint32_t k_TRM_TAG_SHIFT    = 16U;
      static const uint32_t k_EVCOUNTER_MASK   = 0xffffU;
      static const uint32_t k_EVCOUNTER_SHIFT  = 0U;


      // TAG ----------------------------------------------------------
      static const uint32_t k_LOCAL_TAG_ORIGIN  = 0U;
      static const uint32_t k_MAX_LOCAL_TAG     = 0xffffU;


      // Data block ---------------------------------------------------
      // TDC
      static const uint32_t k_LEADING_MAGIC_WORD   = 0xccU;
      static const uint32_t k_TRAILING_MAGIC_WORD  = 0xcdU;
      static const uint32_t k_TDC_MAGIC_MASK       = 0xffU;
      static const uint32_t k_TDC_MAGIC_SHIFT      = 24U;
      static const uint32_t k_TDC_CH_MASK          = 0x7fU;
      static const uint32_t k_TDC_CH_SHIFT         = 16U;
      static const uint32_t k_TDC_DATA_MASK        = 0x7fffU;
      static const uint32_t k_TDC_DATA_SHIFT       = 0U;

      // Flash ADC
      static const uint32_t k_ADC_MAGIC_WORD     = 0xaU;
      static const uint32_t k_ADC_MAGIC_MASK     = 0xfU;
      static const uint32_t k_ADC_MAGIC_SHIFT    = 28U;
      static const uint32_t k_ADC_CH_MASK        = 0x1fU;
      static const uint32_t k_ADC_CH_SHIFT       = 23U;
      static const uint32_t k_ADC_CRS_CNT_MASK   = 0x7ffU;
      static const uint32_t k_ADC_CRS_CNT_SHIFT  = 10U;
      static const uint32_t k_ADC_DATA_MASK      = 0x3ffU;
      static const uint32_t k_ADC_DATA_SHIFT     = 0U;


    private:
      Header*  m_header;
      iterator m_body_first;

    public:
      Rayraw( const unpacker_type& type );
      virtual ~Rayraw();

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
