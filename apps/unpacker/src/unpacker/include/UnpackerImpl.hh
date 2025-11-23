// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__UNPACKER_IMPL_H
#define HDDAQ__UNPACKER_IMPL_H

#include <stdint.h>
#include <string>
#include <vector>
#include <deque>
#include <map>

#include "defines.hh"
#include "Uncopyable.hh"
#include "Tag.hh"

#include "RingBuffer.hh"

namespace hddaq
{

  namespace unpacker
  {

    class Unpacker;

  class UnpackerImpl 
    : private Uncopyable<UnpackerImpl>
  {
    
  public:
    typedef std::vector<uint32_t>::iterator       iterator;
    typedef std::vector<uint32_t>::const_iterator const_iterator;
    typedef std::vector<iterator>                 iterator_list;
    typedef defines::unpacker_type                unpacker_type;
    typedef defines::error_state_t                error_state_t;
    typedef defines::check_mode_t                 check_mode_t;
    typedef defines::dump_mode_t                  dump_mode_t;
    typedef std::vector<std::deque<bool> >        NullChList;
    typedef std::map<uint64_t, Unpacker*>         ChildList;

    struct DigitId
    {
      int m_device;
      int m_plane;
      int m_segment;
      int m_ch;
      int m_data_type;
    };
    typedef std::vector<std::vector<DigitId> > DigitIdList;

  protected:
    unpacker_type  m_type;
    uint64_t       m_id;
    std::string    m_name;
    Unpacker*      m_parent;
    ChildList      m_child_list;
    unsigned int   m_data_size;
    unsigned int   m_word_count;
    iterator       m_data_first;
    iterator       m_data_last;
    iterator_list  m_first_list;
    iterator_list  m_last_list;
    DigitIdList    m_digit_id_list;
    FrontEndData   m_fe_data;       
    FrontEndData*  m_back;
    NullChList     m_null_ch;
    
    std::vector<std::vector<Tag> > m_tag;

    error_state_t        m_error_state;
    std::bitset<k_n_tag> m_has_tag;
    std::bitset<k_n_tag_status> m_tag_status;
    bool                 m_is_node;
    dump_mode_t          m_dump_mode;
    bool                 m_is_decode_mode;
    bool                 m_is_unpack_mode;
    int                  m_run_number;
    std::vector<unsigned long long int> m_sec;
    std::vector<unsigned long long int> m_nsec;

    std::vector<unsigned int> m_fe_info;

    static bool         gm_is_esc_on;
    static check_mode_t gm_check_mode;
    static std::string  gm_true_bit;
    static std::string  gm_false_bit;
    static int          gm_null_device_id;

  private:
    friend class Unpacker;

  public:
    UnpackerImpl(const unpacker_type& type);
    virtual ~UnpackerImpl() = 0;
    
  public:
    void fill(unsigned int ch,
	      unsigned int data_type,
	      unsigned long long data);

  protected:
    virtual void hoge(const std::string& arg="") const;
    virtual void check_data_format();
    virtual void check_slip();
    virtual void check_mismatch();
    virtual void clear();
    virtual void decode();
    virtual void dump_in_binary() const;
    virtual void dump_in_decimal() const;
    virtual void dump_in_hexadecimal() const;
    virtual Unpacker* get_child(uint64_t id) const;

    virtual const std::map<std::string, int>&
                 get_data_id() const;
    virtual unsigned int 
    get_fe_info(int data_type) const {return 0;};
    virtual unsigned int 
    get_header(int data_type) const {return 0;};
    virtual bool is_ready();
    virtual bool is_related(int device_id, int plane_id=-1);
    virtual void reset();
    virtual void resize_fe_data();
    virtual void set_data(unsigned int data);
    virtual void set_data(const iterator& data_first,
			  const iterator& data_last);
    virtual void set_data_size(unsigned int data_size);
    virtual void sort_child();
    virtual bool unpack();
    virtual void update_tag();

  };


  }
}
#endif
