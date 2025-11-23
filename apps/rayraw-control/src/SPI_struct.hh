#ifndef SPI_STRUCT_H_
#define SPI_STRUCT_H_


struct SPI_PLL_SDIF {
  uint8_t EXT_CLK;
  uint8_t SDIF_ADC_CLK_POL;
  uint8_t SDIF_Rn;
  uint8_t ENB_CRC;
  uint8_t REF_DIV_CONT;
  uint8_t OSC_R;
  uint8_t CLK_OUT_POL;
  uint8_t IBAMP;
  uint8_t IBCP;
  uint8_t OSC_CONT;
  uint8_t OSC_EN;
  uint8_t DIV_CONT;
  uint8_t N_CONT;      
};

struct SPI_CH {
  uint8_t ADC_R;
  uint8_t ASD_CLK_EN;
  uint8_t GC_BUF;
  uint8_t TP_MASK;
  uint8_t AMON;
  uint16_t MPPC_BIAS;
  uint16_t VTH_COMP;  
  uint8_t MON_SEL;   // not used, but its bits are necessary
  uint8_t MIN_PULSE_SEL;   // not used, but its bits are necessary
  uint8_t DO_POL;    
  uint8_t COMP_POL;  
  uint8_t DELAY_F;   // not used, but its bits are necessary
  uint8_t DELAY_C;   // not used, but its bits are necessary
  uint8_t RF_B_SHP;
  uint8_t C_B_SHP;
  uint8_t RIN_B_SHP;      
  uint8_t CIN_B_VGA;
  uint8_t R_B_VGA;   //not used, but its bits are necessary
  uint8_t CF_B_VGA;
  uint8_t ASD_R;
  uint8_t GD_BUF;
};

struct SPI_ADC_VCM_DAC
{
  uint8_t NC1;   //not used, but its bit is necessanry
  uint8_t CAL_EN;
  uint8_t CAL_REG_R;
  uint8_t SAM_WD;    
  uint8_t NC2;   //not used, but its bit is necessanry
  uint16_t VCM_DAC;  
};

const uint8_t EXT_CLK_mask = 0b1;
const uint8_t EXT_CLK_shift = 7;
const uint8_t SDIF_ADC_CLK_POL_mask = 0b1;
const uint8_t SDIF_ADC_CLK_POL_shift = 6;
const uint8_t SDIF_Rn_mask = 0b1;
const uint8_t SDIF_Rn_shift = 5;
const uint8_t ENB_CRC_mask = 0b1;
const uint8_t ENB_CRC_shift = 4;
const uint8_t REF_DIV_CONT_mask = 0b11;
const uint8_t REF_DIV_CONT_shift = 2;
const uint8_t OSC_R_mask = 0b1;
const uint8_t OSC_R_shift = 1;
const uint8_t CLK_OUT_POL_mask = 0b1;
const uint8_t CLK_OUT_POL_shift = 0;

const uint8_t IBAMP_mask = 0b11; 
const uint8_t IBAMP_shift = 6;
const uint8_t IBCP_mask = 0b11; 
const uint8_t IBCP_shift = 4;
const uint8_t OSC_CONT_mask = 0b1111; 
const uint8_t OSC_CONT_shift = 0;

const uint8_t OSC_EN_mask = 0b1; 
const uint8_t OSC_EN_shift = 7;
const uint8_t DIV_CONT_mask = 0b11; 
const uint8_t DIV_CONT_shift = 5;
const uint8_t N_CONT_mask = 0b11111; 
const uint8_t N_CONT_shift = 0;

const uint8_t ADC_R_mask = 0b1; 
const uint8_t ADC_R_shift = 7;
const uint8_t ASD_CLK_EN_mask = 0b1; 
const uint8_t ASD_CLK_EN_shift = 6;
const uint8_t GC_BUF_mask = 0b11; 
const uint8_t GC_BUF_shift = 4;
const uint8_t TP_MASK_mask = 0b1; 
const uint8_t TP_MASK_shift = 3;
const uint8_t AMON_mask = 0b1; 
const uint8_t AMON_shift = 2;
const uint16_t MPPC_BIAS1_mask = 0b1100000000; 
const uint8_t MPPC_BIAS1_shift = 8;

const uint8_t MPPC_BIAS2_mask = 0b0011111111; 
const uint8_t MPPC_BIAS2_shift = 0;

const uint16_t VTH_COMP1_mask = 0b1111111100; 
const uint8_t VTH_COMP1_shift = 2;

const uint16_t VTH_COMP2_mask = 0b0000000011; 
const uint8_t VTH_COMP2_shift = 6;
const uint8_t MON_SEL_mask = 0b11; 
const uint8_t MON_SEL_shift = 4;
const uint8_t MIN_PULSE_SEL_mask = 0b1; 
const uint8_t MIN_PULSE_SEL_shift = 3;
const uint8_t DO_POL_mask = 0b1; 
const uint8_t DO_POL_shift = 2;
const uint8_t COMP_POL_mask = 0b1; 
const uint8_t COMP_POL_shift = 1;
const uint8_t DELAY_F1_mask = 0b1000; 
const uint8_t DELAY_F1_shift = 3;

const uint8_t DELAY_F2_mask = 0b111; 
const uint8_t DELAY_F2_shift = 5;
const uint8_t DELAY_C_mask = 0b111; 
const uint8_t DELAY_C_shift = 2;
const uint8_t RF_B_SHP1_mask = 0b110; 
const uint8_t RF_B_SHP1_shift = 1;

const uint8_t RF_B_SHP2_mask = 0b1; 
const uint8_t RF_B_SHP2_shift = 7;
const uint8_t C_B_SHP_mask = 0b111; 
const uint8_t C_B_SHP_shift = 4;
const uint8_t RIN_B_SHP_mask = 0b111; 
const uint8_t RIN_B_SHP_shift = 1;
const uint8_t CIN_B_VGA1_mask = 0b100; 
const uint8_t CIN_B_VGA1_shift = 2;

const uint8_t CIN_B_VGA2_mask = 0b011; 
const uint8_t CIN_B_VGA2_shift = 6;
const uint8_t R_B_VGA_mask = 0b111; 
const uint8_t R_B_VGA_shift = 3;
const uint8_t CF_B_VGA_mask = 0b111; 
const uint8_t CF_B_VGA_shift = 0;

const uint8_t ASD_R_mask = 0b11;
const uint8_t ASD_R_shift = 6;
const uint8_t GD_BUF_mask = 0b11;
const uint8_t GD_BUF_shift = 4;

const uint8_t NC1_mask = 0b1;
const uint8_t NC1_shift = 7;
const uint8_t CAL_EN_mask = 0b1;
const uint8_t CAL_EN_shift = 6;
const uint8_t CAL_REG_R_mask = 0b1;
const uint8_t CAL_REG_R_shift = 5;
const uint8_t SAM_WD_mask = 0b11;
const uint8_t SAM_WD_shift = 3;
const uint8_t NC2_mask = 0b1;
const uint8_t NC2_shift = 2;
const uint16_t VCM_DAC1_mask = 0b1100000000;
const uint8_t VCM_DAC1_shift = 8;

const uint16_t VCM_DAC2_mask = 0b0011111111;
const uint8_t VCM_DAC2_shift = 0;

#endif
