#include <iostream>
#include <sstream>
#include <cstdio>

#include "RegisterMapCommon.hh"
#include "UDPRBCP.hh"
#include "FPGAModule.hh"
#include "FlashMemoryProgrammer.hh"
#include "SitcpController.hh"
#include "rbcp.hh"

enum argIndex{kBin, kIp, kAddr, kReg};
using namespace HUL;
int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "set_sitcpreg [IP address] [Addr offset (hex)] [Register (hex)]" << std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip  = argv[kIp];
  std::string addr_ofs  = argv[kAddr];
  std::string sitcp_reg = argv[kReg];

  std::istringstream iss_addr(addr_ofs);
  std::istringstream iss_reg(sitcp_reg);

  uint32_t addr_ofs_val, reg_val;
  iss_addr >> std::hex >> addr_ofs_val;
  iss_reg  >> std::hex >> reg_val;
  
  RBCP::WriteSiTCP(board_ip, addr_ofs_val, reg_val);
  
  return 0;

}// main
