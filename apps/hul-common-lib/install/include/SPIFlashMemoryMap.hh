#ifndef SPI_FLASH_MEMORY_MAP_HH
#define SPI_FLASH_MEMORY_MAP_HH

#include<stdint.h>
#include<string>
#include<map>
#include<tuple>

namespace SPI{

  // Device common information ___________________________________________
  struct Device
  {
    const uint32_t device_id;   // ManufactureID (1byte) + DevicdID (2byte)
    const uint32_t page_size;   // byte
    const uint32_t sector_size; // kbyte
    const uint32_t max_size;    // byte 
  };

  const Device g_device_s25fl256s = {0x010219, 256, 32, 32000000};
  const Device g_device_s25fl128s = {0x012018, 256, 32, 16000000};
  const Device g_device_s25fl128l = {0x016018, 256, 32, 16000000};
  const Device g_device_n25q128a  = {0x20ba18, 128, 64, 16000000};
  const Device g_device_mt25ql512 = {0x20ba20, 512, 64, 64000000};

  // SPI command _________________________________________________________
  struct Command
  {
    const std::string mode;
    const uint32_t    instruction;
    const uint32_t    length_inst;
    const uint32_t    length_read;
    const uint32_t    length_write;
  };
  const Command g_null_command   = {"null",  0,    0, 0, 0};
  const Command g_dummy_command  = {"inst",  0,    1, 0, 0};
  const Command g_read_device_id = {"read",  0x9f, 1, 3, 0};
 
  using MapCommand = std::map<const std::string, const Command>;
  const MapCommand g_command_s25fl256s = 
    {
     {"read_device_id",                 {"read",  0x9f, 1, 3, 0}},
     {"read_status_register1",          {"read",  0x05, 1, 1, 0}},
     {"read_status_register2",          {"read",  0x07, 1, 1, 0}},
     {"read_configuration_register",    {"read",  0x35, 1, 1, 0}},
     {"bank_register_read",             {"read",  0x16, 1, 1, 0}},
     {"clear_status_register",          {"inst",  0x30, 1, 0, 0}},
     {"reset",                          {"inst",  0xF0, 1, 0, 0}},
     {"bank_register_write",            {"write", 0x17, 1, 0, 1}},
     {"bulk_erase",                     {"inst",  0x60, 1, 0, 0}},
     {"write_register",                 {"write", 0x01, 1, 0, 2}},
     {"write_enable",                   {"inst",  0x06, 1, 0, 0}},
     {"write_disable",                  {"inst",  0x04, 1, 0, 0}},
     {"page_program",                   {"write", 0x12, 5, 0, 0}},
     {"fast_read",                      {"read",  0x0C, 6, 0, 0}},
     {"read",                           {"read",  0x13, 5, 0, 0}}
    };

  const MapCommand g_command_n25q128a = 
    {
     {"read_device_id",                 {"read",  0x9f, 1, 3, 0}},
     {"read_status_register",           {"read",  0x05, 1, 1, 0}},
     {"read_flag_status_register",      {"read",  0x70, 1, 1, 0}},
     {"clear_flag_status_register",     {"inst",  0x50, 1, 0, 0}},
     {"read_volatile_conf_register",    {"read",  0x85, 1, 1, 0}},
     {"write_volatile_conf_register",   {"write", 0x81, 1, 0, 1}},
     {"enable_reset",                   {"inst",  0x66, 1, 0, 0}},
     {"reset",                          {"inst",  0x99, 1, 0, 0}},
     {"bulk_erase",                     {"inst",  0xC7, 1, 0, 0}},
     {"write_enable",                   {"inst",  0x06, 1, 0, 0}},
     {"write_disable",                  {"inst",  0x04, 1, 0, 0}},
     {"page_program",                   {"write", 0x02, 4, 0, 0}},
     {"fast_read",                      {"read",  0x0B, 5, 0, 0}},
     {"read",                           {"read",  0x03, 4, 0, 0}}
    };

  const MapCommand g_command_mt25ql512 = 
    {
     {"read_device_id",                 {"read",  0x9f, 1, 3, 0}},
     {"read_status_register",           {"read",  0x05, 1, 1, 0}},
     {"read_flag_status_register",      {"read",  0x70, 1, 1, 0}},
     {"clear_flag_status_register",     {"inst",  0x50, 1, 0, 0}},
     {"read_volatile_conf_register",    {"read",  0x85, 1, 1, 0}},
     {"write_volatile_conf_register",   {"write", 0x81, 1, 0, 1}},
     {"enter_4byte_address_mode",       {"inst",  0xB7, 1, 0, 0}},
     {"exit_4byte_address_mode",        {"inst",  0xE9, 1, 0, 0}},
     {"enable_reset",                   {"inst",  0x66, 1, 0, 0}},
     {"reset",                          {"inst",  0x99, 1, 0, 0}},
     {"bulk_erase",                     {"inst",  0xC7, 1, 0, 0}},
     {"write_enable",                   {"inst",  0x06, 1, 0, 0}},
     {"write_disable",                  {"inst",  0x04, 1, 0, 0}},
     {"page_program",                   {"write", 0x02, 5, 0, 0}},
     {"fast_read",                      {"read",  0x0B, 6, 0, 0}},
     {"read",                           {"read",  0x03, 5, 0, 0}}
    };
  
  // SPI tuple ___________________________________________________________
  const int kDevice  = 0;
  const int kCommand = 1;
  using TupleSpi = std::tuple<const Device, const MapCommand>;
  using MapSpi   = std::map<const std::string, const TupleSpi>;
  const MapSpi g_spi = 
    {
     {"s25fl256s", std::make_tuple(g_device_s25fl256s, g_command_s25fl256s)},
     {"s25fl128s", std::make_tuple(g_device_s25fl128s, g_command_s25fl256s)},
     {"s25fl128l", std::make_tuple(g_device_s25fl128l, g_command_s25fl256s)},
     {"n25q128a",  std::make_tuple(g_device_n25q128a,  g_command_n25q128a)},
     {"mt25ql512", std::make_tuple(g_device_mt25ql512, g_command_mt25ql512)}
    };

  // Reverse dictionaly __________________________________________________
  using ReverseMap = std::map<uint32_t, std::string>;
  const ReverseMap g_reverse_spi =
    {
     {g_device_s25fl256s.device_id, "s25fl256s"},
     {g_device_s25fl128s.device_id, "s25fl128s"},
     {g_device_s25fl128l.device_id, "s25fl128l"},
     {g_device_n25q128a.device_id,  "n25q128a"},
     {g_device_mt25ql512.device_id, "mt25ql512"}
    };
};
#endif
