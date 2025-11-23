// -*- C++ -*-

// Author: Tomonori Takahashi

#include "VmeCaenV1743.hh"

#include <algorithm>

#include "std_ostream.hh"
#include "HexDump.hh"
#include "defines.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type VmeCaenV1743::k_type = "VmeCaenV1743";

//______________________________________________________________________________
VmeCaenV1743::VmeCaenV1743(const unpacker_type& type)
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
VmeCaenV1743::~VmeCaenV1743()
{
}

//______________________________________________________________________________
void
VmeCaenV1743::decode()
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
      uint32_t group_mask = (buf[1] & k_group_mask_mask);
      
      unsigned int group_num = 0;
      for(uint32_t i=0; i<k_n_group; i++) {
	if (((group_mask>>i) & 0x01) == 1) {
	  group_num ++;
	}
      }

      //defaule mode
      if(1){

	unsigned int sample_size;
	if(group_num!=0)
	  sample_size = (event_size-4)/group_num;
	else
	  sample_size = 0;

	unsigned int i = 4;
	unsigned int d_size[24];

	for(unsigned int gr = 0 ; gr<k_n_group; gr++ ){
	  int tgr = gr;// + 8*no;
	  d_size[tgr] = ((group_mask >> gr) & 0x01 ) *sample_size;
	  for(unsigned int j=0; j<d_size[tgr]; j++, i++){
	    if (j==0) {
	      // group header
	      if (((buf[i] >> k_group_header_shift) & k_group_header_mask) 
		  != k_group_HEADER_MAGIC)
		std::cout << "Group header error : " << buf[i];
	    } else if (j==d_size[tgr]-1) {
	      // trailer header
	      if (((buf[i] >> k_group_trailer_shift) & k_group_trailer_mask) 
		  != k_group_TRAILER_MAGIC)
		std::cout << "Group trailer error : " << buf[i];
	    } else {

	      if (j%17==0)
		continue;

	      unsigned int data_even = ((buf[i] >> k_data_shift_even) & k_data_mask);
	      if ((data_even>>11 & 0x1) == 1)
		data_even = data_even - 0xfff + 2046;
	      else
		data_even = data_even + 2046;

	      int ch = 2*tgr;
	      fill(ch, 0  , data_even);


	      unsigned int data_odd = ((buf[i] >> k_data_shift_odd) & k_data_mask);
	      if ((data_odd>>11 & 0x1) == 1)
		data_odd = data_odd - 0xfff + 2046;
	      else
		data_odd = data_odd + 2046;

	      ch = 2*tgr + 1;
	      fill(ch, 0, data_odd);
	    }
	  }
	}
      }
    }
  return;
}
//______________________________________________________________________________
void
VmeCaenV1743::update_tag()
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
VmeCaenV1743::resize_fe_data()
{
  m_fe_data.resize(k_n_ch);
  for(unsigned int i = 0; i<k_n_ch; ++i){
    m_fe_data[i].resize(k_n_data_type);
  }

  return;
}

}
}
