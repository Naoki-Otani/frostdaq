// -*- C++ -*-

// Author: Tomonori Takahashi

#include "UnpackerRegister.hh"

#include "UnpackerImpl.hh"
#include "DAQNode.hh"
#include "Copper.hh"
#include "CopperLite.hh"


#include "FinesseEncoder.hh"
#include "FinesseEncoderInverse.hh"
#include "FinesseEncoderPPG.hh"
#include "FinessePtdc.hh"

#include "SoyTimeStamp.hh"
#include "Hul3DMtx.hh"
#include "HulScaler.hh"
#include "HulMHTdc.hh"
#include "HulHRTdc.hh"
#include "HulMsT.hh"
#include "HulMassTrigFera.hh"
#include "HulResetTime.hh"
#include "HulUmem.hh"

#include "VmeAPVDaq.hh"
#include "VmeCaenV775.hh"
#include "VmeCaenV792.hh"
#include "VmeCaenV820.hh"
#include "VmeCaenV830.hh"
#include "VmeCaenV1724.hh"
#include "VmeCaenV1743.hh"
#include "VmeEmc.hh"
#include "VmeInputReg.hh"
#include "VmeTdc64M.hh"
#include "VmeV1290A.hh"
#include "VmeMaster.hh"
#include "VmeRm.hh"
#include "VmeRpv100.hh"
#include "VmeRpv130.hh"
#include "VmeSmp.hh"

#include "Tko.hh"
#include "TkoDrT.hh"
#include "TkoDrTII.hh"
#include "TkoMHTdc.hh"
#include "TkoMHTdcCSds.hh"
#include "TkoRm.hh"

#include "Easiroc.hh"
#include "VmeEasiroc.hh"

#include "Rayraw.hh"

#include "VmeAmt.hh"
#include "VmeUmemAD413A.hh"
#include "VmeUmem3377.hh"
#include "VmeUmem.hh"
#include "FeraAD413A.hh"
#include "Fera3377.hh"
#include "HBJTimeStamp.hh"

#include "VVmeCamacRm.hh"
#include "VVmeFERA4300B.hh"
#include "VVmeCoinReg.hh"

#include "GetCoBoMaster.hh"
#include "GetAsAd.hh"

#include "std_ostream.hh"

namespace hddaq
{
  namespace unpacker
  {

//______________________________________________________________________________
UnpackerRegister::UnpackerRegister()
{
//   cout << "#D UnpackerRegister()" << std::endl;
  UnpackerFactory& g_factory = GUnpackerFactory::get_instance();

  g_factory.add_entry(Copper::k_type,     create<Copper>);
  g_factory.add_entry(CopperLite::k_type, create<CopperLite>);
  g_factory.add_entry(DAQNode::k_type,    create<DAQNode>);

  g_factory.add_entry(FinesseEncoder::k_type,        create<FinesseEncoder>);
  g_factory.add_entry(FinesseEncoderInverse::k_type, create<FinesseEncoderInverse>);
  g_factory.add_entry(FinesseEncoderPPG::k_type,     create<FinesseEncoderPPG>);
  g_factory.add_entry(FinessePtdc::k_type,           create<FinessePtdc>);

  g_factory.add_entry(SoyTimeStamp::k_type,    create<SoyTimeStamp>);
  g_factory.add_entry(Hul3DMtx::k_type,        create<Hul3DMtx>);
  g_factory.add_entry(HulScaler::k_type,       create<HulScaler>);
  g_factory.add_entry(HulMHTdc::k_type,        create<HulMHTdc>);
  g_factory.add_entry(HulHRTdc::k_type,        create<HulHRTdc>);
  g_factory.add_entry(HulMsT::k_type,          create<HulMsT>);
  g_factory.add_entry(HulMassTrigFera::k_type, create<HulMassTrigFera>);
  g_factory.add_entry(HulResetTime::k_type,    create<HulResetTime>);
  g_factory.add_entry(HulUmem::k_type,         create<HulUmem>);

  g_factory.add_entry(Tko::k_type,          create<Tko>);
  g_factory.add_entry(TkoDrT::k_type,       create<TkoDrT>);
  g_factory.add_entry(TkoDrTII::k_type,     create<TkoDrTII>);
  g_factory.add_entry(TkoRm::k_type,        create<TkoRm>);
  g_factory.add_entry(TkoMHTdc::k_type,     create<TkoMHTdc>);
  g_factory.add_entry(TkoMHTdcCSds::k_type, create<TkoMHTdcCSds>);

  g_factory.add_entry(VmeAPVDaq::k_type,   create<VmeAPVDaq>);
  g_factory.add_entry(VmeCaenV775::k_type, create<VmeCaenV775>);
  g_factory.add_entry(VmeCaenV792::k_type, create<VmeCaenV792>);
  g_factory.add_entry(VmeCaenV820::k_type, create<VmeCaenV820>);
  g_factory.add_entry(VmeCaenV830::k_type, create<VmeCaenV830>);
  g_factory.add_entry(VmeCaenV1724::k_type, create<VmeCaenV1724>);
  g_factory.add_entry(VmeCaenV1743::k_type, create<VmeCaenV1743>);
  g_factory.add_entry(VmeEmc::k_type,      create<VmeEmc>);
  g_factory.add_entry(VmeInputReg::k_type, create<VmeInputReg>);
  g_factory.add_entry(VmeTdc64M::k_type,   create<VmeTdc64M>);
  g_factory.add_entry(VmeRm::k_type,       create<VmeRm>);
  g_factory.add_entry(VmeRpv100::k_type,   create<VmeRpv100>);
  g_factory.add_entry(VmeRpv130::k_type,   create<VmeRpv130>);
  g_factory.add_entry(VmeMaster::k_type,   create<VmeMaster>);
  g_factory.add_entry(VmeSmp::k_type,      create<VmeSmp>);
  g_factory.add_entry(VmeV1290A::k_type,   create<VmeV1290A>);
  g_factory.add_entry(Easiroc::k_type,     create<Easiroc>);
  g_factory.add_entry(VmeEasiroc::k_type,     create<VmeEasiroc>);

  g_factory.add_entry(Rayraw::k_type,     create<Rayraw>);
  
  g_factory.add_entry(VmeAmt::k_type,        create<VmeAmt>);
  g_factory.add_entry(VmeUmemAD413A::k_type, create<VmeUmemAD413A>);
  g_factory.add_entry(VmeUmem3377::k_type,   create<VmeUmem3377>);
  g_factory.add_entry(VmeUmem::k_type,       create<VmeUmem>);
  g_factory.add_entry(FeraAD413A::k_type,    create<FeraAD413A>);
  g_factory.add_entry(Fera3377::k_type,      create<Fera3377>);
  g_factory.add_entry(HBJTimeStamp::k_type,  create<HBJTimeStamp>);

  g_factory.add_entry(VVmeCamacRm::k_type,   create<VVmeCamacRm>);
  g_factory.add_entry(VVmeFERA4300B::k_type, create<VVmeFERA4300B>);
  g_factory.add_entry(VVmeCoinReg::k_type,   create<VVmeCoinReg>);

  g_factory.add_entry(GetCoBoMaster::k_type,   create<GetCoBoMaster>);
  g_factory.add_entry(GetAsAd::k_type,         create<GetAsAd>);

}

//______________________________________________________________________________
UnpackerRegister::UnpackerRegister(const id_type& type,
				   creator_type creator)
{
  GUnpackerFactory::get_instance().add_entry(type, creator);
}

//______________________________________________________________________________
UnpackerRegister::~UnpackerRegister()
{
//   cout << "#D UnpackerRegister::~UnpackerRegister()" << std::endl;
}


  }
}
