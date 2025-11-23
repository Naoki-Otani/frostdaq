// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__TKO_MULTI_HIT_TDC_UNPACKER_H
#define HDDAQ__TKO_MULTI_HIT_TDC_UNPACKER_H

#include "Tko.hh"

namespace hddaq
{
  namespace unpacker
  {

    class TkoMHTdc
      : public Tko,
	private Uncopyable<TkoMHTdc>
    {

    public:
      static const unpacker_type k_type;
  // TKO MHTDC supports only Single measurement data 
      enum e_packet_type
	{
	  k_TDC_header                = 0xaU, // 1010
	  k_TDC_trailer               = 0xcU, // 1100
	  k_Mask_flags                = 0x2U, // 0010
	  k_Single_measurement_data   = 0x3U, // 0011
	  k_Combined_measurement_data = 0x4U, // 0100
	  k_Errors                    = 0x6U, // 0110
	  k_Debugging_data            = 0x7U  // 0111
	};
  
      static const unsigned int k_PACKET_TYPE_mask   = 0xf000U;
      static const unsigned int k_PACKET_TYPE_shift  = 12U;
      static const unsigned int k_TDC_ID_mask        = 0xf00U;
      static const unsigned int k_TDC_ID_shift       = 8U;
      static const unsigned int k_N_TDC              = 7U; // tdc_id[0:6]
      static const unsigned int k_TDC_CH_mask        = 0xf8U;
      static const unsigned int k_TDC_CH_shift       = 3U;
      static const unsigned int k_N_TDC_CH           = 24U; // tdc_ch[0:23]
      static const unsigned int k_EDGE_TYPE_mask     = 4U; 
      static const unsigned int k_EDGE_TYPE_LEADING  = 1U;
      static const unsigned int k_EDGE_TYPE_TRAILING = 0U; 
      static const unsigned int k_HIT_ERROR_mask     = 2U; 
      static const unsigned int k_DATA_TYPE_mask     = 6U;
      static const unsigned int k_DATA_TYPE_shift    = 1U;
      static const unsigned int k_MSB_mask           = 1U;
      static const unsigned int k_MSB_shift          = 16U;

      static const unsigned int k_n_ch             = 130U;
      static const unsigned int k_n_ch_valid       = 128U;
      static const unsigned int k_ch_trigger_error = 128U;
      static const unsigned int k_ch_invalid       = 129U;

      enum e_data_type
	{
	  k_trailing               = 0,
	  k_trailing_error         = 1,
	  k_leading                = 2,
	  k_leading_error          = 3,
	  k_n_data_type_valid      = 4,

	  k_invalid_packet_type    = 0,
	  k_invalid_packet_tdc_ch  = 1,
	  k_invalid_packet_tdc_id  = 2,
	  k_invalid_packet_data    = 3,
	  k_trigger_error          = 4,
	  k_n_data_type_invalid    = 5
	};


    public:
      TkoMHTdc(const unpacker_type& type);
      virtual ~TkoMHTdc();

    protected:
      virtual void decode();
      virtual void dump_in_binary() const;
      virtual const std::map<std::string, int>&
                   get_data_id() const;
      unsigned int get_data_type(unsigned int word_low) const;
      int          get_input_ch(unsigned int tdc_id,
				unsigned int tdc_ch) const;
      unsigned int get_measurement_data(unsigned int word_low,
					unsigned int word_high) const;
      unsigned int get_packet_type(unsigned int word_low) const;
      unsigned int get_tdc_ch(unsigned int word_low) const;
      unsigned int get_tdc_id(unsigned int word_low) const;
      bool         is_tdc_ch_valid(unsigned int tdc_ch) const;
      bool         is_tdc_id_valid(unsigned int tdc_id) const;
      virtual void resize_fe_data();
//       virtual void set_data(unsigned int data);

    };

  }
}
#endif
