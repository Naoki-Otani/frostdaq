// -*- C++ -*-

// Author: Tomonori Takahashi

#include "VmeRpv100.hh"

#include <algorithm>
#include <iomanip>

#include "std_ostream.hh"
#include "HexDump.hh"

namespace hddaq
{

namespace unpacker
{

  namespace 
  {

    //__________________________________________________________________________
    uint32_t
    bcd_to_dec8(const uint16_t val[2])
    {
//       cout << "#D bcd to dec (8 digits)" << std::endl;
      std::vector<uint32_t> tmp(val, val+2);
      
      uint32_t t[8];
      t[0] = ((tmp[0] << 16U)  & 0xf0000000U) >> 28U;
      t[1] = ((tmp[0] << 16U)  & 0x0f000000U) >> 24U;
      t[2] = ((tmp[0] << 16U)  & 0x00f00000U) >> 20U; 
      t[3] = ((tmp[0] << 16U)  & 0x000f0000U) >> 16U; 
      t[4] = ( tmp[1]          & 0x0000f000U) >> 12U;
      t[5] = ( tmp[1]          & 0x00000f00U) >>  8U;
      t[6] = ( tmp[1]          & 0x000000f0U) >>  4U;
      t[7] =   tmp[1]          & 0x0000000fU;

      uint32_t ret
	= t[0] * 10000000U
	+ t[1] * 1000000U
	+ t[2] * 100000U
	+ t[3] * 10000U
	+ t[4] * 1000U
	+ t[5] * 100U
	+ t[6] * 10U
	+ t[7];

//       for (int i=0; i<8; ++i)
// 	{
// 	  if (t[i]>9)
// 	    throw ret;
// 	}

//       cout << std::hex << val[0] << " " << val[1] 
// 	   << "\n -> " << tmp[0] << " " << tmp[1];
// //     for (int i=0; i<8; ++i)
// //       cout << std::setfill('0') << std::setw(8) << t[i] << " ";
//       cout << "\n -> " << std::dec << ret <<  std::endl;;

      return ret;
    }

    //__________________________________________________________________________
    uint64_t
    bcd_to_dec16(const uint16_t val[4])
    {
//       cout << "#D bcd to dec (16 digits)" << std::endl;
      const std::vector<uint64_t> tmp(val, val+4);

//       uint64_t ret
// 	= ((((tmp[0]<<48U)&0xf000000000000000ULL)>>60U)*1000000000000000ULL)
//         + ((((tmp[0]<<48U)& 0xf00000000000000ULL)>>56U)* 100000000000000ULL)
// 	+ ((((tmp[0]<<48U)&  0xf0000000000000ULL)>>52U)*  10000000000000ULL)
// 	+ ((((tmp[0]<<48U)&   0xf000000000000ULL)>>48U)*   1000000000000ULL)
// 	+ ((((tmp[1]<<32U)&    0xf00000000000ULL)>>44U)*    100000000000ULL)
//         + ((((tmp[1]<<32U)&     0xf0000000000ULL)>>40U)*     10000000000ULL)
// 	+ ((((tmp[1]<<32U)&      0xf000000000ULL)>>36U)*      1000000000ULL)
// 	+ ((((tmp[1]<<32U)&       0xf00000000ULL)>>32U)*       100000000ULL)
// 	+ ((((tmp[2]<<16U)&        0xf0000000ULL)>>28U)*        10000000ULL)
//         + ((((tmp[2]<<16U)&         0xf000000ULL)>>24U)*         1000000ULL)
// 	+ ((((tmp[2]<<16U)&          0xf00000ULL)>>20U)*          100000ULL)
// 	+ ((((tmp[2]<<16U)&           0xf0000ULL)>>16U)*           10000ULL)
// 	+  (((tmp[3]      &            0xf000ULL)>>12U)*            1000ULL)
// 	+  (((tmp[3]      &             0xf00ULL)>> 8U)*             100ULL)
// 	+  (((tmp[3]      &              0xf0ULL)>> 4U)*              10ULL)
//         +    (tmp[3]      &               0xfULL);

      uint64_t t[16];
      t[0]  = ((tmp[0] << 48U) & 0xf000000000000000ULL) >> 60U;
      t[1]  = ((tmp[0] << 48U) &  0xf00000000000000ULL) >> 56U;
      t[2]  = ((tmp[0] << 48U) &   0xf0000000000000ULL) >> 52U;
      t[3]  = ((tmp[0] << 48U) &    0xf000000000000ULL) >> 48U;
      t[4]  = ((tmp[1] << 32U) &     0xf00000000000ULL) >> 44U;
      t[5]  = ((tmp[1] << 32U) &      0xf0000000000ULL) >> 40U;
      t[6]  = ((tmp[1] << 32U) &       0xf000000000ULL) >> 36U;
      t[7]  = ((tmp[1] << 32U) &        0xf00000000ULL) >> 32U;
      t[8]  = ((tmp[2] << 16U) &         0xf0000000ULL) >> 28U;
      t[9]  = ((tmp[2] << 16U) &          0xf000000ULL) >> 24U;
      t[10] = ((tmp[2] << 16U) &           0xf00000ULL) >> 20U;
      t[11] = ((tmp[2] << 16U) &            0xf0000ULL) >> 16U;
      t[12] =  (tmp[3]         &             0xf000ULL) >> 12U;
      t[13] =  (tmp[3]         &              0xf00ULL) >>  8U;
      t[14] =  (tmp[3]         &               0xf0ULL) >>  4U;
      t[15] =   tmp[3]         &                0xfULL;


      uint64_t ret
	= t[0]  * 1000000000000000ULL
	+ t[1]  * 100000000000000ULL
	+ t[2]  * 10000000000000ULL
	+ t[3]  * 1000000000000ULL
	+ t[4]  * 100000000000ULL
	+ t[5]  * 10000000000ULL
	+ t[6]  * 1000000000ULL
	+ t[7]  * 100000000ULL
	+ t[8]  * 10000000ULL
	+ t[9]  * 1000000ULL
	+ t[10] * 100000ULL
	+ t[11] * 10000ULL
	+ t[12] * 1000ULL
	+ t[13] * 100ULL
	+ t[14] * 10ULL
	+ t[15];

      for (int i=0; i<16; ++i)
	if (t[i]>9)
	  throw ret;

//       cout << std::hex
// 	   << val[0] << " " 
// 	   << val[1] << " " 
// 	   << val[2] << " "
// 	   << val[3] << "  -> " 
// 	   << std::dec <<ret
// 	   << std::endl;;
      return ret;
    }
  }

//______________________________________________________________________________
  const defines::unpacker_type VmeRpv100::k_type = "VmeRpv100";

//______________________________________________________________________________
VmeRpv100::VmeRpv100(const unpacker_type& type)
  : VmeModule(type)
{
}

//______________________________________________________________________________
VmeRpv100::~VmeRpv100()
{
}

//______________________________________________________________________________
void
VmeRpv100::decode()
{
  iterator_list::const_iterator f_begin = m_first_list.begin();
  iterator_list::const_iterator f_end   = m_first_list.end();

  for (iterator_list::const_iterator f = f_begin; f!=f_end; ++f)
    {
      iterator first = *f;

      m_vme_header = reinterpret_cast<VmeModule::Header*>(&*first);
      m_module_data_first = first + VmeModule::k_header_size;
      
      register_type* r
	= reinterpret_cast<register_type*>(&*m_module_data_first);
      if (!r)
	cerr << "\n#E " << m_type << "::decode() " << m_name << "\n" 
	     << " got null pointer" << std::endl;
      
      for (unsigned int ch=0; ch<k_n_ch; ++ch)
	{
	  uint16_t* etmp  = r->m_data[ch];
	  uint32_t each_data = 0U; 
// 	  try
// 	    {
	      each_data = bcd_to_dec8(etmp);
// 	    }
// 	  catch (uint32_t e)
// 	    {
// 	      cerr << "\n#E " << m_type << "::decode(each) " << m_name
// 		   << " a value > 9 exists in BCD format " << std::endl;
// 	      each_data = e;
// 	    }
	  fill(ch, k_each, each_data);
	  uint16_t* ptmp = 0;
	  if (ch%2==1)
	    {
	      uint64_t pair_data = 0ULL;
	      try
		{
		  ptmp      = r->m_data[ch-1];
		  pair_data = bcd_to_dec16(ptmp);
		}		  
	      catch (uint64_t e)
		{
// 		  cerr << "\n#E " << m_type << "::decode() " << m_name
// 		       << " a value > 9 exists in BCD format : " 
// 		       << std::hex << e << std::dec
// 		       << std::endl;
// 		  pair_data = 0xffffffffffffffffULL;
		  m_error_state.set(defines::k_bad_bit);
		}
	      fill(ch, k_pair, pair_data);
	    }
	}
    }
  return;
}

//______________________________________________________________________________
const std::map<std::string, int>&
VmeRpv100::get_data_id() const
{
  static std::map<std::string, int> s_data_ref;
  if(s_data_ref.empty())
    {
      s_data_ref["each"] = k_each;
      s_data_ref["pair"] = k_pair;
    }
  return s_data_ref;
}

//______________________________________________________________________________
void
VmeRpv100::resize_fe_data()
{
  m_fe_data.resize(k_n_ch, FECh(k_n_data_type));
  return;
}

}
}
