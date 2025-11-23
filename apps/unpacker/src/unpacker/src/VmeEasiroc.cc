//-----------------------------------------------------------------------------
// VME-EASIROC decoding class for unpacker
//-----------------------------------------------------------------------------

#include"VmeEasiroc.hh"

#include<iterator>
#include <iomanip>
#include <cstdio>

#include"std_ostream.hh"
#include"defines.hh"

namespace hddaq{
    namespace unpacker{
        const defines::unpacker_type VmeEasiroc::k_type = "VmeEasiroc";

        // Constructor ----------------------------------------------------------------
        VmeEasiroc::VmeEasiroc(const unpacker_type& uType)
            :DAQNode(uType),
            m_header(0),
            m_body_first()
        {

        }

        // Destructor -----------------------------------------------------------------
        VmeEasiroc::~VmeEasiroc()
        {

        }

        // decode ---------------------------------------------------------------------
        void VmeEasiroc::decode(){
            const_iterator i = m_body_first;

            int NofEvent = m_header->m_n_word & k_NWORD_MASK;
            for(int current = 0; current<NofEvent; ++current, ++i){
                uint32_t word = decodeWord_(*i);
                if(isHg_(word)) {
                    unsigned int ch = getCh_(word);
                    unsigned int adc = getData_(word);
                    fill(ch, k_adc_high, adc);
                }else if(isLg_(word)) {
                    unsigned int ch = getCh_(word);
                    unsigned int adc = getData_(word);
                    fill(ch, k_adc_low, adc);
                }else if(isTdc_(word)) {
                    unsigned int ch = getCh_(word);
                    unsigned int time = getData_(word);
                    unsigned int ltbit = getLt_(word);
                    if(k_leading == ltbit) {
                        fill(ch, k_leading, time);
                    }else {
                        fill(ch, k_trailing, time);
                    }
                }else if(isScaler_(word)) {
                    unsigned int ch = getScalerCh_(word);
                    unsigned int count = getScalerCount_(word);
                    (void)ch;
                    (void)count;
                }else {
                    std::cout << "VmeEasiroc: Unknown word: " <<
                    std::hex << std::setfill('0') << std::setw(8) << word <<
                    std::endl;
                }

            }
            return;
        }

        // check_data_format ----------------------------------------------------------
        void VmeEasiroc::check_data_format(){
            // header magic word
            if(k_HEADER_MAGIC != m_header->m_vme_easiroc_magic) {
                m_error_state.set(defines::k_header_bit);
                std::cerr << "VmeEasiroc: Invalid header magic" << std::endl;
            }

            // header # of word check
            m_word_count = std::distance(m_body_first, m_data_last);
            if(m_word_count != m_header->m_n_word) {
                m_error_state.set(defines::k_data_size_bit);
                std::cerr << "VmeEasiroc: Number of words is missmatched" << std::endl;
            }

            return;
        }

        // resize_fe_data -------------------------------------------------------------
        void VmeEasiroc::resize_fe_data(){
            m_fe_data.resize(k_n_ch);
            for(unsigned int i = 0; i<k_n_ch; ++i){
                m_fe_data[i].resize(k_n_data_type);
            }

            return;
        }

        // unpack ---------------------------------------------------------------------
        bool VmeEasiroc::unpack(){
            m_node_header = reinterpret_cast<DAQNode::Header*>(&(*m_data_first));
            m_header
                = reinterpret_cast<Header*>(&(*(m_data_first +DAQNode::k_header_size)));

            m_body_first = m_data_first 
                + DAQNode::k_header_size
                + VmeEasiroc::k_header_size;

            return true;
        }
      
      // aka Event Tag      
      void VmeEasiroc::update_tag()
      { 
	iterator_list::const_iterator f_begin = m_first_list.begin();
	iterator_list::const_iterator f_end   = m_first_list.end();
	m_tag[k_tag_current].clear();
	
	for (iterator_list::const_iterator f = f_begin; f!=f_end; ++f)
	  {
	    
	    unsigned int event_count = getEventCounter();	    
	    unsigned int spill_count = getSpillCounter();	    
	    Tag tag;
	    tag.m_event = event_count;
	    tag.m_spill = spill_count;
	    m_tag[k_tag_current].push_back(tag);
	  }
	
	m_has_tag[k_event] = true;
	m_has_tag[k_spill] = true;
	
	return;
      }

      

    }
}
