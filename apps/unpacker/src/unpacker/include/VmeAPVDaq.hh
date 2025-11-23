// -*- C++ -*-

// Author: Shuhei Hayakawa

#ifndef HDDAQ__VME_APVDAQ_UNPCKER_H
#define HDDAQ__VME_APVDAQ_UNPCKER_H

#include "Uncopyable.hh"
#include "VmeModule.hh"

namespace hddaq
{
namespace unpacker
{

  class VmeAPVDaq
    : public VmeModule,
      private Uncopyable<VmeAPVDaq>
  {

  public:
    static const unpacker_type k_type;
    static const unsigned int  k_n_channel = 512;
    enum data_type { k_adc, k_flag, k_n_data_type };
    // 2015.10.01 VmeAPVDaq FPGAv1.7
    // 1 module has 4 chips and 3 words in each chip
    // the number of sampling is 8.
    static const unsigned int nSample = 8;
    enum chip_type { ChipA, ChipB, ChipC, ChipD, nChip };
    enum word_num  { WordModuleId, WordDataSize, WordDataBody };
    enum word_type { Word1, Word2, Word3, nWord };
    
  private:
    int module_id;
    unsigned int data_size_shift[nChip];
    unsigned int data_size_mask[nChip];
    unsigned int data_shift[nSample];
    unsigned int data_mask[nSample];

    static const unsigned int chip_id_shift = 30;
    static const unsigned int chip_id_mask  = 0x3;
    static const unsigned int channel_shift = 20;
    static const unsigned int channel_mask  = 0x7f;
    static const unsigned int suppression_flag_shift = 30;
    static const unsigned int suppression_flag_mask  = 0x1;
    
  public:
    VmeAPVDaq(const unpacker_type& type);
    virtual ~VmeAPVDaq();

  protected:
    virtual void check_data_format();
    virtual void decode();
    virtual void resize_fe_data();
  };
}
}
#endif
