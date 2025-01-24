#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <fcntl.h>
#include <unistd.h>

#include <TMath.h>

#include "DMSDataProcess.h"
#include "DMSMathUtils.h"

DMSDataProcess::DMSDataProcess()
{
}

DMSDataProcess::DMSDataProcess(char* pInputDirectory, char* pOutputFile)
{// {{{
  fEventNumber = 1;
  fChannelNumber = 0;
  fInputDirectory = new char[strlen(pInputDirectory)+1];
  strcpy(fInputDirectory, pInputDirectory);
  std::cout << fInputDirectory << std::endl;
  const char* filesToCheck[4] = { "wave_0.txt",
                                  "wave_1.txt",
                                  "wave_6.txt",
                                  "wave_7.txt" };
  char* fFullPathsToFiles[4];
  for(int i = 0; i < 4; ++i)
  {
    fFullPathsToFiles[i] = new char[strlen(fInputDirectory) + strlen(filesToCheck[i])];
    strcpy(fFullPathsToFiles[i], fInputDirectory);
    strcat(fFullPathsToFiles[i], filesToCheck[i]);
    std::cout << "Full path to file: " << fFullPathsToFiles[i] << std::endl;

    fInputStream[i] = new std::ifstream(fFullPathsToFiles[i]);
    if( fInputStream[i]->is_open() )
      std::cout << fFullPathsToFiles[i] << " has been opened!" << std::endl;
  }

  fOutputFileName = pOutputFile;
	fMaxEventNumber = CountLinesInText()/1024;
  std::cout << "Total number of events to process: " << fMaxEventNumber << std::endl;
  fCherenkovInner = kFALSE;
  fCherenkovOuter = kFALSE;
  fOutputFile = new TFile(fOutputFileName, "RECREATE");
  fTree = new TTree("DMSNtuple", "DAMSA Ntuple");
  fTree->Branch("lCherenkovInner", &fCherenkovInner, "lCherenkovInner/I");
  fTree->Branch("lCherenkovOuter", &fCherenkovOuter, "lCherenkovOuter/I");
  fTree->Branch("lPreBaseline", fPreBaseline, "lPreBaseline[4]/F");
  fTree->Branch("lPreBaseline_RMS", fPreBaseline_RMS, "lPreBaseline_RMS[4]/F");
  fTree->Branch("lPostBaseline", fPostBaseline, "lPostBaseline[4]/F");
  fTree->Branch("lPostBaseline_RMS", fPostBaseline_RMS, "lPostBaseline_RMS[4]/F");
  fTree->Branch("lBaselineDifference", fBaselineDifference, "lBaselineDifference[4]/F");
  fTree->Branch("lPulseFlag", fPulseFlag, "lPulseFlag[4]/I");
  fTree->Branch("lPulsePeak", fPulsePeak, "lPulsePeak[4]/F");
  fTree->Branch("lPulsePeakTimeBin", fPulsePeakTimeBin, "lPulsePeakTimeBin[4]/I");
  fTree->Branch("lPulseStart", &fPulseStartBin, "lPulseStartBin[4]/F");
  fTree->Branch("lPulseTail", &fPulseTailBin, "lPulseTailBin[4]/F");
  fTree->Branch("lPulseEnd", &fPulseEndBin, "lPulseEndBin[4]/F");
  fTree->Branch("lPulseIntegralTotal", fPulseIntegralTotal, "lPulseIntegralTotal[4]/F");
  fTree->Branch("lPulseIntegralTail", fPulseIntegralTail, "lPulseIntegralTail[4]/F");
  fTree->Branch("lPSD", fPSD, "lPSD[4]/F");

  fPreBaseSearch_i[0] = 0;
  fPreBaseSearch_f[0] = 20;
  fPreBaseSearchWidth[0] = fPreBaseSearch_f[0] - fPreBaseSearch_i[0];
  fPostBaseSearch_i[0] = 980;
  fPostBaseSearch_f[0] = 1000;
  fPostBaseSearchWidth[0] = fPostBaseSearch_f[0] - fPostBaseSearch_i[0];
  fPrePulseSearch_i[0] = 300;
  fPrePulseSearch_f[0] = 600;
  fPostPulseSearch_i[0] = 800;
  fPostPulseSearch_f[0] = 1000;

  fPreBaseSearch_i[1] = 0;
  fPreBaseSearch_f[1] = 20;
  fPreBaseSearchWidth[1] = fPreBaseSearch_f[1] - fPreBaseSearch_i[1];
  fPostBaseSearch_i[1] = 980;
  fPostBaseSearch_f[1] = 1000;
  fPostBaseSearchWidth[1] = fPostBaseSearch_f[1] - fPostBaseSearch_i[1];
  fPrePulseSearch_i[1] = 0;
  fPrePulseSearch_f[1] = 400;
  fPostPulseSearch_i[1] = 400;
  fPostPulseSearch_f[1] = 1000;

  fPreBaseSearch_i[2] = 0;
  fPreBaseSearch_f[2] = 20;
  fPreBaseSearchWidth[2] = fPreBaseSearch_f[2] - fPreBaseSearch_i[2];
  fPostBaseSearch_i[2] = 980;
  fPostBaseSearch_f[2] = 1000;
  fPostBaseSearchWidth[2] = fPostBaseSearch_f[2] - fPostBaseSearch_i[2];
  fPrePulseSearch_i[2] = 0;
  fPrePulseSearch_f[2] = 400;
  fPostPulseSearch_i[2] = 400;
  fPostPulseSearch_f[2] = 1000;

  fPreBaseSearch_i[3] = 300;
  fPreBaseSearch_f[3] = 600;
  fPreBaseSearchWidth[3] = fPreBaseSearch_f[3] - fPreBaseSearch_i[3];
  fPostBaseSearch_i[3] = 980;
  fPostBaseSearch_f[3] = 1000;
  fPostBaseSearchWidth[3] = fPostBaseSearch_f[3] - fPostBaseSearch_i[3];
  fPrePulseSearch_i[3] = 0;
  fPrePulseSearch_f[3] = 400;
  fPostPulseSearch_i[3] = 400;
  fPostPulseSearch_f[3] = 1000;
}// }}}

DMSDataProcess::~DMSDataProcess() {
  delete fInputDirectory;
  delete fTree;
  delete fOutputFile;
  for(int i = 0; i < 4; i++)
  {
    delete fFullPathsToFiles[i];
    fInputStream[i]->close();
    delete fInputStream[i];
  }
}

void DMSDataProcess::ProcessFile()
{// {{{
  std::cout << "ProcessFile()" << std::endl;
  // variables for progress indicator
  Char_t symbols[] = {'-', '/', '|', '\\'};
  Int_t symbolIndex = 0;
  ULong64_t currentProgress = 0;
  ULong64_t newProgress = 0;

  const float constFractionRate = 0.05f;
  const float tailFractionRate = 0.30f;

  // loop over events
  while( fEventNumber < fMaxEventNumber )
  {
    // progress indicator
    newProgress = ( fEventNumber * 100 ) / fMaxEventNumber;
    if( newProgress > currentProgress )
    {
      currentProgress = newProgress;
      // Print rotator
      std::cout << '\r'
        << symbols[symbolIndex]
        << " Progressing: " << std::setw(3) << currentProgress << "% ("
        << fEventNumber << "/" << fMaxEventNumber << ")"
        << std::flush;
      symbolIndex = ( symbolIndex + 1 ) % 4;
    }

    // loop over files
    for(int f = 0; f < 4; ++f)  // loop over file index f = 0, 1, 2, 3; 0: wave_0.txt 1: wave_1.txt 2: wave_6.txt 3: wave_7.txt
    {
      // initialize variables
      fChannelNumber = 0;
      fPulsePeak[f] = 9999.f;
      fPulseStartBin[f] = -1;
      fPulseTailBin[f] = -1;
      fPulseEndBin[f] = -1;
      fPulseFlag[f] = 0;
      // read a single event
      while( *fInputStream[f] >> fAdcValue[f][fChannelNumber] )
      {
        fChannelNumber++;
        // find minimum value which is pulse peak candidate
        if( fAdcValue[f][fChannelNumber-1] < fPulsePeak[f] )
        {
          fPulsePeak[f] = fAdcValue[f][fChannelNumber-1];
          fPulsePeakTimeBin[f] = fChannelNumber-1;
        }

        //std::cout << "Event: " << fEventNumber << ", File Index: " << f << ", ADC value at Channel (" << fChannelNumber - 1 << "): " << fAdcValue[f][fChannelNumber-1] << std::endl;
        if( fChannelNumber == 1024 )
        {
          fPreBaseline[f] = 0.0;
          // do calculations here:
          for( int i = fPreBaseSearch_i[f]; i < fPreBaseSearch_f[f]; ++i )
          {
            fPreBaseline[f] += fAdcValue[f][i];
            fPreBaseline_RMS[f] += fAdcValue[f][i] * fAdcValue[f][i];
          }
          fPreBaseline[f] /= (Float_t)fPreBaseSearchWidth[f];
          fPreBaseline_RMS[f] = sqrt( fPreBaseline_RMS[f]/fPreBaseSearchWidth[f] - fPreBaseline[f] * fPreBaseline[f]);

          fPostBaseline[f] = 0.0;
          for( int i = fPostBaseSearch_i[f]; i < fPostBaseSearch_f[f]; ++i )
          {
            fPostBaseline[f] += fAdcValue[f][i];
            fPostBaseline_RMS[f] += fAdcValue[f][i] * fAdcValue[f][i];
          }
          fPostBaseline[f] /= (Float_t)fPostBaseSearchWidth[f];
          fPostBaseline_RMS[f] = sqrt( fPostBaseline_RMS[f]/fPostBaseSearchWidth[f] - fPostBaseline[f] * fPostBaseline[f] );

          // determine the baseline
          fBaseline[f] = ( fPostBaseline[f] > fPreBaseline[f] ) ? fPostBaseline[f] : fPreBaseline[f];
          // determine the peak height
          fPulsePeak[f] = fBaseline[f] - fPulsePeak[f];
          if( fPulsePeak[f] < 15 )
          {
            fPulseFlag[f] = 0;
          }
          else
            fPulseFlag[f] = 1;

          // find pulse range
          if( f == 2 || f == 3 ) // for cherenkov signals
          { // set cherenkov peak search area
            fPrePulseSearch_i[f] = fPulsePeakTimeBin[f] - 50;
            fPrePulseSearch_f[f] = fPulsePeakTimeBin[f];
            fPostPulseSearch_i[f] = fPulsePeakTimeBin[f] + 1;
            fPostPulseSearch_f[f] = fPulsePeakTimeBin[f] + 50;
          }
          // pulse start
          for( int i = fPrePulseSearch_i[f]; i < fPrePulseSearch_f[f]; ++i )
          {
            if( fPulseFlag[f] == 0 ) break;
            if( fAdcValue[f][i] < fPreBaseline[f] - fPulsePeak[f] * constFractionRate && fPulseStartBin[f] == -1 )
              fPulseStartBin[f] = i;
          }

          // pulse end
          for( int i = fPostPulseSearch_i[f]; i < fPostPulseSearch_f[f]; ++i )
          {
            if( fPulseFlag[f] == 0 ) break;
            if( fAdcValue[f][i] > fPostBaseline[f] - fPulsePeak[f] * constFractionRate && fPulseEndBin[f] == -1 )
              fPulseEndBin[f] = i;
          }

          // find the pulse tail time
          //  --> replace this by constant bins from the pulse start
          /*
          for( int i = fPulseEndBin[f]; i > fPulsePeakTimeBin[f]; --i )
          {
            if( fPulseFlag[f] == 0 ) break;
            float signal = fBaseline[f] - fAdcValue[f][i];
            if( signal > fPulsePeak[f] * tailFractionRate )
            {
              fPulseTailBin[f] = i;
              break;
            }
          }
          */
          fPulseTailBin[f] = fPulseStartBin[f] + 500;

          // integrate charge
          fPulseIntegralTotal[f] = 0.0f;
          fPulseIntegralTail[f]  = 0.0f;
          fPSD[f] = 0.0f;
          for( int i = fPulseStartBin[f]; i < fPulseEndBin[f]; ++i )
          {
            if( fPulseFlag[f] == 0 ) break;
            float signal = fBaseline[f] - fAdcValue[f][i];
            fPulseIntegralTotal[f] += signal;
            if( i > fPulseTailBin[f] )
              fPulseIntegralTail[f] += signal;
          }
          fPSD[f] = fPulseIntegralTail[f]/fPulseIntegralTotal[f];

          /*
          std::cout << "Event: " << fEventNumber << ", File: " << f << ", baseline: " << fPreBaseline[f] << std::endl;
          std::cout << "Event: " << fEventNumber << ", File: " << f << ", baseline: " << fPreBaseline_RMS[f] << std::endl;
          std::cout << "Event: " << fEventNumber << ", File: " << f << ", baseline: " << fPostBaseline[f] << std::endl;
          std::cout << "Event: " << fEventNumber << ", File: " << f << ", baseline: " << fPostBaseline_RMS[f] << std::endl;
          std::cout << "Event: " << fEventNumber << ", File: " << f << ", pulse minimum: " << fPulsePeak[f] << std::endl;
          if(fPulsePeak[f] == 0)  std::cout << "Event: " << fEventNumber << ", -- No Pulse -- " << fPulsePeak[f] << std::endl;
          std::cout << "Event: " << fEventNumber << ", File: " << f << ", pulse peak: " << fPulsePeak[f] << std::endl;
          std::cout << "Event: " << fEventNumber << ", File: " << f << ", pulse time: " << fPulsePeakTimeBin[f] << std::endl;
          std::cout << "Event: " << fEventNumber << ", File: " << f << ", pulse start threshold: " << fPreBaseline[f] - fPulsePeak[f] * constFractionRate << std::endl;
          std::cout << "Event: " << fEventNumber << ", File: " << f << ", pulse start bin: " << fPulseStartBin[f] << std::endl;
          std::cout << "Event: " << fEventNumber << ", File: " << f << ", pulse end threshold: " << fPostBaseline[f] - fPulsePeak[f] * constFractionRate << std::endl;
          std::cout << "Event: " << fEventNumber << ", File: " << f << ", pulse end bin: " << fPulseEndBin[f] << std::endl;
          std::cout << "Event: " << fEventNumber << ", File: " << f << ", pulse tail bin: " << fPulseTailBin[f] << std::endl;
          std::cout << "Event: " << fEventNumber << ", File: " << f << ", Integral(Total): " << fPulseIntegralTotal[f] << std::endl;
          std::cout << "Event: " << fEventNumber << ", File: " << f << ", Integral(Tail): " << fPulseIntegralTail[f] << std::endl;
          std::cout << "Event: " << fEventNumber << ", File: " << f << ", PSD: " << fPSD[f] << std::endl;

          std::cout << std::endl;
          */

          // end of event calculation
          fTree->Fill();
          break;
        }
      }
    } // file loop ends
    fEventNumber++;
    //if( fEventNumber == 3 ) break;
  } // event loop ends

  fTree->Write();
  fOutputFile->Close();
  std::cout << "Writing " << fOutputFile->GetName() << " has been done!" << std::endl;

}// }}}

void DMSDataProcess::CheckCherenkovSignals()
{// {{{
/*
  Float_t fCherenkovSearchStart = 150.0f;
  Float_t fCherenkovSearchEnd   = 200.0f;
  Int_t che1StartBin = fChe1HistogramKDE->FindBin(fCherenkovSearchStart);
  Int_t che1EndBin   = fChe1HistogramKDE->FindBin(fCherenkovSearchEnd);

  Float_t fChe1Minimum;
  Float_t fChe2Minimum;
  Float_t che1_value;
  Float_t che1_minimum = fChe1PreBaseline;
  Float_t che2_value;
  Float_t che2_minimum = fChe2PreBaseline;

  for(Int_t i = che1StartBin; i <= che1EndBin; ++i )
  {
    che1_value = fChe1HistogramKDE->GetBinContent(i);
    che2_value = fChe2HistogramKDE->GetBinContent(i);
    if( che1_value < che1_minimum )
      che1_minimum = che1_value;
    if( che2_value < che2_minimum )
      che2_minimum = che2_value;
  }

  if( che1_minimum < fChe1PreBaseline - 20.0f )
  {
    fCherenkovInner = kTRUE;
  }
  if( che2_minimum < fChe2PreBaseline - 20.0f )
  {
    fCherenkovOuter = kTRUE;
  }
  */
}// }}}

void DMSDataProcess::FindPulsePeak()
{// {{{
/*
  Float_t det1_value;
  Float_t det1_minimum = fDet1PreBaseline;
  Float_t che2_value;
  Float_t che2_minimum = fDet2PreBaseline;
  Float_t det1_minimum_time;
  Float_t che2_minimum_time;

  for( Int_t i = fDet1PulseStartBin; i <= fDet1PulseEndBin; ++i )
  {
    det1_value = fChe1HistogramKDE->GetBinContent(i);
    if( det1_value < det1_minimum )
    {
      det1_minimum = det1_value;
      det1_minimum_time = fChe1HistogramKDE->GetBinCenter(i);
    }
    che2_value = fChe2HistogramKDE->GetBinContent(i);
    if( che2_value < che2_minimum )
    {
      che2_minimum = che2_value;
      che2_minimum_time = fChe2HistogramKDE->GetBinCenter(i);
    }
  }
  fDet1PulsePeak = det1_minimum;
  fDet2PulsePeak = che2_minimum;
  fDet1PulsePeakTime = det1_minimum_time;
  fDet2PulsePeakTime = che2_minimum_time;
  */
}// }}}

void DMSDataProcess::FindPulseTail()
{// {{{
/*
  Int_t det1_peak_bin = fDet1HistogramKDE->FindBin(fDet1PulsePeakTime);
  Int_t det2_peak_bin = fDet2HistogramKDE->FindBin(fDet2PulsePeakTime);
  Float_t det1_value;
  Float_t det2_value;
  Float_t det1_inverted_value;
  Float_t det2_inverted_value;
  Float_t det1_tail_threshold = (fDet1PreBaseline - fDet1PulsePeak) * 0.3f;
  Float_t det2_tail_threshold = (fDet2PreBaseline - fDet2PulsePeak) * 0.3f;

  for( Int_t i = det1_peak_bin; i <= fDet1PulseEndBin; ++i )
  {
    det1_value = fDet1HistogramKDE->GetBinContent(i);
    det1_inverted_value = fDet1PreBaseline - det1_value;

    if( det1_inverted_value < det1_tail_threshold )
    {
      fDet1PulseTail = fDet1HistogramKDE->GetBinCenter(i);
      fDet1PulseTailBin = i;
      break;
    }
  }
  for( Int_t i = det2_peak_bin; i <= fDet2PulseEndBin; ++i )
  {
    det2_value = fDet2HistogramKDE->GetBinContent(i);
    det2_inverted_value = fDet1PreBaseline - det1_value;

    if( det2_inverted_value < det2_tail_threshold )
    {
      fDet2PulseTail = fDet2HistogramKDE->GetBinCenter(i);
      fDet2PulseTailBin = i;
      break;
    }
  }
  */
}// }}}

ULong64_t DMSDataProcess::CountLinesInText()
{// {{{
  std::string fileToCheck(fInputDirectory);
  fileToCheck += "wave_0.txt";
  int fd = open(fileToCheck.c_str(), O_RDONLY);
  if (fd == -1) {
    perror("Error opening file");
    return -1;
  }

  char buffer[4096]; // 블록 크기
  ssize_t bytesRead;
  long lineCount = 0;

  while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0) {
    for (ssize_t i = 0; i < bytesRead; ++i) {
      if (buffer[i] == '\n') {
        ++lineCount;
      }
    }
  }

  if (bytesRead == -1) {
    perror("Error reading file");
    close(fd);
    return -1;
  }

  close(fd);
  return lineCount;
}// }}}
