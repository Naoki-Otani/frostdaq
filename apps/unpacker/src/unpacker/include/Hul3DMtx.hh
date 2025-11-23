//-------------------------------------------------------------------------
// Internal TDC of HUL 3D matrix for E07
//-------------------------------------------------------------------------

/*                        Data structure of HUL MTX

  Header (2 words) + data : there is no trailer word
  
  Header1 : 0xffff3dcc (magic word)
  Header2 : Event couter (self counter in FPGA) + Event Tag
          : 0 bit       => LSB of Event Tag
	  : 16 - 28 bit => Event counter

 Data(TOF):[0x100f][000 ch(5bits)][tdc(8bits)]
 Data(FBH):[0xfb80][000 ch(5bits)][tdc(8bits)]
 Data(CH) :[0xc800][000 ch(5bits)][tdc(8bits)]

Header : 2 words
TOF    : 24
FBH    : 31
CH     : 64
Always, these 121 words reterned from HUL

                                                                      */

#ifndef HDDAQ__HUL3DMTX_UNPACKER_H
#define HDDAQ__HUL3DMTX_UNPACKER_H

#include"DAQNode.hh"

namespace hddaq{
  namespace unpacker{
    class Hul3DMtx : public DAQNode, public Uncopyable<Hul3DMtx>
    {
    public:
      static const unpacker_type k_type;
      // Prepare virtual input port
      // This module has 64ch input with 3 kinds of data type,
      // i.e., tof, fbh, and ch are equivalent to ADC, TDC, etc in other modules.
      static const uint32_t      k_n_port = 64U;

      // Actual segment size
      static const uint32_t      k_n_tof  = 24U;
      static const uint32_t      k_n_fbh  = 31U;
      static const uint32_t      k_n_ch   = 64U;
      static const uint32_t      k_n_data = k_n_tof + k_n_fbh + k_n_ch;

      // Definition of header
      struct Header{
	uint32_t m_hul3dmtx_magic;
	uint32_t m_event_counter;
      };

      static const uint32_t k_header_size     = sizeof(Header)/sizeof(uint32_t);
      static const uint32_t k_HEADER_MAGIC    = 0xffff3dccU;
      static const uint32_t k_EVCOUNTER_MASK  = 0xfffU;
      static const uint32_t k_EVCOUNTER_SHIFT = 16U;
      static const uint32_t k_EVTAG_MASK      = 0x1U;
      
      static const uint32_t k_LOCAL_TAG_ORIGIN       = 1U;
      static const uint32_t k_HUL3DMTX_MAX_LOCAL_TAG = 0xfffU;
      static const uint32_t k_HUL3DMTX_MAX_EVENT_TAG = 0x1U;
      
      // Defition of data 
      enum e_data_type{
	k_tfbh,
	k_ttof,
	k_tch,
	k_n_data_type
      };

      static const uint32_t k_data_type_mask  = 0xffff;
      static const uint32_t k_data_type_shift = 16;
      static const uint32_t k_TOF_MAGIC       = 0x100f;
      static const uint32_t k_FBH_MAGIC       = 0xfb80;
      static const uint32_t k_CH_MAGIC        = 0xc800;
      
      static const uint32_t k_tdc_data_mask   = 0xff;
      static const uint32_t k_ch_mask   = 0x3f;
      static const uint32_t k_ch_shift  = 8;

    private:
      Header*  m_header;
      iterator m_body_first;
      
    public:
      Hul3DMtx(const unpacker_type& type);
      virtual ~Hul3DMtx();

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
