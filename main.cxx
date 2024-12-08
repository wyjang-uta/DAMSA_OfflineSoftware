#include <iostream>

#include "TApplication.h"
#include "TFile.h"

#include "DMSPlotter.h"

int main(int argc, char *argv[]) {
  TApplication app("DMSPlotterApp", &argc, argv);

  /*
  std::cout << "argc: " << argc << std::endl;
  std::cout << "app.Argc(): " << app.Argc() << std::endl;
  for(int i = 0; i < argc; ++i) std::cout << "argv[" <<i<< "]: " << argv[i] << '\n';
  for(int i = 0; i < app.Argc(); ++i) std::cout << "app.Argv()[" <<i<< "]: " << app.Argv()[i] << '\n';
  */

  if (app.Argc() < 2) {
    std::cerr << "Usage: " << app.Argv()[0] << " <path/to/your/file.root>\n";
    return 1;
  }

  TFile* file = TFile::Open(app.Argv()[1]);

  if( !file || file->IsZombie() ) {
    std::cerr << "Error: Could not open file, or the IsZombie() returned false.\n";
    return 1;
  }


  DMSPlotter* plotter = new DMSPlotter(gClient->GetRoot(), 1024, 768, file);
  app.Run();

  delete plotter;
  return 0;
}

