///_______________________________________________________________________________
///_______________________________________________________________________________
///_______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4BlobAnalysisProcess.h
///
///_______________________________________________________________________________

#ifndef RestCore_TRestGeant4BlobAnalysisProcess
#define RestCore_TRestGeant4BlobAnalysisProcess

#include <TRestGeant4Event.h>
#include <TRestGeant4Metadata.h>

#include "TRestEventProcess.h"

class TRestGeant4BlobAnalysisProcess : public TRestEventProcess {
   private:
#ifndef __CINT__
    TRestGeant4Event* fG4Event;        //!
    TRestGeant4Metadata* fG4Metadata;  //!

    std::vector<std::string> fQ1_Observables;  //!
    std::vector<double> fQ1_Radius;            //!

    std::vector<std::string> fQ2_Observables;  //!
    std::vector<double> fQ2_Radius;            //!
#endif

    void InitFromConfigFile();

    void Initialize();

    void LoadDefaultConfig();

   protected:
    // add here the members of your event process

   public:
    any GetInputEvent() const override { return fG4Event; }
    any GetOutputEvent() const override { return fG4Event; }

    void InitProcess();
    TRestEvent* ProcessEvent(TRestEvent* eventInput);
    void EndProcess();

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    void PrintMetadata() {
        BeginPrintProcess();

        EndPrintProcess();
    }

    const char* GetProcessName() const override { return "findG4BlobAnalysis"; }

    // Constructor
    TRestGeant4BlobAnalysisProcess();
    TRestGeant4BlobAnalysisProcess(const char* configFilename);
    // Destructor
    ~TRestGeant4BlobAnalysisProcess();

    ClassDef(TRestGeant4BlobAnalysisProcess, 1);  // Template for a REST "event process" class inherited from
                                                  // TRestEventProcess
};
#endif
