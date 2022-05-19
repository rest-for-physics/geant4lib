#include "TRestGeant4Event.h"
#include "TRestGeant4Metadata.h"
#include "TRestTask.h"

#ifndef RestTask_Geant4_FindIsotopes
#define RestTask_Geant4_FindIsotopes

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro.
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_Geant4_FindIsotopes(TString fName, TString fIsotope) {
    cout << "Filename : " << fName << " looking for " << fIsotope << endl;

    // gSystem->Load("librestcore.so");

    TRestRun* run = new TRestRun();

    string fname = fName.Data();
    if (!TRestTools::fileExists(fname)) {
        cout << "WARNING. Input file does not exist" << endl;
        exit(1);
    } else
        run->OpenInputFile(fName);

    if (run == nullptr) {
        cout << "WARNING no TRestGeant4Run class was found" << endl;
        exit(1);
    }

    run->OpenInputFile(fName);

    run->PrintMetadata();

    // Reading event
    TRestGeant4Event* event = new TRestGeant4Event();

    run->SetInputEvent(event);

    int j = 0;

    cout << "Total number of entries : " << run->GetEntries() << endl;

    // Printing the first event
    if (run->GetEntries() > 0) {
        for (int n = 0; n < run->GetEntries(); n++) {
            run->GetEntry(n);
            // cout<<" Event "<<n<<" has "<<event->GetNumberOfTracks()<<" tracks"<<endl;
            for (int i = 0; i < event->GetNumberOfTracks(); i++) {
                TString pName = event->GetTrack(i).GetParticleName();

                if ((event->GetTrack(i).GetParticleName() != "gamma") &&
                    (event->GetTrack(i).GetParticleName() != "e+") &&
                    (event->GetTrack(i).GetParticleName() != "e-")) {
                    if (event->GetTrack(i).GetParticleName() != "neutron")
                        cout << "Track : " << i << " --> " << event->GetTrack(i).GetParticleName() << "  "
                             << event->GetTrack(i).GetHits().GetNumberOfHits() + 1 << " hits in volume "
                             << event->GetTrack(i).GetHits().GetHitVolume(0) << endl;

                    if (pName.Contains(fIsotope)) j++;
                }
            }
        }

        // event->PrintEvent();

        cout << "Number of " << fIsotope << " nuclei activated = " << j << endl;
    }

    delete run;
    delete event;

    return 0;
}
#endif
