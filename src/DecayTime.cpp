#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

/*
*
*Meant to be used as root macros.
*DecayTime is an analysis program that basically does this : 
*(1) Imports a file (specified by a path given as an input) assumed to have to columns, that will be converted in CHv (it corresponds to
*the channel that was activated at a given time) and CLKv (the time in which the channel was triggered)
*(2) Loops over CHv until it finds 1 (START signal): if so it increments index until it finds a 2(STOP signal), if the difference is more 
*than 8 clock cycles, it is multiplied by a calibration constant (see Calibration) and registered in an histogram.
*(3) The histogram is then plotted, the user can work with it with various root utilities
*/

void DecayTime(const char* path) {
    
    ifstream FIFO(path);
    
    TTree* DecayTree = new TTree("Tree", "DecayTree");

    long CH, CLK;

    DecayTree->Branch("CH", &CH, "CH/I");
    DecayTree->Branch("CLK", &CLK, "CLK/I");

    TH1F *h = new TH1F("Results", "Decay time histogram", 100, 0, 20);

    while (FIFO >> CH >> CLK){

        if (CH > 2){continue;}

        else {DecayTree->Fill();}

    }

    Long64_t N = DecayTree->GetEntries();

    double a = 4.98892e-3;

    for (Long64_t i = 0 ; i < N ; i++){

        DecayTree->GetEntry(i);

        if (CH == 1) {

            long START = CLK;

            Long64_t j = i + 1;

            while (j < N) {

                DecayTree->GetEntry(j);

                long STOP = CLK;

                long diff = STOP-START;

                if (CH == 2 && diff > 20){

                    h->Fill(a * diff);

                    break;

                }

                else {j++;}

            }

            i = j+1;

        }

    }

        TCanvas* c = new TCanvas("c_decay", "Canvas Decay Time", 800, 600);

    h->GetXaxis()->SetTitle("Decay Time [us]");

    h->GetYaxis()->SetTitle("Counts [pure]");

    TF1* Exp = new TF1("Exp", "[0] * exp(-x/[1])", 0, 20);

    Exp->SetParNames("N0", "tau");

    Exp->SetParameter(0, h->GetMaximum());

    Exp->SetParameter(1, 2);

    h->Fit(Exp, "R");

    h->Draw();


}