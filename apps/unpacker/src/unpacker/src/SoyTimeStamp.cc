// -*- C++ -*-

#include "SoyTimeStamp.hh"

namespace hddaq
{
namespace unpacker
{

  const defines::unpacker_type SoyTimeStamp::k_type = "SoyTimeStamp";

//______________________________________________________________________________
SoyTimeStamp::SoyTimeStamp(const unpacker_type& type)
  : DAQNode(type),
    m_data(0)
{
}

//______________________________________________________________________________
SoyTimeStamp::~SoyTimeStamp()
{
}

//______________________________________________________________________________
void
SoyTimeStamp::decode()
{
  fill(0, k_data_type_counter, 
       m_data->m_clock & k_data_counter_mask);
  fill(0, k_data_type_slip,
       (m_data->m_clock & k_data_slip_bit)>>k_data_slip_shift);
//   fill(0, k_data_type_n_pos_edge,
//        m_data->m_n_pos_edge);
//   fill(0, k_data_type_n_neg_edge,
//        m_data->m_n_neg_edge);
  fill(0, k_data_type_pos_edge,
       (m_data->m_edge & k_data_pos_edge_mask)>>k_data_pos_edge_shift);
  fill(0, k_data_type_neg_edge,
       (m_data->m_edge & k_data_neg_edge_mask)>>k_data_neg_edge_shift);
  return;
}

//______________________________________________________________________________
void
SoyTimeStamp::resize_fe_data()
{
  m_fe_data.resize(1);
  m_fe_data[0].resize(k_n_data_type);
  return;
}

//______________________________________________________________________________
bool
SoyTimeStamp::unpack()
{
  m_node_header = reinterpret_cast<DAQNode::Header*>(&(*m_data_first));
  m_data 
    = reinterpret_cast<data_t*>(&(*(m_data_first+DAQNode::k_header_size))); 
  return true;
}

}
}
