//-----------------------------------------------------------------------------
// easiroc decoding class for unpacker
//-----------------------------------------------------------------------------

/*                        Data structure of EASIROC                          */
/*
  Header (3 words) + data : there is no trailer word
  
  Header1 : 0xffffea0c (magic word)
  Header2 : Number of words (lower 10 bits)
  Header3 : Event couter (self counter in FPGA) + Event Tag
          : 0 bit       => LSB of Event Tag
	  : 16 - 28 bit => Event counter

	   31                     15                       0 bit
 Data(TDC):    0xcc      ch (5 bit) LT bit    data (10 bit)
          : 0 - 9 bit   => TDC data, LSB = 1ns;
	  : 15 bit      => LT bit, 0 = Leading edge, 1 = Trailing edge
	  : 16 - 20 bit => ch
	  : 24 - 31 bit => TDC header (0xcc)
 Data(ADC):   0x81/60      ch (5 bit) 000 overflow bit  data (12 bit)
          : 0 - 11 bit  => ADC data
	  : 12 bit      => Overflow bit
	  : 16 - 20 bit => ch
	  : 24 - 31 bit => ADC header (0x81/60)
*/
/*                                                                           */

#ifndef HDDAQ__EASIROC_UNPACKER_H
#define HDDAQ__EASIROC_UNPACKER_H

#include"DAQNode.hh"

namespace hddaq{
  namespace unpacker{
    class Easiroc : public DAQNode, public Uncopyable<Easiroc>
    {
    public:
      static const unpacker_type k_type;
      static const uint32_t      k_n_ch = 32U;

      // Definition of header
      struct Header{
	uint32_t m_easiroc_magic;
	uint32_t m_n_word;
	uint32_t m_event_counter;
      };

      static const uint32_t k_header_size     = sizeof(Header)/sizeof(uint32_t);
      static const uint32_t k_HEADER_MAGIC    = 0xffffea0cU;
      static const uint32_t k_NWORD_MASK      = 0x3ffU;
      static const uint32_t k_EVCOUNTER_MASK  = 0xfffU;
      static const uint32_t k_EVCOUNTER_SHIFT = 16U;
      static const uint32_t k_EVTAG_MASK      = 0x1U;
      
      static const uint32_t k_LOCAL_TAG_ORIGIN      = 0U;
      static const uint32_t k_EASIROC_MAX_LOCAL_TAG = 0xfffU;
      static const uint32_t k_EASIROC_MAX_EVENT_TAG = 0x1U;
      
      // Defition of data 
      enum e_data_type{
	k_leading,
	k_trailing,
	k_adc_high,
	k_adc_low,
	k_n_data_type
      };

      static const uint32_t k_data_type_mask  = 0xff;
      static const uint32_t k_data_type_shift = 24;
      static const uint32_t k_TDC_MAGIC       = 0xcc;
      static const uint32_t k_ADC_HI_MAGIC    = 0x81;
      static const uint32_t k_ADC_LO_MAGIC    = 0x60;
      
      static const uint32_t k_tdc_data_mask   = 0x3ff;
      static const uint32_t k_adc_data_mask   = 0x1fff; // include overflow bit
      static const uint32_t k_lt_mask   = 0x1;
      static const uint32_t k_lt_shift  = 15;
      static const uint32_t k_ch_mask   = 0x1f;
      static const uint32_t k_ch_shift  = 16;

    private:
      Header*  m_header;
      iterator m_body_first;
      
    public:
      Easiroc(const unpacker_type& type);
      virtual ~Easiroc();

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
