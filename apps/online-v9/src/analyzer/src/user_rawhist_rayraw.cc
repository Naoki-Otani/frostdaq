// -*- C++ -*-

// Author: Rintaro Kurata

#include <iostream>
#include <iterator>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include <TGFileBrowser.h>
#include <TH1.h>
#include <TH2.h>
#include <TMath.h>
#include <TStyle.h>

#include <DAQNode.hh>
#include <filesystem_util.hh>
#include <Unpacker.hh>
#include <UnpackerManager.hh>

#include "Controller.hh"
#include "user_analyzer.hh"

#include "ConfMan.hh"
#include "DCDriftParamMan.hh"
#include "DCGeomMan.hh"
#include "DCTdcCalibMan.hh"
#include "DetectorID.hh"
#include "GuiPs.hh"
#include "HistMaker.hh"
#include "HodoParamMan.hh"
#include "HodoPHCMan.hh"
#include "MacroBuilder.hh"
#include "MatrixParamMan.hh"
#include "MsTParamMan.hh"
#include "ProcInfo.hh"
#include "PsMaker.hh"
#include "SsdAnalyzer.hh"
#include "TpcPadHelper.hh"
#include "UserParamMan.hh"

#define DEBUG      0
#define FLAG_DAQ   1
#define TIME_STAMP 0

// temporalily
#define SINGLE_PLANE 0 // 1 RAYRAW board
#define MULTI_PLANE  1 // 2 RAYRAW boards

namespace
{
using hddaq::unpacker::GUnpacker;
using hddaq::unpacker::DAQNode;
const auto& gUnpacker = GUnpacker::get_instance();
auto& gHist           = HistMaker::getInstance();
// const auto& gMatrix   = MatrixParamMan::GetInstance();
// auto& gTpcPad         = TpcPadHelper::GetInstance();
const auto& gHodo     = HodoParamMan::GetInstance();
const auto& gUser     = UserParamMan::GetInstance();
std::vector<TH1*> hptr_array;
Bool_t flag_event_cut = false;
Int_t event_cut_factor = 1; // for fast semi-online analysis
}

namespace analyzer
{

//____________________________________________________________________________
Int_t
process_begin(const std::vector<std::string>& argv)
{
  auto& gConfMan = ConfMan::GetInstance();
  gConfMan.Initialize(argv);
  gConfMan.InitializeParameter<HodoParamMan>("HDPRM");
  gConfMan.InitializeParameter<HodoPHCMan>("HDPHC");
  // gConfMan.InitializeParameter<DCGeomMan>("DCGEO");
  // gConfMan.InitializeParameter<DCTdcCalibMan>("DCTDC");
  // gConfMan.InitializeParameter<DCDriftParamMan>("DCDRFT");
  // gConfMan.InitializeParameter<MatrixParamMan>("MATRIX2D1",
  // 					       "MATRIX2D2",
  // 					       "MATRIX3D");
  // gConfMan.InitializeParameter<TpcPadHelper>("TPCPAD");
  gConfMan.InitializeParameter<UserParamMan>("USER");

  if (!gConfMan.IsGood()) return -1;

  // unpacker and all the parameter managers are initialized at this stage

  if (argv.size()==4) {
    Int_t factor = std::strtod(argv[3].c_str(), NULL);
    if (factor!=0) event_cut_factor = std::abs(factor);
    flag_event_cut = true;
    std::cout << "#D Event cut flag on : factor="
	      << event_cut_factor << std::endl;
  }

  // Make tabs
  hddaq::gui::Controller& gCon = hddaq::gui::Controller::getInstance();
  TGFileBrowser *tab_hist  = gCon.makeFileBrowser("Hist");
  TGFileBrowser *tab_macro = gCon.makeFileBrowser("Macro");
  TGFileBrowser *tab_misc  = gCon.makeFileBrowser("Misc");

  // Add macros to the Macro tab
  //tab_macro->Add(hoge());
  tab_macro->Add(macro::Get("clear_all_canvas"));
  tab_macro->Add(macro::Get("clear_canvas"));
  tab_macro->Add(macro::Get("split22"));
  tab_macro->Add(macro::Get("split32"));
  tab_macro->Add(macro::Get("split33"));
  tab_macro->Add(macro::Get("dispRAYRAW"));
  tab_macro->Add(macro::Get("dispRAYRAW_Waveform"));
  tab_macro->Add(macro::Get("dispRAYRAW_wave_TDC"));
  tab_macro->Add(macro::Get("dispRAYRAW_TDC"));
  tab_macro->Add(macro::Get("dispDAQ"));

  // Add histograms to the Hist tab
  tab_hist->Add(gHist.createRAYRAW());

#if FLAG_DAQ
  tab_hist->Add(gHist.createDAQ());
#endif

#if TIME_STAMP
  tab_hist->Add(gHist.createTimeStamp(false));
#endif

  tab_hist->Add(gHist.createDCEff());

  //misc tab
  tab_misc->Add(gHist.createBTOF());

  // set histogram pointers to the vector sequentially.
  // This vector contains both TH1 and TH2.
  // Then you need to do down cast when you use TH2.
  if (0 != gHist.setHistPtr(hptr_array)) { return -1; }

  // Users don't have to touch this section (Make Ps tab),
  // but the file path should be changed.
  // ----------------------------------------------------------
  auto& gPsMaker = PsMaker::getInstance();
  std::vector<TString> detList;
  std::vector<TString> optList;
  gHist.getListOfPsFiles(detList);
  gPsMaker.getListOfOption(optList);

  auto& gPsTab = hddaq::gui::GuiPs::getInstance();
  gPsTab.setFilename(Form("%s/PSFile/pro/default.ps", std::getenv("HOME")));
  gPsTab.initialize(optList, detList);
  // ----------------------------------------------------------

  gStyle->SetOptStat(1110);
  gStyle->SetTitleW(.400);
  gStyle->SetTitleH(.100);
  gStyle->SetStatW(.320);
  gStyle->SetStatH(.250);

  return 0;
}

//____________________________________________________________________________
Int_t
process_end()
{
  hptr_array.clear();
  return 0;
}

//____________________________________________________________________________
Int_t
process_event()
{
#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

  const Int_t event_number = gUnpacker.get_event_number();
  if (flag_event_cut && event_number%event_cut_factor!=0)
    return 0;

#if MULTI_PLANE
  // RAYRAW ---------------------------------------------------
  {
    static const Int_t k_device    = gUnpacker.get_device_id("RAYRAW");
    static const Int_t k_fadc      = gUnpacker.get_data_id("RAYRAW", "fadc");
    static const Int_t k_crs_cnt   = gUnpacker.get_data_id("RAYRAW", "crs_cnt");
    static const Int_t k_leading   = gUnpacker.get_data_id("RAYRAW", "leading");
    static const Int_t k_trailing  = gUnpacker.get_data_id("RAYRAW", "trailing");
    static const Int_t fadc_id     = gHist.getSequentialID(kRAYRAW, 0, kFADC, 0); // 0 origin
    static const Int_t max_adc_id  = gHist.getSequentialID(kRAYRAW, 0, kADC, 0); // 0 origin
    static const Int_t qdc_id      = gHist.getSequentialID(kRAYRAW, 0, kQDC, 0); // 0 origin
    static const Int_t tdc_id      = gHist.getSequentialID(kRAYRAW, 0, kTDC, 0); // 0 origin
    static const Int_t tot_id      = gHist.getSequentialID(kRAYRAW, 0, kTOT, 0); // 0 origin
    static const Int_t fadc_wl_id  = gHist.getSequentialID(kRAYRAW, 0, kFADCwTDC, 0); // 0 origin
    static const Int_t fadc_wol_id = gHist.getSequentialID(kRAYRAW, 0, kADCwTDC, 0); // 0 origin
    static const auto  tdc_min     = gUser.GetParameter("TdcRAYRAW", 0);
    static const auto  tdc_max     = gUser.GetParameter("TdcRAYRAW", 1);
    static const auto  win_min     = gUser.GetParameter("RangeRAYRAW", 0);
    static const auto  win_max     = gUser.GetParameter("RangeRAYRAW", 1);

    for(Int_t plane=0; plane<NumOfPlaneRAYRAW; ++plane){
      for(Int_t seg=0; seg<NumOfSegRAYRAW; ++seg) {

	//Double_t baseline = gHodo.GetP0(k_device, plane, seg, 0, 0); // Waveform Baseline
  //std::cout << "baseline: " << baseline << std::endl;
	Double_t baseline = 512;
  Double_t max_adc  = 0.;
	Double_t qdc      = 0.;
	Int_t  leading_hit_in   = 0;
	Int_t  leading_hit_out  = 0;



	// TDC Leading
	for(Int_t j=0, k=gUnpacker.get_entries(k_device, plane, seg, 0, k_leading); j<k; ++j) {
	  auto leading = gUnpacker.get(k_device, plane, seg, 0, k_leading, j);
	  if (leading != 0) {
	    hptr_array[tdc_id + plane*NumOfSegRAYRAW + seg]->Fill(leading);
      std::cout << "leading: " << leading
                << ", tdc_min: " << tdc_min
                << ", tdc_max: " << tdc_max
                << std::endl;
	    if(tdc_min < leading && leading < tdc_max)
	      leading_hit_in += 1;
	    else
	      leading_hit_out += 1;
	  }
	}

	// TDC Trailing
	for(Int_t j=0, k=gUnpacker.get_entries(k_device, plane, seg, 0, k_trailing); j<k; ++j) {
	  auto trailing = gUnpacker.get(k_device, plane, seg, 0, k_trailing, j);
	  if (trailing != 0)
	    hptr_array[tot_id + plane*NumOfSegRAYRAW + seg]->Fill(trailing);
	}

  std::cout << "fadc_id: " << fadc_id
              << ", qdc_id: " << qdc_id
              << ", plane: " << plane
              << ", seg: " << seg
              << std::endl;

  std::cout << "fadc_id + plane*NumOfSegRAYRAW + seg: "
                << fadc_id + plane*NumOfSegRAYRAW + seg
                << std::endl;

  std::cout << "qdc_id + plane*NumOfSegRAYRAW + seg: "
                << qdc_id + plane*NumOfSegRAYRAW + seg
                << std::endl;

	// Waveform
	for(Int_t m=0, n=gUnpacker.get_entries(k_device, plane, seg, 0, k_fadc);  m<n; ++m) {
	  auto fadc    = gUnpacker.get(k_device, plane, seg, 0, k_fadc, m);
	  auto crs_cnt = gUnpacker.get(k_device, plane, seg, 0, k_crs_cnt, m);

	  hptr_array[fadc_id + plane*NumOfSegRAYRAW + seg]->Fill(m, fadc);
	  // hptr_array[fadc_id + plane*NumOfSegRAYRAW + seg]->Fill(crs_cnt, fadc);

	  // QDC
	  //if(win_min < m && m < win_max)
	  qdc += fadc - baseline;

	  // ADC
	  if(fadc > max_adc)
	    max_adc = fadc;

	  if(leading_hit_in > 0)
	    hptr_array[fadc_wl_id + plane*NumOfSegRAYRAW + seg]->Fill(m, fadc);



	  // if(leading_hit_out > 0)
	  // hptr_array[fadc_wol_id + plane*NumOfSegRAYRAW + seg]->Fill(m, fadc);
	}
	hptr_array[max_adc_id + plane*NumOfSegRAYRAW + seg]->Fill(max_adc);
	hptr_array[qdc_id + plane*NumOfSegRAYRAW + seg]->Fill(qdc);
      } // for seg
    } // for plane

    //   // Bool_t l1_flag =
    //   //   trigger_flag[trigger::kL1SpillOn] ||
    //   //   trigger_flag[trigger::kL1SpillOff] ||
    //   //   trigger_flag[trigger::kSpillOnEnd] ||
    //   //   trigger_flag[trigger::kSpillOffEnd];
    //   // if(!l1_flag)
    //   //   hddaq::cerr << "#W Trigger flag is missing : "
    //   // 		  << trigger_flag << std::endl;

#if 0
    gUnpacker.dump_data_device(k_device);
#endif
  } // End of RAYRAW

#endif // MULTI_PLANE

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif


  return 0;

  // TriggerFlag ---------------------------------------------------
  std::bitset<NumOfSegTFlag> trigger_flag;
  {
    static const Int_t k_device = gUnpacker.get_device_id("TFlag");
    static const Int_t k_tdc    = gUnpacker.get_data_id("TFlag", "tdc");
    static const Int_t tdc_id   = gHist.getSequentialID(kTriggerFlag, 0, kTDC);
    static const Int_t hit_id   = gHist.getSequentialID(kTriggerFlag, 0, kHitPat);
    for(Int_t seg=0; seg<NumOfSegTFlag; ++seg) {
      for(Int_t m=0, n=gUnpacker.get_entries(k_device, 0, seg, 0, k_tdc);
	  m<n; ++m) {
	auto tdc = gUnpacker.get(k_device, 0, seg, 0, k_tdc, m);
	if (tdc>0) {
	  trigger_flag.set(seg);
	  hptr_array[tdc_id+seg]->Fill(tdc);
	}
      }
      if (trigger_flag[seg]) hptr_array[hit_id]->Fill(seg);
    }

    Bool_t l1_flag =
      trigger_flag[trigger::kL1SpillOn] ||
      trigger_flag[trigger::kL1SpillOff] ||
      trigger_flag[trigger::kSpillOnEnd] ||
      trigger_flag[trigger::kSpillOffEnd];
    if(!l1_flag)
      hddaq::cerr << "#W Trigger flag is missing : "
		  << trigger_flag << std::endl;

#if 0
    gUnpacker.dump_data_device(k_device);
#endif
  }

#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif

#if TIME_STAMP
  // TimeStamp --------------------------------------------------------
  {
    static const auto hist_id = gHist.getSequentialID(kTimeStamp, 0, kTDC);
    Int_t i = 0;
    for(auto&& c : gUnpacker.get_root()->get_child_list()) {
      if (!c.second)
	continue;
      auto t = gUnpacker.get_node_header(c.second->get_id(),
					 DAQNode::k_unix_time);
      hptr_array[hist_id+i]->Fill(t);
      ++i;
    }
  }
#endif

  UInt_t cobo_data_size = 0;

#if FLAG_DAQ
  { ///// DAQ
    //___ node id
    static const Int_t k_eb = gUnpacker.get_fe_id("k18eb");
    std::vector<Int_t> vme_fe_id;
    std::vector<Int_t> hul_fe_id;
    std::vector<Int_t> ea0c_fe_id;
    std::vector<Int_t> cobo_fe_id;
    for(auto&& c : gUnpacker.get_root()->get_child_list()) {
      if (!c.second) continue;
      TString n = c.second->get_name();
      auto id = c.second->get_id();
      if (n.Contains("vme"))
	vme_fe_id.push_back(id);
      if (n.Contains("hul"))
	hul_fe_id.push_back(id);
      if (n.Contains("easiroc"))
	ea0c_fe_id.push_back(id);
      if (n.Contains("cobo"))
	cobo_fe_id.push_back(id);
    }

    //___ sequential id
    static const Int_t eb_hid = gHist.getSequentialID(kDAQ, kEB, kHitPat);
    static const Int_t vme_hid = gHist.getSequentialID(kDAQ, kVME, kHitPat2D);
    static const Int_t hul_hid = gHist.getSequentialID(kDAQ, kHUL, kHitPat2D);
    static const Int_t ea0c_hid = gHist.getSequentialID(kDAQ, kEASIROC, kHitPat2D);
    static const Int_t cobo_hid = gHist.getSequentialID(kDAQ, kCoBo, kHitPat2D);

    { //___ EB
      auto data_size = gUnpacker.get_node_header(k_eb, DAQNode::k_data_size);
      hptr_array[eb_hid]->Fill(data_size);
    }

    { //___ VME
      for(Int_t i=0, n=vme_fe_id.size(); i<n; ++i) {
	auto data_size = gUnpacker.get_node_header(vme_fe_id[i], DAQNode::k_data_size);
	hptr_array[vme_hid]->Fill(i, data_size);
      }
    }

    { // EASIROC & VMEEASIROC node
      for(Int_t i=0, n=ea0c_fe_id.size(); i<n; ++i) {
	auto data_size = gUnpacker.get_node_header(ea0c_fe_id[i], DAQNode::k_data_size);
	hptr_array[ea0c_hid]->Fill(i, data_size);
      }
    }

    { //___ HUL node
      for(Int_t i=0, n=hul_fe_id.size(); i<n; ++i) {
	auto data_size = gUnpacker.get_node_header(hul_fe_id[i], DAQNode::k_data_size);
	hptr_array[hul_hid]->Fill(i, data_size);
      }
    }

    { //___ CoBo node
      for(Int_t i=0, n=cobo_fe_id.size(); i<n; ++i) {
	auto data_size = gUnpacker.get_node_header(cobo_fe_id[i], DAQNode::k_data_size);
	hptr_array[cobo_hid]->Fill(i, data_size);
	cobo_data_size += data_size;
      }
    }
  }

#endif

  if (trigger_flag[trigger::kSpillOnEnd] || trigger_flag[trigger::kSpillOffEnd])
    return 0;


#if DEBUG
  std::cout << __FILE__ << " " << __LINE__ << std::endl;
#endif
  return 0;
} //process_event()

} //analyzer
