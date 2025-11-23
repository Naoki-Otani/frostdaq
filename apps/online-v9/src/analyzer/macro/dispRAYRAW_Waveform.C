// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

#include "UserParamMan.hh"

void dispRAYRAW_Waveform()
{
  const UserParamMan& gUser = UserParamMan::GetInstance();
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  // Draw Waveform
  for (Int_t n =0; n<4; n++){
    TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", n+1));
    c->Clear();
    c->Divide(3,3);
    Int_t fadc_id     = HistMaker::getUniqueID(kRAYRAW, 0, kFADC, 0);
    for( Int_t i=0; i<8; ++i ){
      c->cd(i+1);
      gPad->SetLogz();
      TH1 *h = (TH1*)GHist::get( fadc_id + i + n*8);
      if( !h ) continue;
      h->GetYaxis()->SetRangeUser(0,1024);
      h->Draw("colz");
    }
    c->Update();
  }

  // Draw Waveform w/ Leading
  for (Int_t n =0; n<4; n++){
    TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", (n+1) + 4));
    c->Clear();
    c->Divide(3,3);
    Int_t fadc_wl_id     = HistMaker::getUniqueID(kRAYRAW, 0, kFADCwTDC, 0);
    for( Int_t i=0; i<8; ++i ){
      c->cd(i+1);
      gPad->SetLogz();
      TH1 *h = (TH1*)GHist::get( fadc_wl_id + i + n*8);
      if( !h ) continue;
      h->GetYaxis()->SetRangeUser(0,1024);
      h->Draw("colz");
    }
    c->Update();
  }

  // Draw Waveform w/o Leading
  for (Int_t n =0; n<2; n++){
    TCanvas *c = (TCanvas*)gROOT->FindObject(Form("c%d", (n+1) + 8));
    c->Clear();
    c->Divide(4,4);
    Int_t fadc_wol_id     = HistMaker::getUniqueID(kRAYRAW, 0, kADCwTDC, 0);
    for( Int_t i=0; i<16; ++i ){
      c->cd(i+1);
      gPad->SetLogz();
      TH1 *h = (TH1*)GHist::get( fadc_wol_id + i + n*16);
      if( !h ) continue;
      h->GetYaxis()->SetRangeUser(0,1024);
      h->Draw("colz");
    }
    c->Update();
  }

  // // draw Waveform for ASIC1
  // {
  //   TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
  //   c->Clear();
  //   c->Divide(3,3);
  //   Int_t fadc_id     = HistMaker::getUniqueID(kRAYRAW, 0, kFADC, 0);
  //   for( Int_t i=0; i<8; ++i ){
  //     c->cd(i+1);
  //     gPad->SetLogz();
  //     TH1 *h = (TH1*)GHist::get( fadc_id + i );
  //     if( !h ) continue;
  //     h->GetYaxis()->SetRangeUser(0,1024);
  //     h->Draw("colz");
  //   }
  //   c->Update();
  // }

  // // draw Waveform for ASIC2
  // {
  //   TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
  //   c->Clear();
  //   c->Divide(3,3);
  //   Int_t fadc_id     = HistMaker::getUniqueID(kRAYRAW, 0, kFADC, 0);
  //   for( Int_t i=0; i<8; ++i ){
  //     c->cd(i+1);
  //     gPad->SetLogz();
  //     TH1 *h = (TH1*)GHist::get( fadc_id + i + 8);
  //     if( !h ) continue;
  //     h->GetYaxis()->SetRangeUser(0,1024);
  //     h->Draw("colz");
  //   }
  //   c->Update();
  // }

  // // draw Waveform for ASIC3
  // {
  //   TCanvas *c = (TCanvas*)gROOT->FindObject("c3");
  //   c->Clear();
  //   c->Divide(3,3);
  //   Int_t fadc_id     = HistMaker::getUniqueID(kRAYRAW, 0, kFADC, 0);
  //   for( Int_t i=0; i<8; ++i ){
  //     c->cd(i+1);
  //     gPad->SetLogz();
  //     TH1 *h = (TH1*)GHist::get( fadc_id + i + 16);
  //     if( !h ) continue;
  //     h->GetYaxis()->SetRangeUser(0,1024);
  //     h->Draw("colz");
  //   }
  //   c->Update();
  // }

  // // draw Waveform for ASIC4
  // {
  //   TCanvas *c = (TCanvas*)gROOT->FindObject("c4");
  //   c->Clear();
  //   c->Divide(3,3);
  //   Int_t fadc_id     = HistMaker::getUniqueID(kRAYRAW, 0, kFADC, 0);
  //   for( Int_t i=0; i<8; ++i ){
  //     c->cd(i+1);
  //     gPad->SetLogz();
  //     TH1 *h = (TH1*)GHist::get( fadc_id + i + 24);
  //     if( !h ) continue;
  //     h->GetYaxis()->SetRangeUser(0,1024);
  //     h->Draw("colz");
  //   }
  //   c->Update();
  // }

  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
