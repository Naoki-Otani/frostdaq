#include <iostream>
#include <cstdio>
#include <ios>
#include <iomanip>
#include <sstream>

#include "RegisterMapCommon.hh"
#include "UDPRBCP.hh"
#include "FPGAModule.hh"
#include "BitDump.hh"
#include "Utility.hh"

const double kToPs = 2000./2048.;

enum argIndex{kBin, kIp, kAddrOffset};
using namespace HUL;

int32_t trans_to_straight(uint32_t twos_compl)
{
  int32_t ret_val=0;
  if((twos_compl >> 15) == 1){
    // Negative val //
    ret_val = -(0xffff - (twos_compl & 0xffff) +1);
  }else{
    ret_val = static_cast<int32_t>(twos_compl);
  }

  return ret_val;
}

std::string trans_to_ipaddr(uint32_t reg)
{
  if(reg == 0){
    return "Not Connected";
  }else{
    std::ostringstream oss;
    for(int i = 3; i>=0; --i){
      oss << ((reg >> 8*i) & 0xff) << (i == 0? "" : ".");
    }

    return oss.str();
  }
}

int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "show_laccp [IP address] [Address offset (hex)(option)]" << std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip     = argv[kIp];
  std::string addr_offset  = "0x00000000";
  if(argc == kAddrOffset+1){
    addr_offset  = argv[kAddrOffset];
  }

  RBCP::UDPRBCP udp_rbcp(board_ip, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);

  std::istringstream iss_addr(addr_offset);
  uint32_t addr_offset_val;
  iss_addr >> std::hex >> addr_offset_val;

  uint32_t num_mikumari = fpga_module.ReadModule(LBUS::MUTIL::kAddrNumLinks + addr_offset_val);
  Utility::PrintNormal("", "-- Link Up status --");
  
  hddaq::BitDump bdump;
  std::cout << " - LACCP" << std::endl;
  bdump(fpga_module.ReadModule(LBUS::MUTIL::kAddrLaccpUp    + addr_offset_val, 4));
  std::cout << std::dec << std::endl;

  Utility::PrintNormal("", "-- My offset --");
  uint32_t hbc_offset = fpga_module.ReadModule(LBUS::MUTIL::kAddrHbcOffset + addr_offset_val, 2);
  std::cout << "Heartbeat count offset: " << hbc_offset << "(" << hbc_offset*8 << " ns)" << std::endl;
  uint32_t local_offset = fpga_module.ReadModule(LBUS::MUTIL::kAddrLocalFineOffset + addr_offset_val, 2);
  std::cout << "Local fine offset:      " << trans_to_straight(local_offset) << "(" << trans_to_straight(local_offset)*kToPs << " ps)" << std::endl;
  uint32_t laccp_offset = fpga_module.ReadModule(LBUS::MUTIL::kAddrLaccpFineOffset + addr_offset_val, 2);
  std::cout << "LACCP fine offset:      " << trans_to_straight(laccp_offset) << "(" << trans_to_straight(laccp_offset)*kToPs << " ps)" << std::endl;
  std::cout << std::endl;

  printf("Port, Partner IP address \n");
  for(uint32_t i = 0; i<num_mikumari; ++i){
    fpga_module.WriteModule(LBUS::MUTIL::kAddrRegIndex + addr_offset_val, i);
    uint32_t ip_addr  = fpga_module.ReadModule(LBUS::MUTIL::kAddrPartnerIpAddr + addr_offset_val, 4);
    std::cout << " " << i << ", " << trans_to_ipaddr(ip_addr) << std::endl;
  }
  
  return 0;

}// main

