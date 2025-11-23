#include <iostream>
#include <cstdio>

#include "RegisterMapCommon.hh"
#include "UDPRBCP.hh"
#include "FPGAModule.hh"

using namespace HUL;
int main(int argc, char* argv[])
{
  if(1 == argc){
    std::cout << "Usage\n";
    std::cout << "read_xadc [IP address]" << std::endl;
    return 0;
  }// usage
  
  // body ------------------------------------------------------
  std::string board_ip = argv[1];

  RBCP::UDPRBCP udp_rbcp(board_ip, RBCP::gUdpPort, RBCP::DebugMode::kNoDisp);
  HUL::FPGAModule fpga_module(udp_rbcp);
			  

  // XADC _____________________________________________________________
  fpga_module.WriteModule(LBUS::SDS::kAddrXadcDrpMode, LBUS::SDS::kDrpReadMode );

  // Read temperature monitor
  fpga_module.WriteModule(LBUS::SDS::kAddrXadcDrpAddr, LBUS::SDS::kAddrDrpTemp);
  fpga_module.WriteModule(LBUS::SDS::kAddrXadcExecute, 1);
  uint32_t adc_temp = fpga_module.ReadModule(LBUS::SDS::kAddrXadcDrpDout, 2);

  // Read VCCINT
  fpga_module.WriteModule(LBUS::SDS::kAddrXadcDrpAddr, LBUS::SDS::kAddrDrpVccInt);
  fpga_module.WriteModule(LBUS::SDS::kAddrXadcExecute, 1);
  uint32_t adc_vccint = fpga_module.ReadModule(LBUS::SDS::kAddrXadcDrpDout, 2);

  // Read VCCAUX
  fpga_module.WriteModule(LBUS::SDS::kAddrXadcDrpAddr, LBUS::SDS::kAddrDrpVccAux);
  fpga_module.WriteModule(LBUS::SDS::kAddrXadcExecute, 1);
  uint32_t adc_vccaux = fpga_module.ReadModule(LBUS::SDS::kAddrXadcDrpDout, 2);

  // Read status
  uint32_t status = fpga_module.ReadModule(LBUS::SDS::kAddrSdsStatus, 1);
  
  // translate
  const uint32_t shift   = 4;
  //  const uint32_t mask    = 0xfff;
  const uint32_t max_adc = 0x1000;

  double temp = (adc_temp >> shift)*503.975/max_adc - 273.15; // 503.975: magic number
  double vccint = 3.0*(adc_vccint >> shift)/max_adc;
  double vccaux = 3.0*(adc_vccaux >> shift)/max_adc;

  printf("FPGA temp.  : %.2f C\n", temp);
  printf("VCCINT      : %.4f V\n", vccint);
  printf("VCCAUX      : %.4f V\n", vccaux);
  printf("\n");

  // Status
  printf("XADC status (0:false, 1:true)\n");
  printf(" - Over temp.   (Th. 125C)   : %d\n", (status & LBUS::SDS::kXadcOverTemperature) >> 0);
  printf(" - Temp alarm   (Th. 85C)    : %d\n", (status & LBUS::SDS::kXadcUserTempAlarm)   >> 1);
  printf(" - VCCINT alarm (0.97-1.03V) : %d\n", (status & LBUS::SDS::kXadcUserVccIntAlarm) >> 2);
      
  
  return 0;

}// main
