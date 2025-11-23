#ifndef SPI_CONFIG_LOADER_H_
#define SPI_CONFIG_LOADER_H_

#include <cstdint>
#include "rayraw_const.hh"
#include "SPI_struct.hh"

class SPIConfigLoader
{
public :
  SPIConfigLoader();

  void read_EXT_CLK();
  void read_SDIF_ADC_CLK_POL();
  void read_SDIF_Rn();
  void read_ENB_CRC();
  void read_OSC_R();
  void read_CLK_OUT_POL();
  void read_TP_MASK();
  void read_AMON();
  void read_MPPC_BIAS();
  void read_VTH_COMP();
  void read_RF_B_SHP();
  void read_C_B_SHP();
  void read_RIN_B_SHP();
  void read_CIN_B_VGA();
  void read_CF_B_VGA();
  void read_SPI_Values();
  void make_RegisterValues();

  static SPIConfigLoader& get_instance();
  
private :
  SPI_PLL_SDIF m_SPI_PLL_SDIF[NumOfYaenami];
  SPI_CH       m_SPI_CH[NumOfYaenami][NumOfCh];
  SPI_ADC_VCM_DAC m_SPI_ADC_VCM_DAC[NumOfYaenami];

  int EXT_CLK[4];
  int SDIF_ADC_CLK_POL[4];
  int SDIF_Rn[4];
  int ENB_CRC[4];
  int OSC_R[4];
  int CLK_OUT_POL[4];
  int TP_MASK[4][8];
  int AMON[4][8];
  int MPPC_BIAS[4][8];
  int VTH_COMP[4][8];
  int RF_B_SHP[4][8];
  int C_B_SHP[4][8];
  int RIN_B_SHP[4][8];
  int CIN_B_VGA[4][8];
  int CF_B_VGA[4][8];

  void initialize_SPI_Values();

};

  inline SPIConfigLoader&
  SPIConfigLoader::get_instance()
  {
    static SPIConfigLoader inst;
    return inst;
  }


#endif
