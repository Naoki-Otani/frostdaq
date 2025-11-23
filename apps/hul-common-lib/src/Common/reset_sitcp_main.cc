#include <iostream>
#include <sstream>
#include <cstdio>

#include "RegisterMapCommon.hh"
#include "UDPRBCP.hh"
#include "FPGAModule.hh"
#include "FlashMemoryProgrammer.hh"
#include "SitcpController.hh"
#include "rbcp.hh"

enum argIndex{kBin, kIp};
using namespace HUL;
int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "reset_sitcp [IP address]" << std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip  = argv[kIp];
  RBCP::ResetSiTCP(board_ip);
  
  return 0;

}// main
