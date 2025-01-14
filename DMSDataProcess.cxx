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
  fTree->Branch("lDet1PulseFlag", &fDet1PulseFound, "lDet1PulseFlag/O");
  fTree->Branch("lDet2PulseFlag", &fDet2PulseFound, "lDet2PulseFlag/O");
  fTree->Branch("lDet1PulsePeak", &fDet1PulsePeak, "lDet1PulsePeak/F");
  fTree->Branch("lDet2PulsePeak", &fDet2PulsePeak, "lDet2PulsePeak/F");
  fTree->Branch("lDet1PulseStart", &fDet1PulseStart, "lDet1PulseStart/F");
  fTree->Branch("lDet1PulseTail", &fDet1PulseTail, "lDet1PulseTail/F");
  fTree->Branch("lDet1PulseEnd", &fDet1PulseEnd, "lDet1PulseEnd/F");
  fTree->Branch("lDet2PulseStart", &fDet2PulseStart, "lDet2PulseStart/F");
  fTree->Branch("lDet2PulseTail", &fDet2PulseTail, "lDet2PulseTail/F");
  fTree->Branch("lDet2PulseEnd", &fDet2PulseEnd, "lDet2PulseEnd/F");
  fTree->Branch("lDet1PulseIntegralTotal", &fDet1PulseIntegralTotal, "lDet1PulseIntegralTotal/F");
  fTree->Branch("lDet1PulseIntegralTail", &fDet1PulseIntegralTail, "lDet1PulseIntegralTail/F");
  fTree->Branch("lDet2PulseIntegralTotal", &fDet2PulseIntegralTotal, "lDet2PulseIntegralTotal/F");
  fTree->Branch("lDet2PulseIntegralTail", &fDet2PulseIntegralTail, "lDet2PulseIntegralTail/F");
  fTree->Branch("lDet1PSD", &fDet1PSD, "lDet1PSD/F");
  fTree->Branch("lDet2PSD", &fDet2PSD, "lDet2PSD/F");

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
      // read a single event
      while( *fInputStream[f] >> fAdcValue[f][fChannelNumber] )
      {
        fChannelNumber++;
        //std::cout << "Event: " << fEventNumber << ", File Index: " << f << ", ADC value at Channel (" << fChannelNumber - 1 << "): " << fAdcValue[f][fChannelNumber-1] << std::endl;
        if( fChannelNumber == 1024 )
        {
          // do calculations here:
          
          
          // end of event calculation
          fTree->Fill();
          break;
        }
      }
    } // file loop ends
    fEventNumber++;
  } // event loop ends

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
