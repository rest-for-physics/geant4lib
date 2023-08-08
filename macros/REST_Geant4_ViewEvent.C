#include "TRestBrowser.h"
#include "TRestTask.h"

#ifndef RestTask_ViewG4Events
#define RestTask_ViewG4Events

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro.
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_Geant4_ViewEvent(TString fName, Double_t geomScale = 0.1) {
    TFile* f = TFile::Open(fName);

    TIter nextkey(f->GetListOfKeys());
    TKey* key;
    Bool_t containsGeometry = false;
    while ((key = (TKey*)nextkey())) {
        RESTDebug << "Reading key with name : " << key->GetName() << RESTendl;
        RESTDebug << "Key type (class) : " << key->GetClassName() << RESTendl;

        if (key->GetClassName() == (TString) "TGeoManager") containsGeometry = true;
    }

    if (!containsGeometry) {
        RESTWarning << "The file you are trying to visualize does not contain a geometry file!" << RESTendl;
        RESTWarning << "Are you opening the file generated direcly with restG4?" << RESTendl;
    }

    TRestBrowser* browser = new TRestBrowser("TRestGeant4EventViewer", geomScale);

    TRestEvent* eve = new TRestGeant4Event();
    browser->SetInputEvent(eve);

    browser->OpenFile(fName);

// when we run this macro from restManager from bash,
// we need to call TRestMetadata::GetChar() to prevent returning,
// while keeping GUI alive.
#ifdef REST_MANAGER
    GetChar("Running...\nPress a key to exit");
#endif

    return 0;
}
#endif
