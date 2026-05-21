///_______________________________________________________________________________
///_______________________________________________________________________________
///_______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4PhysicsLists.cxx
///
///             G4 class description
///
///             Apr 2017:   First concept
///                     A metadata class to store the physics lists to be used
///                     in a REST-Geant4 simulation.
///
///                 Javier Galan
///_______________________________________________________________________________

#include "TRestGeant4PhysicsLists.h"

#include <TRestTools.h>

using namespace std;

ClassImp(TRestGeant4PhysicsLists);

TRestGeant4PhysicsLists::TRestGeant4PhysicsLists() : TRestMetadata() { Initialize(); }

TRestGeant4PhysicsLists::TRestGeant4PhysicsLists(const char* configFilename, string name)
    : TRestMetadata(configFilename) {
    Initialize();
    LoadConfigFromFile(fConfigFileName, name);
}

TRestGeant4PhysicsLists::~TRestGeant4PhysicsLists() = default;

void TRestGeant4PhysicsLists::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);
}

void TRestGeant4PhysicsLists::InitFromConfigFile() {
    this->Initialize();

    fIonLimitStepList = Split(GetParameter("ionLimitStepList", ""), ",");

    fCutForGamma = GetDblParameterWithUnits("cutForGamma", 0.01);
    fCutForElectron = GetDblParameterWithUnits("cutForElectron", 1);
    fCutForPositron = GetDblParameterWithUnits("cutForPositron", 1);
    fCutForMuon = GetDblParameterWithUnits("cutForMuon", 1);
    fCutForNeutron = GetDblParameterWithUnits("cutForNeutron", 1);
    fMinEnergyRangeProductionCuts = GetDblParameterWithUnits("minEnergyRangeProductionCuts", 1);
    fMaxEnergyRangeProductionCuts = GetDblParameterWithUnits("maxEnergyRangeProductionCuts", 1e6);

    TiXmlElement* physicsListDefinition = GetElement("physicsList");
    while (physicsListDefinition) {
        // PhysicsList name
        TString physicsListName = GetFieldValue("name", physicsListDefinition);

        if (!PhysicsListExists(physicsListName)) {
            cerr << "TRestPhysicsList: Physics list: '" << physicsListName
                 << "' not found among valid options" << endl;
            exit(1);
        }

        TString optionString = "";
        TiXmlElement* optionDefinition = GetElement("option", physicsListDefinition);
        while (optionDefinition) {
            TString optionName = GetFieldValue("name", optionDefinition);
            TString optionValue = GetFieldValue("value", optionDefinition);

            if (optionString != "") {
                optionString += ":";
            }
            optionString += optionName + ":" + optionValue;

            optionDefinition = GetNextElement(optionDefinition);
        }

        fPhysicsLists.push_back(physicsListName);
        fPhysicsListOptions.push_back(optionString);
        physicsListDefinition = GetNextElement(physicsListDefinition);
    }
}

Int_t TRestGeant4PhysicsLists::FindPhysicsList(const TString& physicsListName) const {
    if (!PhysicsListExists(physicsListName)) return -1;

    for (unsigned int n = 0; n < fPhysicsLists.size(); n++) {
        if (fPhysicsLists[n] == physicsListName) {
            return (Int_t)n;
        }
    }

    return -1;
}

TString TRestGeant4PhysicsLists::GetPhysicsListOptionString(const TString& physicsListName) const {
    Int_t index = FindPhysicsList(physicsListName);

    if (index == -1) {
        return "";
    }

    return fPhysicsListOptions[index];
}

TString TRestGeant4PhysicsLists::GetPhysicsListOptionValue(const TString& physicsListName,
                                                           const TString& option,
                                                           const TString& defaultValue) const {
    vector<string> optList = TRestTools::GetOptions((string)GetPhysicsListOptionString(physicsListName));

    for (unsigned int n = 0; n < optList.size(); n = n + 2) {
        if (optList[n] == option) {
            return optList[n + 1];
        }
    }

    return defaultValue;
}

Bool_t TRestGeant4PhysicsLists::PhysicsListExists(const TString& physicsListName) const {
    const set<TString> validPhysicsLists = {"G4DecayPhysics",
                                            "G4RadioactiveDecayPhysics",
                                            "G4RadioactiveDecay",
                                            "G4RadioactiveDecayBase",
                                            "G4Radioactivation",
                                            "G4EmLivermorePhysics",
                                            "G4EmPenelopePhysics",
                                            "G4EmStandardPhysics_option3",
                                            "G4EmStandardPhysics_option4",
                                            "G4HadronElasticPhysicsHP",
                                            "G4IonBinaryCascadePhysics",
                                            "G4HadronPhysicsQGSP_BIC_HP",
                                            "G4NeutronTrackingCut",
                                            "G4EmExtraPhysics",
                                            "G4OpticalPhysics"};

    return validPhysicsLists.count(physicsListName) > 0;
}

void TRestGeant4PhysicsLists::PrintMetadata() {
    TRestMetadata::PrintMetadata();

    RESTMetadata << "Cut for electrons : " << fCutForElectron << " mm" << RESTendl;
    RESTMetadata << "Cut for positrons : " << fCutForPositron << " mm" << RESTendl;
    RESTMetadata << "Cut for gammas : " << fCutForGamma << " mm" << RESTendl;
    RESTMetadata << "Cut for muons : " << fCutForMuon << " mm" << RESTendl;
    RESTMetadata << "Cut for neutrons : " << fCutForNeutron << " mm" << RESTendl;
    RESTMetadata << "Min Energy for particle production: " << fMinEnergyRangeProductionCuts << " keV"
                 << RESTendl;
    RESTMetadata << "Max Energy for particle production: " << fMaxEnergyRangeProductionCuts << " keV"
                 << RESTendl;
    RESTMetadata << "---------------------------------------" << RESTendl;
    for (unsigned int n = 0; n < fPhysicsLists.size(); n++) {
        RESTMetadata << "Physics list " << n << " : " << fPhysicsLists[n] << RESTendl;
        vector<string> optList = TRestTools::GetOptions((string)fPhysicsListOptions[n]);
        for (unsigned int m = 0; m < optList.size(); m = m + 2)
            RESTMetadata << " - Option " << m / 2 << " : " << optList[m] << " = " << optList[m + 1]
                         << RESTendl;
    }
    if (!fIonLimitStepList.empty()) {
        RESTMetadata << "List of ions affected by step limit" << RESTendl;
        for (const auto& ion : fIonLimitStepList) {
            RESTMetadata << "   - " << ion << RESTendl;
        }
    }

    RESTMetadata << "******************************************" << RESTendl;
    RESTMetadata << RESTendl;
    RESTMetadata << RESTendl;
}
