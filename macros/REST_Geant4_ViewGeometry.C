#include <iostream>

#include "TRestBrowser.h"
#include "TRestGeant4Event.h"
#include "TRestTask.h"
#include "TRestGDMLParser.h"
#include "TEveManager.h"
#include "TEveGeoNode.h"

using namespace std;

#ifndef RestTask_ViewGeometry
#define RestTask_ViewGeometry

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro.
//*** This macro might need update/revision.
//***
//*******************************************************************************************************
Int_t REST_Geant4_ViewGeometry(const char *filename, const char *option = "") {
    cout << "REST View Geometry Macro called with input: " << filename << endl;

    TGeoManager *geo = nullptr;
    if (TRestTools::isRootFile(filename)) {
        TFile *fFile = new TFile(filename);

        TIter nextkey(fFile->GetListOfKeys());
        TKey *key;
        while ((key = (TKey *) nextkey()) != nullptr) {
            if (key->GetClassName() == (TString) "TGeoManager") {
                if (geo == nullptr)
                    geo = (TGeoManager *) fFile->Get(key->GetName());
                else if (key->GetName() == (TString) "Geometry")
                    geo = (TGeoManager *) fFile->Get(key->GetName());
            }
        }
    } else if (string(filename).find(".gdml") != string::npos) {
        auto parser = new TRestGDMLParser();
        parser->Load(filename);
        geo = parser->CreateGeoManager();
    } else {
        cout << "File type is not supported. Only .root and .gdml files are supported" << endl;
        return 1;
    }

    if (geo == nullptr) {
        RESTError << "Geometry initialization failed!" << RESTendl;
        return 1;
    }

    if (geo->GetTopVolume() == nullptr) {
        RESTError << "No master volume found in the geometry!" << RESTendl;
        return 1;
    }

    if (string(option).empty()) {
        cout << "Launching ROOT viewer..." << endl;
        geo->GetTopVolume()->Draw("ogl");
    } else if (ToUpper((string) option) == "EVE") {
        cout << "Launching EVE viewer..." << endl;
        /*
        TRestEventViewer *view = (TRestEventViewer *) REST_Reflection::Assembly("TRestGeant4EventViewer");
        if (view == nullptr) return -1;
        view->SetGeometry(geo);
        view->AddEvent(new TRestGeant4Event());
        */

        auto eve = TEveManager::Create();

        auto node = geo->GetTopNode();
        auto eveTopNode = new TEveGeoTopNode(geo, node);
        eveTopNode->SetVisLevel(10);
        eve->AddGlobalElement(eveTopNode);

        for (int i = 0; i < geo->GetListOfVolumes()->GetEntries(); i++) {
            auto volume = geo->GetVolume(i);
            auto material = volume->GetMaterial();
            if (material->GetDensity() <= 0.01) {
                volume->SetTransparency(95);
                if (material->GetDensity() <= 0.001) {
                    // We consider this vacuum for display purposes
                    volume->SetVisibility(kFALSE);
                }
            } else {
                volume->SetTransparency(50);
            }
        }

        eve->FullRedraw3D(kTRUE);
    }
// when we run this macro from restManager from bash,
// we need to call TRestMetadata::GetChar() to prevent returning,
// while keeping GUI alive.

    GetChar("Running...\nPress a key to exit");
    return 0;
}

#endif
