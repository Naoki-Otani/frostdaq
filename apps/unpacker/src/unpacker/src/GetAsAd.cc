// -*- C++ -*-

// Author: Shuhei Hayakawa

#include "GetAsAd.hh"

#include <algorithm>

#include "std_ostream.hh"
#include "Unpacker.hh"
#include "UnpackerManager.hh"

namespace hddaq
{
namespace unpacker
{

const defines::unpacker_type GetAsAd::k_type = "GetAsAd";

//_____________________________________________________________________________
GetAsAd::GetAsAd(const unpacker_type& type)
  : UnpackerImpl(type),
    m_header(nullptr),
    m_module_data_first(),
    m_frame_size(),
    m_frame_type(),
    m_revision(),
    m_header_size(),
    m_item_size(),
    m_n_data(),
    m_event_time_high(),
    m_event_time_low(),
    m_event_id(),
    m_cobo_id(),
    m_asad_id(),
    m_n_padded(),
    m_event_offset(0xffffffff),
    m_prev_run_number(0)
{
  // cout << "created" << std::endl;
  Tag& orig = m_tag[k_tag_origin].back();
  orig.m_local = k_local_tag_origin;
  Tag& max = m_tag[k_tag_max].back();
  max.m_local = k_local_tag_max;
  max.m_event = 0;
  max.m_spill = 0;
}

//_____________________________________________________________________________
GetAsAd::~GetAsAd()
{
}

//_____________________________________________________________________________
void
GetAsAd::check_data_format()
{
  // check header
  if(!m_header){
    cerr << "\n#E " << m_type << " " << m_name << " does not have "
         << " AsAd header \n"  << std::endl;
    m_error_state.set(defines::k_header_bit);
  }else if(m_header->m_magic != k_ASAD_HEADER_MAGIC ||
           m_header->Revision != kRevision){
    print_header();
    m_error_state.set(defines::k_header_bit);
  }

  // data size
  m_word_count = std::distance(m_module_data_first, m_data_last);

  return;
}

//_____________________________________________________________________________
void
GetAsAd::decode()
{
  fill(0, k_tdc_high, m_event_time_high);
  fill(0, k_tdc_low, m_event_time_low);

  const uint32_t* buf = reinterpret_cast<uint32_t*>(&*m_module_data_first);

  std::map<uint32_t, uint32_t> filled_time_bucket;

  switch(m_frame_type){
  case kPartialRead: {
    for(uint32_t i=0, n=m_n_data; i<n; ++i){
      uint32_t aget_id     = (buf[i] >> AGetShift) & AGetMask;
      uint32_t adc_high    = (buf[i] >> AdcHighShiftPartial) & AdcHighMask;
      uint32_t adc_low     = (buf[i] >> AdcLowShiftPartial) & AdcLowMask;
      uint32_t adc         = (adc_high << (AdcLowShiftPartial -
                                           AdcHighShiftPartial)) | adc_low;
      uint32_t channel_high = (buf[i] >> ChannelHighShift) & ChannelHighMask;
      uint32_t channel_low  = (buf[i] >> ChannelLowShift) & ChannelLowMask;
      uint32_t channel      = (channel_high << ChannelLowMask) | channel_low;
      uint32_t fe_channel = channel + aget_id*NumOfChannelAGET;
#if 0
      uint32_t time_bucket_high = ((buf[i] >> TimeBucketHighShift)
                                   & TimeBucketHighMask);
      uint32_t time_bucket_low  = ((buf[i] >> TimeBucketLowShift)
                                   & TimeBucketLowMask);
      uint32_t time_bucket = ((time_bucket_high << TimeBucketHighOffset)
                              | time_bucket_low);
#endif
#if 0
      cout << "#D GetAsAd::decode() aget_id=" << aget_id << ", "
           << "channel=" << channel << ", "
           << "fe_channel=" << fe_channel << ", "
           << "time_bucket=" << time_bucket << ", "
           << "adc=" << adc << std::endl;
#endif

      if(fe_channel >= k_n_channel){
        cerr << "#W GetAsAd::decode() found invalid channel: "
             << channel
             << ", aget_id=" << aget_id
             << std::endl;
      }
#if 0
      {
        FrontEndData& fe_data = *m_back;
        Data* data = fe_data.at(fe_channel).at(k_adc);
        if(data && (data->size() != time_bucket)){
          cerr << "#W GetAsAd::decode() found invalid time bucket: "
               <<"(fe_channel,time)=("
               << fe_channel << ", " << time_bucket << ")"
               << " size=" << data->size()
               << std::endl;
        }
      }
#endif
      // filled_time_bucket[fe_channel]++;
      fill(fe_channel, k_adc, adc);
    }
    break;
  }
  case kFullRead: {
    cerr << "#W GetAsAd::decode() unsupport FullRead mode -> skip decoding"
         << std::endl;
    break;
  }
  default:
    cerr << "#W GetAsAd::decode() got unknown frame type: " << m_frame_type
         << " -> skip decoding"
         << std::endl;
    break;
  }
}

//_____________________________________________________________________________
void
GetAsAd::decode_header()
{
  m_frame_size = ((m_header->FrameSize[0] << 16) |
                  (m_header->FrameSize[1] <<  8) |
                  (m_header->FrameSize[2] <<  0));
  m_frame_type = m_header->FrameType[1];
  m_revision = m_header->Revision;
  m_header_size = ((m_header->HeaderSize[0] << 8) |
                   (m_header->HeaderSize[1] << 0));
  m_item_size = ((m_header->ItemSize[0] << 8) |
                 (m_header->ItemSize[1] << 0));
  m_n_data = ((m_header->nItems[0] << 24) |
              (m_header->nItems[1] << 16) |
              (m_header->nItems[2] <<  8) |
              (m_header->nItems[3] <<  0));
  m_event_time_high = ((m_header->EventTime[0] <<  8) |
                       (m_header->EventTime[1] <<  0));
  m_event_time_low = ((m_header->EventTime[2] << 24) |
                      (m_header->EventTime[3] << 16) |
                      (m_header->EventTime[4] <<  8) |
                      (m_header->EventTime[5] <<  0));
  m_event_id = ((m_header->EventIdx[0] << 24) |
                (m_header->EventIdx[1] << 16) |
                (m_header->EventIdx[2] <<  8) |
                (m_header->EventIdx[3] <<  0));
  m_cobo_id = m_header->CoboIdx;
  m_asad_id = m_header->AsadIdx;
  m_n_padded = (m_frame_size - m_header_size)*256 - m_item_size*m_n_data;

  const UnpackerManager& gUnpacker = GUnpacker::get_instance();

  uint32_t root_run_number = gUnpacker.get_root()->get_run_number();
  if(m_event_offset == 0xffffffff ||
     m_prev_run_number != root_run_number){
    m_event_offset = m_event_id - gUnpacker.get_event_number();
    m_prev_run_number = root_run_number;
  }
}

//_____________________________________________________________________________
void
GetAsAd::print_header() const
{
  if(!m_header){
    cout << "\n#E GetAsAd::print_asad_header() : got "
         << m_name << " null pointer to header"
         << std::endl;
  }else{
    cout << "#D GetAsAd::print_header() " << m_name << std::endl
         << std::showbase << std::hex
         << " magic           = " << m_header->m_magic << std::endl
         << " data size       = " << m_header->m_data_size << std::endl
         << " frame size      = " << m_frame_size << std::endl
         << " frame type      = " << m_frame_type << std::endl
         << " revision        = " << m_revision << std::endl
         << " header size     = " << m_header_size << std::endl
         << " item size       = " << m_item_size << std::endl
         << " n data          = " << m_n_data << std::endl
         << " event time high = " << m_event_time_high << std::endl
         << " event time low  = " << m_event_time_low << std::endl
         << " event id        = " << m_event_id << std::endl
         << " cobo id         = " << m_cobo_id << std::endl
         << " asad id         = " << m_asad_id << std::endl
         << " n padded        = " << m_n_padded << std::endl
         << " event offset    = " << m_event_offset << std::endl
         << std::dec << std::noshowbase << std::endl;
  }
  return;
}

//______________________________________________________________________________
void
GetAsAd::resize_fe_data()
{
  m_fe_data.resize(k_n_channel);
  for(uint32_t i=0; i<k_n_channel; ++i){
    m_fe_data[i].resize(k_n_data_type);
  }
  return;
}

//_____________________________________________________________________________
bool
GetAsAd::unpack()
{
  m_header = reinterpret_cast<Header*>(&(*m_data_first));
  m_module_data_first = m_data_first + k_header_size;
  decode_header();
  return true;
}

//_____________________________________________________________________________
void
GetAsAd::update_tag()
{
  Tag& tag = m_tag[k_tag_current].back();
  tag.m_local = m_event_id - m_event_offset;
  m_has_tag.set(k_local);
}

}
}
