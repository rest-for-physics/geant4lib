#include <string>
#include <vector>

#include "TRestGeant4Event.h"
#include "TRestGeant4Metadata.h"
#include "TRestTask.h"

#ifndef RestTask_Geant4_MergeRestG4Files
#define RestTask_Geant4_MergeRestG4Files

//*******************************************************************************************************
//***
//*** Your HELP is needed to verify, validate and document this macro.
//*** This macro might need update/revision.
//***
//*******************************************************************************************************

/*
 * Author: Luis Obis (lobis@unizar.es)
 * Description: Macro used to merge simulation files, before any analysis is performed
 * All files are understood to have the same configuration (same generator, same detector, etc.)
 * They can only differ in the random seed, run number or number of events.
 */

using namespace std;

void REST_Geant4_MergeRestG4Files(const char* outputFilename, const char* inputFilesDirectory) {
    // TODO: use glob pattern instead of directory. Already tried this but conflicts with TRestTask...

    cout << "Output file: " << outputFilename << endl;
    // print input
    cout << "Input files directory: " << inputFilesDirectory << endl;

    // find all .root files in the directory
    vector<string> inputFiles = TRestTools::GetFilesMatchingPattern(string(inputFilesDirectory) + "/*.root");

    cout << "Number of input files: " << inputFiles.size() << endl;
    for (auto file : inputFiles) {
        cout << "  - " << file << endl;
    }

    if (inputFiles.size() == 0) {
        cerr << "No input files found" << endl;
        exit(1);
    }

    // open the first file
    TRestGeant4Metadata mergeMetadata;

    auto mergeRun = new TRestRun();
    mergeRun->SetName("run");
    mergeRun->SetOutputFileName(outputFilename);
    mergeRun->FormOutputFile();
    mergeRun->GetOutputFile()->cd();
    mergeRun->SetRunType("restG4");

    TRestGeant4Event* mergeEvent = nullptr;
    auto mergeEventTree = mergeRun->GetEventTree();
    mergeEventTree->Branch("TRestGeant4EventBranch", "TRestGeant4Event", &mergeEvent);

    set<Int_t> eventIds;

    // iterate over all other files
    for (int i = 0; i < inputFiles.size(); i++) {
        auto run = TRestRun(inputFiles[i].c_str());
        auto metadata = (TRestGeant4Metadata*)run.GetMetadataClass("TRestGeant4Metadata");
        if (i == 0) {
            mergeMetadata = *metadata;
        } else {
            mergeMetadata.Merge(*metadata);
        }
        TRestGeant4Event* event = nullptr;
        auto eventTree = run.GetEventTree();
        eventTree->SetBranchAddress("TRestGeant4EventBranch", &event);
        for (int j = 0; j < eventTree->GetEntries(); j++) {
            eventTree->GetEntry(j);
            Int_t eventId = event->GetID();
            if (eventIds.find(eventId) != eventIds.end()) {
                const maxEventId = *max_element(eventIds.begin(), eventIds.end());
                eventId = maxEventId + 1;
            }
            eventIds.insert(eventId);
            event->SetID(eventId);
            *mergeEvent = *event;
            mergeEventTree->Fill();
            mergeRun->GetAnalysisTree()->Fill();
        }
    }

    cout << "Output filename: " << mergeRun->GetOutputFileName() << endl;
    cout << "Output file: " << mergeRun->GetOutputFile() << endl;

    mergeRun->GetOutputFile()->cd();

    gGeoManager->Write("Geometry", TObject::kOverwrite);
    // mergeRun->AddMetadata(static_cast<TRestMetadata*>(&mergeMetadata));
    mergeMetadata.SetName("geant4Metadata");
    mergeMetadata.Write();
    mergeRun->UpdateOutputFile();
    mergeRun->CloseFile();
}

#endif
