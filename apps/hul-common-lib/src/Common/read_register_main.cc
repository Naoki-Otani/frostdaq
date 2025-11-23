#include <iostream>
#include <cstdio>
#include <ios>
#include <iomanip>
#include <sstream>

#include "RegisterMapCommon.hh"
#include "UDPRBCP.hh"
#include "FPGAModule.hh"

enum argIndex{kBin, kIp, kAddr, kNBytes};
using namespace HUL;

int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "read_register [IP address] [Address (hex)] [Num bytes]" << std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip     = argv[kIp];
  std::string rbcp_address = argv[kAddr];
  std::string num_bytes    = argv[kNBytes];

  RBCP::UDPRBCP udp_rbcp(board_ip, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);

  std::istringstream iss_addr(rbcp_address);
  std::istringstream iss_nbytes(num_bytes);
  uint32_t address_val;
  uint32_t nbytes_val;
  iss_addr   >> std::hex >> address_val;
  iss_nbytes >> nbytes_val;

  uint32_t register_val = fpga_module.ReadModule(address_val, nbytes_val);

  std::cout << "#D: Read register: " << register_val << " (0x" << std::hex << register_val << ")" << std::endl;
  
  return 0;

}// main

