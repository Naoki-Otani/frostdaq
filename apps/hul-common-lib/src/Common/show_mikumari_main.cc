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

enum argIndex{kBin, kIp, kAddrOffset};
using namespace HUL;

int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "show_mikumari [IP address] [Address offset (hex)(option)]" << std::endl;
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
  std::cout << " - CBT" << std::endl;
  bdump(fpga_module.ReadModule(LBUS::MUTIL::kAddrCbtLaneUp  + addr_offset_val, 4));
  std::cout << " - MIKUMARI" << std::endl;
  bdump(fpga_module.ReadModule(LBUS::MUTIL::kAddrMikumariUp + addr_offset_val, 4));
  std::cout << " - LACCP" << std::endl;
  bdump(fpga_module.ReadModule(LBUS::MUTIL::kAddrLaccpUp    + addr_offset_val, 4));

  std::cout << std::dec << std::endl;

  printf("Port, IdelayTap, Bitslip Number \n");
  for(uint32_t i = 0; i<num_mikumari; ++i){
    fpga_module.WriteModule(LBUS::MUTIL::kAddrRegIndex + addr_offset_val, i);
    uint32_t idelay_tap  = fpga_module.ReadModule(LBUS::MUTIL::kAddrCbtTapValueIn + addr_offset_val);
    uint32_t bitslip_num = fpga_module.ReadModule(LBUS::MUTIL::kAddrCbtBitSlipIn + addr_offset_val);
    printf(" %2d, %8d, %8d \n", i, idelay_tap, bitslip_num);
  }
  
  return 0;

}// main

