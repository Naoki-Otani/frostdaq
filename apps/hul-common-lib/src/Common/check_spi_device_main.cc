#include <iostream>
#include <cstdio>

#include "RegisterMapCommon.hh"
#include "UDPRBCP.hh"
#include "FPGAModule.hh"
#include "FlashMemoryProgrammer.hh"

using namespace HUL;
int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Check SPI device name\n";
    std::cout << "Usage\n";
    std::cout << "check_spi_device [IP address]" << std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::vector<std::string> in_arg(argc-1);
  std::copy(argv+1, argv+argc, in_arg.begin());

  std::string ip_address = in_arg[0];
  
  RBCP::UDPRBCP udp_rbcp(ip_address, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);
  HUL::FlashMemoryProgrammer fmp(fpga_module);

  fmp.InitializeRegister();
  fmp.SendDummyClock();
  const std::string device_name = fmp.CheckSpiDevice();

  return 0;

}// main
