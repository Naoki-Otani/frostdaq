#include <iostream>
#include <sstream>
#include <cstdio>

#include "RegisterMapCommon.hh"
#include "UDPRBCP.hh"
#include "FPGAModule.hh"
#include "SitcpController.hh"

enum argIndex{kBin, kIp, kReg};
using namespace HUL;
int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "set_ctrl_eeprom [IP address] [CTRL regsiter (hex)]" << std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip = argv[kIp];
  std::string ctrl_reg = argv[kReg];

  std::istringstream iss_reg(ctrl_reg);
  uint32_t reg_val;
  iss_reg >> std::hex >> reg_val;
  
  RBCP::SetCTRLReg(board_ip, reg_val);
  
  return 0;

}// main
