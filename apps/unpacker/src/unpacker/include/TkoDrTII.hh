// -*- C++ -*-

// Author: Tomonori Takahashi

// TKO backplane
//==========================================
//   MA
//   0 -------------------------------24
//
//      |-|
//      | |
//      | |
//      | |
//      | |   CH 31
//      | |   CH 30
//      | |     :
//      | |   CH 17
//      | |   CH 16
//      |-|<| GND
//
//
//
//
//      |-|
//      | |
//      | |
//      | |
//      | |   CH 15 
//      | |   CH 14 
//      | |     :
//      | |   CH  1
//      | |   CH  0
//      |-|<| GND
//
//==========================================


#ifndef HDDAQ__TKO_DOCTOR_T_II_UNPACKER_H
#define HDDAQ__TKO_DOCTOR_T_II_UNPACKER_H

#include "Tko.hh"

namespace hddaq
{
  namespace unpacker
  {

    class TkoDrTII
      : public Tko,
      private Uncopyable<TkoDrTII>
    {
    public:
      static const unpacker_type k_type;

      static const unsigned int k_CH_mask   = 0xf800U;
      static const unsigned int k_CH_shift  = 11U;
      static const unsigned int k_DATA_mask = 0x7ffU;


      TkoDrTII(const unpacker_type& type);
      virtual ~TkoDrTII();

    protected:
      virtual void decode();
//       virtual void set_data(unsigned int data);

    };

  }
}
#endif
