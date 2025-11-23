// Updater belongs to the namespace hddaq::gui
using namespace hddaq::gui;

#include "UserParamMan.hh"

void dispRAYRAW()
{
  const UserParamMan& gUser = UserParamMan::GetInstance();
  // You must write these lines for the thread safe
  // ----------------------------------
  if(Updater::isUpdating()){return;}
  Updater::setUpdating(true);
  // ----------------------------------

  // draw ADC for ASIC1
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c1");
    c->Clear();
    c->Divide(3,3);
    Int_t adc_id     = HistMaker::getUniqueID(kRAYRAW, 0, kADC, 0);
    //    Int_t adcwtdc_id = HistMaker::getUniqueID(kRAYRAW, 0, kADCwTDC, 0);
    for( Int_t i=0; i<8; ++i ){
      c->cd(i+1);
      //      gPad->SetLogy();
      TH1 *h = (TH1*)GHist::get( adc_id + i );
      if( !h ) continue;
      //      h->GetXaxis()->SetRangeUser(0,1024);
      h->Draw();
      // TH1 *hh = (TH1*)GHist::get( adcwtdc_id + i );
      // if( !hh ) continue;
      // hh->GetXaxis()->SetRangeUser(0,1024);
      // hh->SetLineColor( kRed );
      // hh->Draw("same");
    }
    c->Update();
  }

  // draw ADC for ASIC2
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c2");
    c->Clear();
    c->Divide(3,3);
    Int_t adc_id     = HistMaker::getUniqueID(kRAYRAW, 0, kADC, 0);
    //    Int_t adcwtdc_id = HistMaker::getUniqueID(kRAYRAW, 0, kADCwTDC, 0);
    for( Int_t i=0; i<8; ++i ){
      c->cd(i+1);
      //      gPad->SetLogy();
      TH1 *h = (TH1*)GHist::get( adc_id + i + 8);
      if( !h ) continue;
      //      h->GetXaxis()->SetRangeUser(0,1024);
      h->Draw();
      // TH1 *hh = (TH1*)GHist::get( adcwtdc_id + i );
      // if( !hh ) continue;
      // hh->GetXaxis()->SetRangeUser(0,1024);
      // hh->SetLineColor( kRed );
      // hh->Draw("same");
    }
    c->Update();
  }

  // draw ADC for ASIC3
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c3");
    c->Clear();
    c->Divide(3,3);
    Int_t adc_id     = HistMaker::getUniqueID(kRAYRAW, 0, kADC, 0);
    //    Int_t adcwtdc_id = HistMaker::getUniqueID(kRAYRAW, 0, kADCwTDC, 0);
    for( Int_t i=0; i<8; ++i ){
      c->cd(i+1);
      //      gPad->SetLogy();
      TH1 *h = (TH1*)GHist::get( adc_id + i + 16);
      if( !h ) continue;
      //      h->GetXaxis()->SetRangeUser(0,1024);
      h->Draw();
      // TH1 *hh = (TH1*)GHist::get( adcwtdc_id + i );
      // if( !hh ) continue;
      // hh->GetXaxis()->SetRangeUser(0,1024);
      // hh->SetLineColor( kRed );
      // hh->Draw("same");
    }
    c->Update();
  }

  // draw ADC for ASIC4
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c4");
    c->Clear();
    c->Divide(3,3);
    Int_t adc_id     = HistMaker::getUniqueID(kRAYRAW, 0, kADC, 0);
    //    Int_t adcwtdc_id = HistMaker::getUniqueID(kRAYRAW, 0, kADCwTDC, 0);
    for( Int_t i=0; i<8; ++i ){
      c->cd(i+1);
      //      gPad->SetLogy();
      TH1 *h = (TH1*)GHist::get( adc_id + i + 24);
      if( !h ) continue;
      //      h->GetXaxis()->SetRangeUser(0,1024);
      h->Draw();
      // TH1 *hh = (TH1*)GHist::get( adcwtdc_id + i );
      // if( !hh ) continue;
      // hh->GetXaxis()->SetRangeUser(0,1024);
      // hh->SetLineColor( kRed );
      // hh->Draw("same");
    }
    c->Update();
  }

  // // TDC gate range
  // static const unsigned int tdc_min = gUser.GetParameter("TdcBH1", 0);
  // static const unsigned int tdc_max = gUser.GetParameter("TdcBH1", 1);

  // draw TDC for ASIC1
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c5");
    c->Clear();
    c->Divide(3,3);
    Int_t tdc_l_id = HistMaker::getUniqueID(kRAYRAW, 0, kTDC, 0);
    Int_t tdc_t_id = HistMaker::getUniqueID(kRAYRAW, 0, kTOT, 0);
    for( Int_t i=0; i<8; ++i ){
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get( tdc_l_id + i );
      if( !h ) continue;
      // h->GetXaxis()->SetRangeUser(tdc_min,tdc_max);
      h->Draw();

      TH1 *hh = (TH1*)GHist::get( tdc_t_id + i );
      if( !hh ) continue;
      // hh->GetXaxis()->SetRangeUser(0,1024);
      hh->SetLineColor( kRed );
      hh->Draw("same");
    }
    c->Update();
  }

  // draw TDC for ASIC2
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c6");
    c->Clear();
    c->Divide(3,3);
    Int_t tdc_l_id = HistMaker::getUniqueID(kRAYRAW, 0, kTDC, 0);
    Int_t tdc_t_id = HistMaker::getUniqueID(kRAYRAW, 0, kTOT, 0);
    for( Int_t i=0; i<8; ++i ){
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get( tdc_l_id + i + 8 );
      if( !h ) continue;
      // h->GetXaxis()->SetRangeUser(tdc_min,tdc_max);
      h->Draw();

      TH1 *hh = (TH1*)GHist::get( tdc_t_id + i + 8);
      if( !hh ) continue;
      // hh->GetXaxis()->SetRangeUser(0,1024);
      hh->SetLineColor( kRed );
      hh->Draw("same");
    }
    c->Update();
  }

  // draw TDC for ASIC3
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c7");
    c->Clear();
    c->Divide(3,3);
    Int_t tdc_l_id = HistMaker::getUniqueID(kRAYRAW, 0, kTDC, 0);
    Int_t tdc_t_id = HistMaker::getUniqueID(kRAYRAW, 0, kTOT, 0);
    for( Int_t i=0; i<8; ++i ){
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get( tdc_l_id + i + 16 );
      if( !h ) continue;
      // h->GetXaxis()->SetRangeUser(tdc_min,tdc_max);
      h->Draw();

      TH1 *hh = (TH1*)GHist::get( tdc_t_id + i + 16);
      if( !hh ) continue;
      // hh->GetXaxis()->SetRangeUser(0,1024);
      hh->SetLineColor( kRed );
      hh->Draw("same");
    }
    c->Update();
  }

  // draw TDC for ASIC4
  {
    TCanvas *c = (TCanvas*)gROOT->FindObject("c8");
    c->Clear();
    c->Divide(3,3);
    Int_t tdc_l_id = HistMaker::getUniqueID(kRAYRAW, 0, kTDC, 0);
    Int_t tdc_t_id = HistMaker::getUniqueID(kRAYRAW, 0, kTOT, 0);
    for( Int_t i=0; i<8; ++i ){
      c->cd(i+1);
      TH1 *h = (TH1*)GHist::get( tdc_l_id + i + 24 );
      if( !h ) continue;
      // h->GetXaxis()->SetRangeUser(tdc_min,tdc_max);
      h->Draw();

      TH1 *hh = (TH1*)GHist::get( tdc_t_id + i + 24);
      if( !hh ) continue;
      // hh->GetXaxis()->SetRangeUser(0,1024);
      hh->SetLineColor( kRed );
      hh->Draw("same");
    }
    c->Update();
  }


  // You must write these lines for the thread safe
  // ----------------------------------
  Updater::setUpdating(false);
  // ----------------------------------
}
