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

// Usage:
// restGeant4_MergeRestG4Files merge_result.root /path/to/directory/with/files/*.root

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

    TRestRun mergeRun;
    mergeRun.SetName("run");
    mergeRun.SetOutputFileName(outputFilename);
    mergeRun.FormOutputFile();
    mergeRun.GetOutputFile()->cd();
    mergeRun.SetRunType("restG4");

    TRestGeant4Event* mergeEvent = nullptr;
    auto mergeEventTree = mergeRun.GetEventTree();
    mergeEventTree->Branch("TRestGeant4EventBranch", "TRestGeant4Event", &mergeEvent);

    set<Int_t> eventIds;  // std::set is sorted from lower to higher automatically

    long long eventCounter = 0;
    // iterate over all other files
    for (int i = 0; i < inputFiles.size(); i++) {
        cout << "Processing file " << i + 1 << "/" << inputFiles.size() << endl;

        map<Int_t, Int_t>
            eventIdUpdates;  // repeatedId -> newId. Make sure if there are repeated event ids in a file
                             // (because of sub-events) they keep the same event id after modification
        TRestRun run(inputFiles[i].c_str());
        auto metadata = dynamic_cast<TRestGeant4Metadata*>(run.GetMetadataClass("TRestGeant4Metadata"));
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
            *mergeEvent = *event;

            Int_t eventId = mergeEvent->GetID();
            if (eventIdUpdates.find(eventId) != eventIdUpdates.end()) {
                eventId = eventIdUpdates[eventId];
                cout << "WARNING: event ID " << mergeEvent->GetID() << " with sub-event ID "
                     << mergeEvent->GetSubID() << " already exists. It was already changed to " << eventId
                     << endl;
            } else if (eventIds.find(eventId) != eventIds.end()) {
                // set the new ID a number that is not in the set
                eventId = 1;
                while (eventIds.find(eventId) != eventIds.end()) {
                    eventId++;
                }
                eventIdUpdates[mergeEvent->GetID()] = eventId;
                cout << "WARNING: event ID " << mergeEvent->GetID() << " with sub-event ID "
                     << mergeEvent->GetSubID() << " already exists. Updating all instances of "
                     << mergeEvent->GetID() << " to " << eventId << endl;
            }
            mergeEvent->SetID(eventId);
            eventIds.insert(mergeEvent->GetID());

            mergeEventTree->Fill();
            mergeRun.GetAnalysisTree()->Fill();

            eventCounter++;
        }
    }

    cout << "Output filename: " << mergeRun.GetOutputFileName() << endl;
    cout << "Output file: " << mergeRun.GetOutputFile() << endl;

    mergeRun.GetOutputFile()->cd();

    gGeoManager->Write("Geometry", TObject::kOverwrite);

    mergeMetadata.SetName("geant4Metadata");
    mergeMetadata.Write();
    mergeRun.UpdateOutputFile();
    mergeRun.CloseFile();

    // Open the file again to check the number of events
    TRestRun runCheck(outputFilename);
    if (runCheck.GetEntries() != eventCounter) {
        cerr << "ERROR: number of events in the output file (" << runCheck.GetEntries()
             << ") does not match the number of events in the input files (" << eventCounter << ")" << endl;
        exit(1);
    }
    cout << "Number of events in the output file: " << runCheck.GetEntries() << " matches internal count"
         << endl;
}

#endif
