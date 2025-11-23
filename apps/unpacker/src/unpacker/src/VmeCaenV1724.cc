// -*- C++ -*-

// Author: Tomonori Takahashi

#include "VmeCaenV1724.hh"

#include <algorithm>

#include "std_ostream.hh"
#include "HexDump.hh"
#include "defines.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type VmeCaenV1724::k_type = "VmeCaenV1724";

//______________________________________________________________________________
VmeCaenV1724::VmeCaenV1724(const unpacker_type& type)
  : VmeModule(type)
{
  Tag& orig = m_tag[k_tag_origin].back();
  orig.m_local = k_local_tag_origin;

  Tag& max = m_tag[k_tag_max].back();
  max.m_local = k_local_tag_max;
  max.m_event = 0;
  max.m_spill = 0;
}

//______________________________________________________________________________
VmeCaenV1724::~VmeCaenV1724()
{
}

//______________________________________________________________________________
void
VmeCaenV1724::decode()
{
  iterator_list::const_iterator f_begin = m_first_list.begin();
  iterator_list::const_iterator f_end   = m_first_list.end();
  for (iterator_list::const_iterator f  = f_begin;  f!=f_end; ++f)
    {
      iterator first = *f;
      m_vme_header = reinterpret_cast<VmeModule::Header*>(&*first);
      m_module_data_first = first + VmeModule::k_header_size;

      const uint32_t* buf = reinterpret_cast<uint32_t*>(&*m_module_data_first);

      uint32_t header_type = ((buf[0] >> k_header_shift) & k_header_mask);
      if(header_type != k_HEADER_MAGIC){
	cerr << "\n#E " << header_type << " vme module header \n"  << std::endl;
	return;
      }

      uint32_t event_size  = (buf[0] & k_event_size_mask);
      uint32_t mode = ((buf[1] >> k_mode_shift) & k_mode_mask);
      uint32_t ch_mask = (buf[1] & k_ch_mask_mask);

      unsigned int ch_num = 0;
      for(int i=0; i<24; i++)
	ch_num += ((ch_mask>>i) & 0x01);

      //defaule mode
      if(mode == k_MODE_FALSE_MAGIC){

	unsigned int sample_size;
	if(ch_num!=0)
	  sample_size = (event_size-4)/ch_num;
	else
	  sample_size = 0;

	unsigned int i = 4;
	unsigned int d_size[24];

	for(unsigned int ch = 0 ; ch<8; ch++ ){
	  int tch = ch;// + 8*no;
	  d_size[tch] = ((ch_mask >> ch) & 0x01 ) *sample_size;
	  for(unsigned int j=0; j<d_size[tch]; j++, i++){
	    unsigned int data_even = ((buf[i] >> 0) & k_data_mask);
	    fill(tch, 0  , data_even);
	    unsigned int data_odd = ((buf[i] >> 16) & k_data_mask);
	    fill(tch, 0, data_odd);
	  }
	}


      }
      else if(mode == k_MODE_TRUE_MAGIC){//PedSup mode
	unsigned int ZLEdata[24][20000];
	unsigned int line=4;
	bool control =true;
	bool good = false;
	unsigned int nword=0,nread=0,nwrite=0;
	unsigned int d_size[24];

	for(int ch =0;ch<8;ch++){
	  int tch = ch;// + 8*no;
	  if( (ch_mask>>ch &0x01 )==0x01){
	    d_size[tch]=buf[line++]-1;
	    for(unsigned int j=0;j<d_size[tch];j++)
	      ZLEdata[tch][j] = buf[line++];
	  }
	  else
	    d_size[tch] =0;

	  nwrite=0;
	  for(unsigned int j=0;j<d_size[tch];j++){
	    if(control){
	      good = (ZLEdata[tch][j]>>31)&0x01;
	      nword =(ZLEdata[tch][j])&0xFFFFF;
	      nread=0;
	      if(good)
		control = false;
	      else{
		for(unsigned int k=0;k<nword;k++){
		  fill(tch, 0, 0xFFFF);
		  fill(tch, 0, 0xFFFF);
		  nwrite++;
		}
	      }
	      continue;
	    }
	    if(good){
	      unsigned int data_even = ((ZLEdata[tch][j] >> 0) & k_data_mask);
	      fill(tch, 0, data_even);
	      unsigned int data_odd = ((ZLEdata[tch][j] >> 16) & k_data_mask);
	      fill(tch, 0, data_odd);
	      nread++;
	      nwrite++;
	    }

	    if(nread == nword)
	      control =true;
	  }
	  d_size[tch] = nwrite;
	}

      }



    }
  return;
}
//______________________________________________________________________________
void
VmeCaenV1724::update_tag()
{
  m_module_data_first = m_data_first + VmeModule::k_header_size;

  uint32_t buf = *(m_module_data_first+2);
  uint32_t event_number = buf & k_event_number_mask;
  //  std::cout << event_number << std::endl;

  Tag& tag = m_tag[k_tag_current].back();
  tag.m_local = event_number;
  m_has_tag.set(k_local);
}


//______________________________________________________________________________
void
VmeCaenV1724::resize_fe_data()
{
  m_fe_data.resize(k_n_ch);
  for(unsigned int i = 0; i<k_n_ch; ++i){
    m_fe_data[i].resize(k_n_data_type);
  }

  return;
}

}
}
