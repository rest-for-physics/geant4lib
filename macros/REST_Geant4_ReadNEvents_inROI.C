#include "TRestGeant4Event.h"
#include "TRestGeant4Metadata.h"
#include "TRestTask.h"

#ifndef RestTask_ReadNEvents_inROI
#define RestTask_ReadNEvents_inROI

//*******************************************************************************************************
//*** Description : It prints out on screen Geant4 events in the ROI between entries n1 and n2.
//*** --------------
//*** This macro might need update/revision.
//*******************************************************************************************************
Int_t REST_Geant4_ReadNEvents_inROI(string fName, int n1, int n2, double en1, double en2) {
    TRestRun* run = new TRestRun(fName);

    run->PrintMetadata();

    /////////////////////////////

    /////////////////////////////
    // Reading events
    int n = 0;
    double en = 0;
    TRestGeant4Event* event = new TRestGeant4Event();

    run->SetInputEvent(event);
    for (int i = n1; i < n2 + 1; i++) {
        run->GetEntry(i);
        en = event->GetSensitiveVolumeEnergy();
        if ((en >= en1) && (en <= en2)) {
            event->PrintEvent();
            n++;
            cout << n << " event with " << en << " energy in sensitive volume " << endl;
        }
    }

    delete event;
    delete run;

    return 0;
}
#endif
