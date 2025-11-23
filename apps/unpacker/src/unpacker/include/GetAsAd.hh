// -*- C++ -*-

// Author: Shuhei Hayakawa

#ifndef HDDAQ__GET_ASAD_UNPACKER_H
#define HDDAQ__GET_ASAD_UNPACKER_H

#include "UnpackerImpl.hh"

namespace hddaq
{
namespace unpacker
{

//_____________________________________________________________________________
class GetAsAd
  : public UnpackerImpl,
    private Uncopyable<GetAsAd>
{
public:
  static const unpacker_type k_type;
  static const uint32_t NumOfAGET        = 4;
  static const uint32_t NumOfChannelAGET = 68;

  static const uint32_t k_n_channel = NumOfAGET*NumOfChannelAGET;
  enum EDataType { k_adc, k_tdc_high, k_tdc_low, k_n_data_type };

  static const uint32_t k_local_tag_origin = 0;
  static const uint32_t k_local_tag_max    = 0xffffffff;

  static const uint32_t k_ASAD_HEADER_MAGIC = 0x41734164; // "AsAd"

  static const uint32_t kRevision            = 5;
  static const uint32_t AGetShift            = 6;
  static const uint32_t AGetMask             = 0x3;
  static const uint32_t AdcHighShiftFull     = 0;
  static const uint32_t AdcHighShiftPartial  = 16;
  static const uint32_t AdcHighMask          = 0xf;
  static const uint32_t AdcLowShiftFull      = 8;
  static const uint32_t AdcLowShiftPartial   = 24;
  static const uint32_t AdcLowMask           = 0xff;
  static const uint32_t ChannelHighShift     = 0;
  static const uint32_t ChannelHighMask      = 0x3f;
  static const uint32_t ChannelLowShift      = 15;
  static const uint32_t ChannelLowMask       = 0x1;
  static const uint32_t TimeBucketHighShift  = 8;
  static const uint32_t TimeBucketHighMask   = 0x7f;
  static const uint32_t TimeBucketLowShift   = 22;
  static const uint32_t TimeBucketLowMask    = 0x3;
  static const uint32_t TimeBucketHighOffset = 2;

  struct Header
  {
    uint32_t m_magic;
    uint32_t m_data_size;
    // unsigned char: 8 bit
    uint8_t MetaType;          //1
    uint8_t FrameSize[3];      //4
    uint8_t DataSource;        //5
    uint8_t FrameType[2];      //7
    uint8_t Revision;          //8
    uint8_t HeaderSize[2];     //10
    uint8_t ItemSize[2];       //12
    uint8_t nItems[4];         //16
    uint8_t EventTime[6];      //22
    uint8_t EventIdx[4];       //26
    uint8_t CoboIdx;           //27
    uint8_t AsadIdx;           //28
    uint8_t ReadOffset[2];     //30
    uint8_t Status;            //31
    uint8_t HitPatAGet0[9];    //40
    uint8_t HitPatAGet1[9];    //49
    uint8_t HitPatAGet2[9];    //58
    uint8_t HitPatAGet3[9];    //67
    uint8_t MultiPlAGet0[2];   //69
    uint8_t MultiPlAGet1[2];   //71
    uint8_t MultiPlAGet2[2];   //73
    uint8_t MultiPlAGet3[2];   //75
    uint8_t WindowOut[4];      //79
    uint8_t LastCellAGet0[2];  //81
    uint8_t LastCellAGet1[2];  //83
    uint8_t LastCellAGet2[2];  //85
    uint8_t LastCellAGet3[2];  //87
    uint8_t reserve1byte;      //88
    uint8_t reserve4byte[168]; //256 <---(42*4=168)
  };
  enum EFrameType { kPartialRead = 1, kFullRead = 2 };
  static const uint32_t k_header_size = sizeof(Header)/sizeof(uint32_t);

protected:
  Header*  m_header;
  iterator m_module_data_first;
  uint32_t m_frame_size;
  uint32_t m_frame_type;
  uint32_t m_revision;
  uint32_t m_header_size;
  uint32_t m_item_size;
  uint32_t m_n_data;
  uint32_t m_event_time_high;
  uint32_t m_event_time_low;
  uint32_t m_event_id;
  uint32_t m_cobo_id;
  uint32_t m_asad_id;
  uint32_t m_n_padded;
  uint32_t m_event_offset;
  uint32_t m_prev_run_number;

public:
  GetAsAd(const unpacker_type& type);
  virtual ~GetAsAd();

protected:
  virtual void check_data_format();
  virtual void decode();
  virtual void decode_header();
  virtual void print_header() const;
  virtual void resize_fe_data();
  virtual void update_tag();
  virtual bool unpack();

};

}
}

#endif
