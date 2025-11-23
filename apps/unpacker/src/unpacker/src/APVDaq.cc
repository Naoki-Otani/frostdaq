// -*- C++ -*-

#include "APVDaq.hh"

namespace hddaq
{
  namespace unpacker
  {

    const defines::unpacker_type APVDaq::k_type = "APVDaq";

    //_______________________________________________________________________
    APVDaq::APVDaq(const unpacker_type& type)
      : VmeModule(type),
	module_id(0)
    {

    }

    //_______________________________________________________________________
    APVDaq::~APVDaq()
    {

    }

    //_______________________________________________________________________
    void
    APVDaq::decode()
    {
      // bool tdc_flag = true;
      for(const_iterator i = m_module_data_first;i!=m_data_last;++i)
	{
	  int apv_value = *i;

	  // if(apv_value == k_data_tdc_flag){
	  //   tdc_flag = false;
	  //   continue;
	  // }

	  int mask = apv_value & k_data_counter_mask;
	  if(apv_value == k_data_trailer){break;}
	  if(mask == k_data_module_id)
	    {
	      module_id = apv_value >> k_data_module_id_shift;
	    }
	  else
	    {
	      //	      int error_bit = apv_value & k_data_error_mask;
	      int hit_adc   = (apv_value>>k_data_adc_shift) & k_data_adc_mask;
	      int hit_ch    = (apv_value>>k_data_ch_shift) & k_data_ch_mask;
	      int sample    = (apv_value>>k_data_sp_shift) & k_data_sp_mask;

	      if(sample == 0)continue;
	      hit_ch = ((sample-1)/6)*128 +
		(32*(hit_ch%4)+8*int(hit_ch/4)-31*int(hit_ch/16));
	      fill(hit_ch, module_id, hit_adc);//<-(ch_id,module_id,data)
	      // cout << std::dec << "Module : " << module_id<< ", Ch : " << hit_ch
	      // 	   << ", Adc : " << hit_adc << std::endl;
	    }
	}
      return;
      // "fill" corresponds to "push_back"
    }

    //_______________________________________________________________________
    void
    APVDaq::resize_fe_data()
    {
      m_fe_data.resize(k_n_ch);
      for(unsigned int ch = 0; ch<k_n_ch; ++ch)
	m_fe_data[ch].resize(k_n_module);
      return;
    }
  }
}
