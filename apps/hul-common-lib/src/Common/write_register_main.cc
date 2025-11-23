#include <iostream>
#include <cstdio>
#include <ios>
#include <iomanip>
#include <sstream>

#include "RegisterMapCommon.hh"
#include "UDPRBCP.hh"
#include "FPGAModule.hh"


enum argIndex{kBin, kIp, kAddr, kData, kNBytes};
using namespace HUL;

int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "write_register [IP address] [Address (hex)] [Data (hex)] [Num bytes]" << std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip     = argv[kIp];
  std::string rbcp_address = argv[kAddr];
  std::string rbcp_data    = argv[kData];
  std::string num_bytes    = argv[kNBytes];

  RBCP::UDPRBCP udp_rbcp(board_ip, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);

  std::istringstream iss_addr(rbcp_address);
  std::istringstream iss_data(rbcp_data);
  std::istringstream iss_nbytes(num_bytes);
  uint32_t address_val;
  uint32_t data_val;
  uint32_t nbytes_val;
  iss_addr   >> std::hex >> address_val;
  iss_data   >> std::hex >> data_val;
  iss_nbytes >> nbytes_val;

  fpga_module.WriteModule(address_val, data_val, nbytes_val);

  std::cout << "#D: Write register: " << std::dec << data_val << " (0x" << std::hex << data_val << ")" << std::endl;
  
  return 0;

}// main

