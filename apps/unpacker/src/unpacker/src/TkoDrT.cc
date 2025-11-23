// -*- C++ -*-

// Author: Tomonori Takahashi

#include "TkoDrT.hh"

#include "std_ostream.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type TkoDrT::k_type = "TkoDrT";


//______________________________________________________________________________
TkoDrT::TkoDrT(const unpacker_type& type)
  : Tko(type)
{
}

//______________________________________________________________________________
TkoDrT::~TkoDrT()
{
}

//______________________________________________________________________________
void
TkoDrT::decode()
{
  const_iterator first = m_data_first;
  const_iterator last  = m_data_last;
  for (const_iterator i=first; i!=last; ++i)
    {
      unsigned int ch    = (((*i) & k_CH_mask) >> k_CH_shift) ^ 0xfU;
      unsigned int value = ((*i)& k_DATA_mask);
//       hoge();
//       cout << " ch = " << ch << ", val = " << value << std::endl;
      fill(ch, 0,value);
    }
  return;
}

//______________________________________________________________________________
// void
// TkoDrT::set_data(unsigned int data)
// {
//   if (m_is_decode_mode)
//     {
// //       unsigned int sa    = (data & k_SMP_SA_mask) >> k_SMP_SA_shift;
//       unsigned int ch    = (data & k_CH_mask) >> k_CH_shift;
//       unsigned int value = (data & k_DATA_mask);
//       fill(ch, 0, value);
//     }

//   return;
// }

}
}
