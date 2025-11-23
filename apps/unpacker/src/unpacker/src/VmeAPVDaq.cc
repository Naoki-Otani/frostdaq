// -*- C++ -*-

// Author: Shuhei Hayakawa

#include "VmeAPVDaq.hh"

#include <cstdio>
#include <iomanip>

#include "std_ostream.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type VmeAPVDaq::k_type = "VmeAPVDaq";

//_______________________________________________________________________
VmeAPVDaq::VmeAPVDaq(const unpacker_type& type)
  : VmeModule(type),
    module_id(0)
{
  for(int chip=0; chip<nChip; ++chip){
    VmeAPVDaq::data_size_shift[chip] = chip*8;
    VmeAPVDaq::data_size_mask[chip]  = 0xff;
  }

  VmeAPVDaq::data_shift[0] =  0;
  VmeAPVDaq::data_shift[1] = 10;
  VmeAPVDaq::data_shift[2] =  0;
  VmeAPVDaq::data_shift[3] = 10;
  VmeAPVDaq::data_shift[4] = 20;
  VmeAPVDaq::data_shift[5] =  0;
  VmeAPVDaq::data_shift[6] = 10;
  VmeAPVDaq::data_shift[7] = 20;
  for(int sample=0; sample<(int)nSample; ++sample){
    VmeAPVDaq::data_mask[sample]  = 0x3ff;
  }
}

//_______________________________________________________________________
VmeAPVDaq::~VmeAPVDaq()
{
}

//_______________________________________________________________________
void
VmeAPVDaq::check_data_format()
{
  return;
}

//_______________________________________________________________________
void
VmeAPVDaq::decode()
{
  iterator_list::const_iterator f_begin = m_first_list.begin();
  iterator_list::const_iterator f_end   = m_first_list.end();
  for(iterator_list::const_iterator f=f_begin; f!=f_end; ++f){
    iterator first = *f;
    m_vme_header = reinterpret_cast<VmeModule::Header*>(&*first);
    m_module_data_first = first + VmeModule::k_header_size;

    const uint32_t *buf = reinterpret_cast<uint32_t*>(&*m_module_data_first);
    unsigned int data_size = m_vme_header->m_data_size - VmeModule::k_header_size;

    int chip_data_size[nChip] = {};
    int chip_id               = 0;
    int channel               = 0;
    int data[nSample]         = {};
    int suppression_flag      = 0;

    for(unsigned int i=0; i<data_size; ++i){
      switch(i){
      case WordModuleId:
	module_id = buf[i];
	continue;
      case WordDataSize:
	for(int chip=0; chip<nChip; ++chip){
	  chip_data_size[chip] =
	    (buf[i]>>data_size_shift[chip]) & data_size_mask[chip];
	  // cout<<"#D VmeAPVDaq::decode() ["<<std::setfill('0')<<std::setw(8)<<std::hex<<module_id
	  //     <<"] chip #"<<chip<<" data size : "<<chip_data_size[chip]<<std::endl;
	}
	continue;
      case WordDataBody:
	for(int chip=0; chip<nChip; ++chip){
	  for(int chip_data=0; chip_data<chip_data_size[chip]; ++chip_data){
	    for(int word=0; word<nWord; ++word){
	      switch(word){
	      case Word1:
		chip_id = (buf[i]>>chip_id_shift) & chip_id_mask;
		channel = (buf[i]>>channel_shift) & channel_mask;
		channel = 32*(channel%4)+8*int(channel/4)-31*int(channel/16);
		channel = chip_id*128 + channel;
		data[0] = (buf[i]>>data_shift[0]) & data_mask[0];
		data[1] = (buf[i]>>data_shift[1]) & data_mask[1];
		fill( channel, k_adc, data[0] );
		fill( channel, k_adc, data[1] );
		break;
	      case Word2:
		data[2] = (buf[i]>>data_shift[2]) & data_mask[2];
		data[3] = (buf[i]>>data_shift[3]) & data_mask[3];
		data[4] = (buf[i]>>data_shift[4]) & data_mask[4];
		fill( channel, k_adc, data[2] );
		fill( channel, k_adc, data[3] );
		fill( channel, k_adc, data[4] );
		break;
	      case Word3:
		suppression_flag = (buf[i]>>suppression_flag_shift) & suppression_flag_mask;
		data[5] = (buf[i]>>data_shift[5]) & data_mask[5];
		data[6] = (buf[i]>>data_shift[6]) & data_mask[6];
		data[7] = (buf[i]>>data_shift[7]) & data_mask[7];
		fill( channel, k_adc, data[5] );
		fill( channel, k_adc, data[6] );
		fill( channel, k_adc, data[7] );
		fill( channel, k_flag, suppression_flag );
		break;
	      default:
		break;
	      }//switch(word)
	      ++i;
	    }//for(word)
	  }//for(chip_data)
	}//for(chip)
	break;
      default:
	cerr<<"#E VmeAPVDaq::decode() strange data structure"<<std::endl;
      }//switch(i)
    }//for(i)
  }//for(f)
  return;
}

//_______________________________________________________________________
void
VmeAPVDaq::resize_fe_data()
{
  m_fe_data.resize(k_n_channel);
  for(unsigned int ch=0; ch<k_n_channel; ++ch){
    m_fe_data[ch].resize(k_n_data_type);
  }

  return;
}

}
}
