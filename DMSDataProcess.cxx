#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <fcntl.h>
#include <unistd.h>
#include <TMath.h>
#include "DMSDataProcess.h"

DMSDataProcess::DMSDataProcess() {}

DMSDataProcess::DMSDataProcess(const char* pInputFile, const char* pOutputFile) {
  fEventNumber = 1;
  fChannelNumber = 0;

  fInputFilePath = std::string(pInputFile);
  fOutputFileName = std::string(pOutputFile);

  fInputStream = new std::ifstream(fInputFilePath);
  if (!fInputStream->is_open()) {
    std::cerr << "Error opening file: " << fInputFilePath << std::endl;
    exit(1);
  }

  fMaxEventNumber = CountLinesInText() / 1024;
  std::cout << "Total number of events to process: " << fMaxEventNumber << std::endl;

  fOutputFile = new TFile(fOutputFileName.c_str(), "RECREATE");
  fTree = new TTree("DMSNtuple", "DAMSA Ntuple");

  fTree->Branch("lPulseFlag", &fPulseFlag, "lPulseFlag/I");
  fTree->Branch("lPulsePeak", &fPulsePeak, "lPulsePeak/F");
  fTree->Branch("lPulsePeakTimeBin", &fPulsePeakTimeBin, "lPulsePeakTimeBin/I");
  fTree->Branch("lPulseStartBin", &fPulseStartBin, "lPulseStartBin/I");
  fTree->Branch("lPulseTailBin", &fPulseTailBin, "lPulseTailBin/I");
  fTree->Branch("lPulseEndBin", &fPulseEndBin, "lPulseEndBin/I");
  fTree->Branch("lPulseIntegralTotal", &fPulseIntegralTotal, "lPulseIntegralTotal/F");
  fTree->Branch("lPulseIntegralTail", &fPulseIntegralTail, "lPulseIntegralTail/F");
  fTree->Branch("lPSD", &fPSD, "lPSD/F");

  fPreBaseSearch_i = 0;
  fPreBaseSearch_f = 20;
  fPostBaseSearch_i = 980;
  fPostBaseSearch_f = 1000;
  fPrePulseSearch_i = 300;
  fPrePulseSearch_f = 600;
  fPostPulseSearch_i = 800;
  fPostPulseSearch_f = 1000;
}

DMSDataProcess::~DMSDataProcess() {
  delete fTree;
  delete fOutputFile;
  fInputStream->close();
  delete fInputStream;
}

void DMSDataProcess::ProcessFile() {
  std::cout << "ProcessFile()" << std::endl;

  Char_t symbols[] = {'-', '/', '|', '\\'};
  Int_t symbolIndex = 0;
  ULong64_t currentProgress = 0;
  const float adc_cut = 30.0f;
  const int tail_offset = 270;
  const int tail_width = 350;

  while (fEventNumber < fMaxEventNumber) {
    ULong64_t newProgress = (fEventNumber * 100) / fMaxEventNumber;
    if (newProgress > currentProgress) {
      currentProgress = newProgress;
      std::cout << "\r" << symbols[symbolIndex]
                << " Progressing: " << std::setw(3) << currentProgress << "% ("
                << fEventNumber << "/" << fMaxEventNumber << ")"
                << std::flush;
      symbolIndex = (symbolIndex + 1) % 4;
    }

    fPulsePeak = 9999.f;
    fPulseStartBin = -1;
    fPulseTailBin = -1;
    fPulseEndBin = -1;
    fPulseFlag = 0;
    fPulseIntegralTotal = 0.0f;
    fPulseIntegralTail = 0.0f;
    fPSD = 0.0f;

    fChannelNumber = 0;
    while (fChannelNumber < 1024 && *fInputStream >> fAdcValue[fChannelNumber]) {
      if (fAdcValue[fChannelNumber] < fPulsePeak) {
        fPulsePeak = fAdcValue[fChannelNumber];
        fPulsePeakTimeBin = fChannelNumber;
      }
      fChannelNumber++;

      if (fChannelNumber == 1024) {
        float preBaseline = 0.0, postBaseline = 0.0;
        for (int i = fPreBaseSearch_i; i < fPreBaseSearch_f; ++i)
          preBaseline += fAdcValue[i];
        preBaseline /= (fPreBaseSearch_f - fPreBaseSearch_i);

        for (int i = fPostBaseSearch_i; i < fPostBaseSearch_f; ++i)
          postBaseline += fAdcValue[i];
        postBaseline /= (fPostBaseSearch_f - fPostBaseSearch_i);

	float baseline = 0.5 * (preBaseline + postBaseline);
	//float baseline = std::max(preBaseline, postBaseline);


        fPulsePeak = baseline - fPulsePeak;

fPulseFlag = (fPulsePeak >= adc_cut) ? 1 : 0;

        for (int i = 10; i < 1024; ++i) {
          float diff = baseline - fAdcValue[i];
          if (diff > adc_cut && fPulseStartBin == -1)
            fPulseStartBin = i;
        }

        if (!fPulseFlag || fPulseStartBin == -1)
          continue;

        fPulseTailBin = fPulseStartBin + tail_offset;
        //fPulseEndBin = fPulseTailBin + tail_width;
        fPulseEndBin = 1024;

        for (int i = fPulseStartBin; i < fPulseEndBin; ++i) {
          float signal = baseline - fAdcValue[i];
          fPulseIntegralTotal += signal;
          if (i >= fPulseTailBin)
            fPulseIntegralTail += signal;
        }

        fPSD = (fPulseIntegralTotal > 0.0f)
                 ? fPulseIntegralTail / fPulseIntegralTotal
                 : 0.0f;

        fTree->Fill();
        break;
      }
    }
    fEventNumber++;
  }

  fTree->Write();
  fOutputFile->Close();
  std::cout << "\nWriting " << fOutputFile->GetName() << " has been done!" << std::endl;
}

ULong64_t DMSDataProcess::CountLinesInText() {
  int fd = open(fInputFilePath.c_str(), O_RDONLY);
  if (fd == -1) {
    perror("Error opening file");
    return -1;
  }

  char buffer[4096];
  ssize_t bytesRead;
  long lineCount = 0;

  while ((bytesRead = read(fd, buffer, sizeof(buffer))) > 0)
    for (ssize_t i = 0; i < bytesRead; ++i)
      if (buffer[i] == '\n') ++lineCount;

  if (bytesRead == -1) {
    perror("Error reading file");
    close(fd);
    return -1;
  }

  close(fd);
  return lineCount;
}

