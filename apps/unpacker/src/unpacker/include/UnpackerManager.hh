// -*- C++ -*-

// Author: Tomonori Takahashi

#ifndef HDDAQ__UNPACKER_MANAGER_H
#define HDDAQ__UNPACKER_MANAGER_H

#include <stdint.h>
#include <map>
#include <vector>
#include <string>

#include "Singleton.hh"
#include "Uncopyable.hh"
#include "defines.hh"

#include "RingBuffer.hh"


namespace hddaq
{
namespace unpacker
{

class EventReader;
class IStream;

class Unpacker;


class UnpackerManager
  : private Uncopyable<UnpackerManager>
{

public:
  typedef defines::unpacker_type          unpacker_type;
  typedef defines::check_mode_t           check_mode_t;
  typedef defines::dump_mode_t            dump_mode_t;
  typedef std::vector<uint32_t>::iterator iterator;


  struct EventBuffer
  {
    DigitList                 m_digit;
    std::vector<FrontEndData> m_fe;
  };

  typedef RingBuffer<EventBuffer> fifo_t;
  typedef fifo_t::iterator        fifo_iterator;
  typedef fifo_t::const_iterator  const_fifo_iterator;


private:
  friend class Singleton<UnpackerManager>;
  EventReader*           m_reader;
  std::string            m_input_stream;
  std::vector<Unpacker*> m_unpacker;
  Unpacker*              m_root;
  bool                   m_is_good;
  bool                   m_is_esc_on;
  int                    m_skip;
  int                    m_max_loop;
  int                    m_print_cycle;
  DigitList              m_digit_list;
  fifo_t                 m_fifo;
  DigitList*             m_front;
  bool                   m_is_data_ready;
  int                    m_counter;
  bool                   m_decode_mode;
  dump_mode_t            m_dump_mode;
  int                    m_run_number;
  bool                   m_enable_istream_bookmark;

public:
  ~UnpackerManager();

  void            hoge(const std::string& arg="") const;
  void            add_entry(Unpacker* entry);
  void            check();
  void            clear();
  void            close();
  Unpacker*       create(const unpacker_type& type);
  void            decode();
  void            disable_istream_bookmark();
  void            dump_data() const; // all data are displayed
  void            dump_data_device(int device_id,
                                   int plane_id=-1) const;
  void            dump_data_fe(int node_id,
                               int address=-1,
                               int ma=-1) const;
  bool            empty() const;
  void            enable_istream_bookmark();
  bool            eof() const;

  const Device&   find(const char* name) const;       // char*
  const Device&   find(const std::string& name) const;// std::string
  const Device&   find(unsigned int device_id) const; // unsigned int
  const Plane&    find(unsigned int device_id,
                       unsigned int plane_id) const;
  const Plane&    find(const char* device_name,       // char*
                       const char* plane_name) const;
  const Plane&    find(const std::string& device_name,// std::string
                       const std::string& plane_name) const;
  unsigned long long get(unsigned int device_id,
                         unsigned int plane_id,
                         unsigned int segment_id,
                         unsigned int ch,
                         unsigned int data_type=0,
                         unsigned int hit_id=0) const;
  unsigned long long get(const char* device_name,
                         unsigned int plane_id,
                         unsigned int segment_id,
                         unsigned int ch,
                         unsigned int data_type=0,
                         unsigned int hit_id=0) const;
  unsigned long long get(const std::string& device_name,
                         unsigned int plane_id,
                         unsigned int segment_id,
                         unsigned int ch,
                         unsigned int data_type=0,
                         unsigned int hit_id=0) const;
  unsigned long long get(const char* device_name,
                         const char* plane_name,
                         unsigned int segment_id,
                         unsigned int ch,
                         unsigned int data_type=0,
                         unsigned int hit_id=0) const;
  unsigned long long get(const std::string& device_name,
                         const std::string& plane_name,
                         unsigned int segment_id,
                         unsigned int ch,
                         unsigned int data_type=0,
                         unsigned int hit_id=0) const;


  void            get_buffer(const std::vector<uint64_t>& fe_id,
                             iterator& begin,
                             iterator& end) const;
  iterator        get_buffer_begin(const std::vector<uint64_t>& fe_id) const;
  iterator        get_buffer_end(const std::vector<uint64_t>& fe_id) const;
  unsigned int    get_counter() const;
  unsigned int    get_daq_root_number() const;
  int             get_fe_id(const char* fe_name) const;
  int             get_fe_id(const std::string& fe_name) const;
  unsigned int    get_fe_info(int node_id,
                              int address = -1,
                              int ma      = -1,
                              int data_type = 0
    ) const;
  int             get_device_id(const char* device_name) const;
  int             get_device_id(const std::string& device_name) const;
  int             get_plane_id(const char* device_name,
                               const char* plane_name ) const;;
  int             get_plane_id(const std::string& device_name,
                               const std::string& plane_name ) const;
  int             get_data_id(const char* device_name,
                              const char* data_name ) const;
  int             get_data_id(const std::string& device_name,
                              const std::string& data_name ) const;
  unsigned int    get_entries(unsigned int device_id,
                              unsigned int plane_id,
                              unsigned int segment_id,
                              unsigned int ch,
                              unsigned int data_type=0) const;
  unsigned int    get_entries(const std::string& name,
                              unsigned int plane_id,
                              unsigned int segment_id,
                              unsigned int ch,
                              unsigned int data_type=0) const;
  unsigned int  get_n_device() const;
  unsigned int  get_n_plane(int device_id) const;
  unsigned int  get_n_segment(int device_id,
                              int plane_id) const;
  unsigned int  get_n_ch(int device_id,
                         int plane_id,
                         int segment_id) const;
  unsigned int  get_n_data(int device_id,
                           int plane_id,
                           int segment_id,
                           int ch_id) const;
  uint32_t        get_event_number() const;
  fifo_t::size_type get_fifo_entries() const;
  fifo_t::size_type get_fifo_length() const;
  const std::string& get_istream() const;
  uint64_t        get_istream_bookmark() const;
  int             get_max_loop() const;
  unsigned int    get_node_header(int node_id, int header_data_type) const;
  Unpacker*       get_root() const;
  uint32_t        get_run_number() const;
  int             get_skip() const;
  void            initialize();
  bool            is_data_ready();
  bool            is_decode_mode() const;
  bool            is_good() const;
  bool            is_online() const;
  void            operator++();
  void            pop_event();
  void            print() const;
  void            print_tag() const;
  void            push_event();
  void            read();
  void            reset();
  void            set_config_file(const std::string& config_file,
                                  const std::string& digit_file="",
                                  const std::string& channel_map_file="");
  void            set_decode_mode(bool flag);
  void            set_dump_mode(unsigned int dump_mode);
  void            set_istream(const std::string& name);
  void            set_istream_bookmark(uint64_t bookmark);
  void            set_parameter(const std::string& name,
                                const std::string& value);
  void            show_event_number() const;
  void            show_summary(bool is_forced=false);

private:
  UnpackerManager();
  void            search_device(Unpacker* u,
                                std::vector<Unpacker*>& uvect,
                                int device_id, int plane_id
    ) const;
  Unpacker*       search_fe(Unpacker* parent, int id) const;

};

//______________________________________________________________________________
typedef Singleton<UnpackerManager> GUnpacker;


}
}
#endif
