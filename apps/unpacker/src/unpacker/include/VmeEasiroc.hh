//-----------------------------------------------------------------------------
// VME-EASIROC decoding class for unpacker
//-----------------------------------------------------------------------------

/*                        Data structure of VME-EASIROC                      */
/*
  Header (3 words) + data : there is no trailer word

  Header1 : 0x4d495741 (magic word)
  Header2 : Number of words (lower 12 bits)
  Header3 : Event couter Event Tag
          : 0 - 3 bit       => Event Tag(Spill# 1bit + Event# 3bits)

 Data(ADC):
     0 - 11 bit => ADC data
    12      bit => Over The Range
    13 - 18 bit => Channel
    19      bit => High gain(0) / Low gain(1)
    20      bit => Unused
    21      bit => Must 0
    22      bit => Must 0
    21 - 26 bit => Unused
    27      bit => Must 0

 Data(TDC):
     0 - 11 bit => TDC data, LSB = 1ns
    12      bit => Leading(0), Trailing(1)
    13 - 18 bit => Channel
    19 - 20 bit => Unused
    21      bit => Must 1
    20      bit => Must 0
    22 - 26 bit => Unused
    27      bit => Must 0
 Data(Scaler):
     0 - 12 bit => Scaler data
    13      bit => Overflow
    14 - 20 bit => Channel
    21      bit => Must 0
    22      bit => Must 1
    23 - 26 bit => Unused
    27      bit => Must 0
*/

#ifndef HDDAQ_VME_EASIROC_UNPACKER_H
#define HDDAQ_VME_EASIROC_UNPACKER_H

#include"DAQNode.hh"

namespace hddaq{
    namespace unpacker{
        class VmeEasiroc : public DAQNode, public Uncopyable<VmeEasiroc>
        {
            public:
                static const unpacker_type k_type;
                static const uint32_t      k_n_ch = 64 + 5;

                // Definition of header
                struct Header{
                    uint32_t m_vme_easiroc_magic;
                    uint32_t m_n_word;
                    uint32_t m_event_counter;
                };

                static const uint32_t k_header_size     = sizeof(Header)/sizeof(uint32_t);
                static const uint32_t k_HEADER_MAGIC    = 0x4157494d;
                static const uint32_t k_NWORD_MASK      = 0x00000fff;
	  //static const uint32_t k_EVCOUNTER_MASK  = 0x0000000f;
	  static const uint32_t k_EVCOUNTER_MASK  = 0x00000007;
	  static const uint32_t k_SPILLCOUNTER_MASK  = 0x00000008;

                static const uint32_t k_ADC_HIGH_MASK   = 0x08680000;
                static const uint32_t k_ADC_HIGH        = 0x00000000;
                static const uint32_t k_ADC_LOW_MASK    = 0x08680000;
                static const uint32_t k_ADC_LOW         = 0x00080000;
                static const uint32_t k_TDC_MASK        = 0x08600000;
                static const uint32_t k_TDC             = 0x00200000;

                static const uint32_t k_SCALER_MASK     = 0x08600000;
                static const uint32_t k_SCALER          = 0x00400000;

                // Defition of data 
                enum e_data_type{
                    k_leading     = 0,
                    k_trailing    = 1,
                    k_adc_high    = 2,
                    k_adc_low     = 3,
                    k_scaler      = 4,
                    k_n_data_type = 5,
                };

            private:
                Header*  m_header;
                iterator m_body_first;

            public:
                VmeEasiroc(const unpacker_type& type);
                virtual ~VmeEasiroc();

            protected:
                virtual void decode();
                virtual void check_data_format();
                virtual void resize_fe_data();
                virtual bool unpack();
                virtual void update_tag();

            private:
                unsigned int getEventCounter() {
		  return m_header->m_event_counter & k_EVCOUNTER_MASK ;
                }

                unsigned int getSpillCounter() {
		  return (m_header->m_event_counter & k_SPILLCOUNTER_MASK) >> 3 ;
                }

                bool isHg_(uint32_t word) {
                    return (word & k_ADC_HIGH_MASK) == k_ADC_HIGH;
                }
                bool isLg_(uint32_t word) {
                    return (word & k_ADC_LOW_MASK) == k_ADC_LOW;
                }
                bool isTdc_(uint32_t word) {
                    return (word & k_TDC_MASK) == k_TDC;
                }
                bool isScaler_(uint32_t word) {
                    return (word & k_SCALER_MASK) == k_SCALER;
                }
                uint32_t decodeWord_(uint32_t word) {
                    word = ((word & 0xff000000) >> 24) |
                           ((word & 0x00ff0000) >>  8) |
                           ((word & 0x0000ff00) <<  8) |
                           ((word & 0x000000ff) << 24);

                    uint32_t frame = word & 0x80808080;
                    const uint32_t NORMAL_FRAME = 0x80000000;
                    if(frame != NORMAL_FRAME) {
                        std::cerr << "VmeEasiroc: Frame Error!" << std::endl;
                    }
                    return ((word & 0x7f000000) >> 3) |
                           ((word & 0x007f0000) >> 2) |
                           ((word & 0x00007f00) >> 1) |
                           ((word & 0x0000007f) >> 0);
                }

                unsigned int getCh_(uint32_t word) {
                    return (word & 0x0007e000) >> 13;
                }

                unsigned int getOtr_(uint32_t word) {
                    return (word & 0x00001000) >> 12;
                }

                unsigned int getLt_(uint32_t word) {
		  return  (( (word & 0x00001000) >> 12 ) + 1) & 0x1;
                }

                unsigned int getData_(uint32_t word) {
                    return word & 0x00000fff;
                }
                unsigned int getScalerCh_(uint32_t word) {
                    return (word & 0x001fc000) >> 14;
                }
                unsigned int getScalerCount_(uint32_t word) {
                    return word & 0x00001fff;
                }
                bool getScalerOVF_(uint32_t word) {
                    return (word & 0x00002000) != 0;
                }
        };
    }
}

#endif
