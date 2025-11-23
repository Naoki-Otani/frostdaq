#ifndef FWMAGIC_MAP_HH
#define FWMAGIC_MAP_HH

#include<stdint.h>
#include<string>
#include<map>

namespace HUL{
  using FwMagicMap = std::map<const uint32_t, const std::string>;
  const FwMagicMap g_fw_map =
    {
      {0xF000, "MikumariClock-Root"},
      {0xF100, "MikumariClock-Hub"},
      {0x60C4, "AMANEQ StrLrTdc"},
      {0xC480, "AMANEQ StrHrTdc Base"},
      {0xA000, "AMANEQ Skeleton"},
      {0x390A, "MikumariPrim-Ex"},
      {0x390B, "MikumariSecond-Ex"},
      {0x413E, "CIRASAME Skeleton"},
      {0x6041, "CIRASAME StrTdc"},
      {0x7041, "CIRASAME StrTdc-10G"}
    };

    const FwMagicMap g_mzn_fw_map =
    {
      {0x804C, "Mezzanine StrHrTdc"}
    };

};

#endif
