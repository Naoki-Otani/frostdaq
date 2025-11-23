// -*- C++ -*-

// Author: Tomonori Takahashi

#include "Tko.hh"

#include "std_ostream.hh"
#include "lexical_cast.hh"
#include "defines.hh"
#include "Unpacker.hh"

namespace hddaq
{
  namespace unpacker
  {

    const defines::unpacker_type Tko::k_type = "Tko";

//______________________________________________________________________________
Tko::Tko(const unpacker_type& type)
  : UnpackerImpl(type)
{
  m_fe_info.resize(k_size_TkoInfo);
}      

//______________________________________________________________________________
Tko::~Tko()
{
}      

//______________________________________________________________________________
void
Tko::decode()
{
//   cout << "#D Tko::decode() " << m_name << " " << std::endl;
  const_iterator first = m_data_first;
  const_iterator last  = m_data_last;

//   Unpacker::dump_data(*this);

  for (const_iterator i = first; i!=last; ++i)
    {
      unsigned int data = *i;
      unsigned int sa    = (data & k_SMP_SA_mask) >> k_SMP_SA_shift;
      unsigned int value = data & k_SMP_DATA_mask;
      fill(sa, 0, value);
      
      ++m_fe_info[k_n_decoded];
    }
  return;
}

//______________________________________________________________________________
unsigned int
Tko::get_fe_info(int data_type) const
{
  if(data_type >= k_size_TkoInfo){
    cerr << "\n#E Tko::get_fe_info invaid data_type: "
	 << data_type << std::endl;
    return 0;
  }

  return m_fe_info[data_type];
}

//______________________________________________________________________________
void
Tko::resize_fe_data()
{
  m_fe_data.resize(k_tko_n_ch);
  for (unsigned int i=0; i<k_tko_n_ch; ++i)
    m_fe_data[i].resize(1);
  return;
}

//______________________________________________________________________________
// void
// Tko::set_data(unsigned int data)
// {
//   if (m_is_decode_mode)
//     {
//       unsigned int sa    = (data & k_SMP_SA_mask) >> k_SMP_SA_shift;
//       unsigned int value = data & k_SMP_DATA_mask;
//       fill(sa, 0, value);
// //       cout << "#D Tko::set_data()"
// // 	   << "fill " << sa << " " << value << std::endl;
//     }
//   ++m_word_count;
//   return;
// }

  }
}
