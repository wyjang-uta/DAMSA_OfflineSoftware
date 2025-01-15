#include <iostream>
#include <typeinfo>

#include <boost/program_options.hpp>

#include "TApplication.h"
#include "TFile.h"

#include "DMSPlotter.h"
#include "DMSDataProcess.h"

namespace bpo = boost::program_options;

int main(int argc, char *argv[])
{
  bpo::options_description desc("Allowed options");
  desc.add_options()
    ("help,h", "Show help message")
    ("gui,p", bpo::value<std::string>(), "Run in GUI mode with an input file")
    ("batch,b", bpo::value<std::vector<std::string>>()->multitoken(), "Run in Batch mode with an input and an output file");
  bpo::variables_map vm;
  bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
  bpo::notify(vm);

  if (vm.count("gui"))
  {
    std::cout << "GUI mode with file: " << vm["gui"].as<std::string>() << "\n";
    TApplication app("DMSPlotterApp", &argc, argv);
    //TFile* file = TFile::Open(vm["gui"].as<std::string>().data());
    //if( !file || file->IsZombie() ) {
    //  std::cerr << "Error: Could not open file, or the IsZombie() returned false.\n";
    //  return 1;
    //}
    //std::cout << typeid(vm["gui"].as<std::string>().c_str()).name() << std::endl;
    DMSPlotter* plotter = new DMSPlotter( gClient->GetRoot(), 800, 600, vm["gui"].as<std::string>().c_str() );
    app.Run();
    delete plotter;
  }
  else if (vm.count("batch"))
  {
    auto files = vm["batch"].as<std::vector<std::string>>();
    if (files.size() == 2)
    {
      std::cout << "DAMSA Offline Software v0.1" << std::endl;
      std::cout << "//Batch mode//" << std::endl;
      std::cout << "Input directory: " << files[0] << std::endl;
      std::cout << "Output file: " << files[1] << std::endl;
      char inputDirectory[256];
      char outputFileName[256];
      strcpy(inputDirectory, files[0].data());
      strcpy(outputFileName, files[1].data());
      DMSDataProcess* dataProcess = new DMSDataProcess(inputDirectory, outputFileName);
      dataProcess->ProcessFile();
    }
    else
    {
      std::cerr << "Batch mode requires two files: input and output.\n";
    }
  }
  else
  {
    std::cerr << "Invalid arguments. Use --help for usage.\n";
  }

  std::cout << "Program finished!" << std::endl;

  return 0;
}

