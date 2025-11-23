#include"SitcpController.hh"
#include"errno.h"
#include"UDPRBCP.hh"
#include"Utility.hh"

#include<iostream>
#include<sstream>
#include<cstdio>
#include<csignal>
#include<string>
#include<unistd.h>

namespace RBCP{

  namespace{
    static constexpr uint32_t kAddrBase        = 0xffffff00;
    static constexpr uint32_t kAddrCtrlSitcp   = 0x10;
    static constexpr uint8_t  kCmdSitcpReset   = 0x80;
  
    static constexpr uint8_t  kCmdKeepAlive    = 0x04;
    static constexpr uint8_t  kCmdFastRetrains = 0x02;
    static constexpr uint8_t  kCmdNagleBuffer  = 0x01;

   }

// Reset SiTCP -------------------------------------------------------------
void
ResetSiTCP(const std::string ip)
{
  static const std::string func_name {std::string("::") + __func__ + "()"};
  
  UDPRBCP udp_man(ip, gUdpPort);
  const uint32_t addr   = kAddrBase + kAddrCtrlSitcp;
  const uint8_t  wd_def = kCmdNagleBuffer; // default;
  const uint8_t  wd     = wd_def + kCmdSitcpReset;

  Utility::PrintNormal(func_name, "Trying Reset SiTCP");

  udp_man.SetWD(addr, 1, &wd);
  udp_man.DoRBCP();

  udp_man.SetWD(addr, 1, &wd_def);
  udp_man.DoRBCP();
}

// Write SiTCP -------------------------------------------------------------
void
WriteSiTCP(const std::string ip, const uint32_t addr_ofs, const uint32_t reg)
{
  static const std::string func_name {std::string("::") + __func__ + "()"};
  
  UDPRBCP udp_man(ip, gUdpPort);
  const uint32_t addr     = kAddrBase + addr_ofs;
  const uint8_t  reg_8bit = static_cast<const uint8_t>(reg & 0xff);

  std::ostringstream message;
  message << std::hex << "Trying Write Register (0x" << reg << ") to Address (0x" << addr << ")" << std::dec;
  Utility::PrintNormal(func_name, message.str());

  udp_man.SetWD(addr, 1, &reg_8bit);
  udp_man.DoRBCP();
}

// Read SiTCP  -------------------------------------------------------------
void
ReadSiTCP(const std::string ip, const uint32_t addr_ofs)
{
  static const std::string func_name {std::string("::") + __func__ + "()"};
  
  UDPRBCP udp_man(ip, gUdpPort);
  const uint32_t addr   = kAddrBase + addr_ofs;

  std::ostringstream message;
  message << std::hex << "#D : Trying Read Register from Address (" << addr << ")" << std::dec;
  Utility::PrintNormal(func_name, message.str());

  udp_man.SetRD(addr, 1);
  udp_man.DoRBCP();
}

// Erase EEPROM ------------------------------------------------------------
void
EraseEEPROM(const std::string ip)
{
  static const std::string func_name {std::string("::") + __func__ + "()"};

  Utility::PrintNormal(func_name, "Trying Erase EEPROM");

  UDPRBCP udp_man(ip, gUdpPort, DebugMode::kNoDisp);
  uint32_t addr = 0xfffffcff;
  uint8_t wd    = 0x0;
  udp_man.SetWD(addr, 1, &wd);
  udp_man.DoRBCP();

  // fill 128byte from 0xfffffc00 with 0xff 
  addr = 0xfffffc00;
  wd   = 0xff;
  for(uint32_t i = 0; i<128; ++i){
    //    std::cout << "." << std::flush;
    Utility::FlashMessage(" Erasing EEPROM");
    udp_man.SetWD(addr+i, 1, &wd);
    udp_man.DoRBCP();
    usleep(1000);
  }// for(i)

  Utility::PrintNormal(func_name, "Finish Erase EEPROM");
}

// Set CTRL reg ------------------------------------------------------------
void
SetCTRLReg(const std::string ip, uint32_t reg_value)
{
  static const std::string func_name {std::string("::") + __func__ + "()"};

  std::ostringstream message;
  message << "Trying set CTRL reg as 0x" << std::hex << reg_value << std::dec;
  Utility::PrintNormal(func_name, message.str());

  UDPRBCP udp_man(ip, gUdpPort, DebugMode::kNoDisp);
  uint32_t addr = 0xfffffcff;
  uint8_t wd    = 0x0;
  udp_man.SetWD(addr, 1, &wd);
  udp_man.DoRBCP();

  // set CTRL reg as reg_value
  addr = 0xfffffc10;
  wd   = static_cast<uint8_t>(reg_value);
  udp_man.SetWD(addr, 1, &wd);
  udp_man.DoRBCP();
  
  Utility::PrintNormal(func_name, "Complete");
}
};// End of namespace RBCP
