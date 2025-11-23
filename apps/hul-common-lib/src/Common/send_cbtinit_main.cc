#include <iostream>
#include <cstdio>
#include <ios>
#include <iomanip>
#include <sstream>

#include "RegisterMapCommon.hh"
#include "UDPRBCP.hh"
#include "FPGAModule.hh"
#include "Utility.hh"

enum argIndex{kBin, kIp, kPortNum, kAddrOffset};
using namespace HUL;

int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "send_cbtinit [IP address] [Miku port number (0-31)] [Address offset (hex)(option)]" << std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip     = argv[kIp];
  std::string port_num     = argv[kPortNum];
  std::string addr_offset  = "0x00000000";
  if(argc == kAddrOffset+1){
    addr_offset  = argv[kAddrOffset];
  }

  RBCP::UDPRBCP udp_rbcp(board_ip, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);

  std::istringstream iss_port(port_num);
  std::istringstream iss_addr(addr_offset);
  uint32_t port_val;
  uint32_t addr_offset_val;
  iss_port >> port_val;
  iss_addr >> std::hex >> addr_offset_val;

  uint32_t reg = (1 << port_val);

  fpga_module.WriteModule(LBUS::MUTIL::kAddrCbtInit + addr_offset_val, reg, 4);
  fpga_module.WriteModule(LBUS::MUTIL::kAddrCbtInit + addr_offset_val, 0,   4);

  Utility::PrintNormal("", "Sent CbtInit to port number " + port_num);
  
  return 0;

}// main

