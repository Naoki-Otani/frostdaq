// -*- C++ -*-

// Author: Tomonori Takahashi

#include "FinessePtdc.hh"

#include "std_ostream.hh"
#include "std_pair_dump.hh"

namespace hddaq
{
  namespace unpacker
  {

    const defines::unpacker_type FinessePtdc::k_type = "FinessePtdc";

//______________________________________________________________________________
FinessePtdc::FinessePtdc(const unpacker_type& type)
  : Finesse(type)
{
}

//______________________________________________________________________________
FinessePtdc::~FinessePtdc()
{
}

//______________________________________________________________________________
unsigned int
FinessePtdc::calc_check_sum() const
{
  //  return 0U;
  return 0;//(m_trailer->m_value & 0xffffU);
}

//______________________________________________________________________________
void
FinessePtdc::decode()
{
//   cout() << "D FinessePtdc::decode()" << std::endl;
  const_iterator first = m_body_first;
  const_iterator last  = m_body_last;
  for (const_iterator i=first; i!=last; ++i)
    {
      unsigned int ch = ((*i & k_ch_mask) >> k_ch_shift) ^ 0xfU;
      int time = *i & k_data_mask;
      fill(ch, k_leading, time);      
//       const unsigned int empty_flag 
// 	= (*i & k_empty_flag_mask) >> k_empty_flag_shift;
//       if (empty_flag==0)
// 	fill(ch, k_leading, time);
//       else
// 	fill(ch, k_leading_empty, time);
      
//       cout << "#D " << ch << ", " << time << " " << empty_flag
// 	     << "\n";
    }
//   cout.flush();
  
  return;
}


//______________________________________________________________________________
const std::map<std::string, int>&
FinessePtdc::get_data_id() const
{
  static std::map<std::string, int> s_data_ref;
  if(s_data_ref.empty())
    {
      s_data_ref["leading"]       = k_leading;
      //      s_data_ref["leading_empty"] = k_leading_empty;
    }
  return s_data_ref;
}

//______________________________________________________________________________
void
FinessePtdc::resize_fe_data()
{
  m_fe_data.resize(k_n_ch);
  for (unsigned int i=0; i<k_n_ch; ++i)
    m_fe_data[i].resize(k_n_data_type);
  return;
}

}
}
