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

#include "TRestTools.h"

using namespace std;

ClassImp(TRestGeant4PhysicsLists);

TRestGeant4PhysicsLists::TRestGeant4PhysicsLists() : TRestMetadata() {
    // TRestGeant4PhysicsLists default constructor
    Initialize();
}

TRestGeant4PhysicsLists::TRestGeant4PhysicsLists(const char* configFilename, string name)
    : TRestMetadata(configFilename) {
    Initialize();

    LoadConfigFromFile(fConfigFileName, name);

    PrintMetadata();
}

TRestGeant4PhysicsLists::~TRestGeant4PhysicsLists() {
    // TRestGeant4PhysicsLists destructor
}

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
        TString phName = GetFieldValue("name", physicsListDefinition);

        if (!PhysicsListExists(phName)) {
            cout << "REST WARNING. TRestPhysicsList : Skipping physics list : " << phName << endl;
            continue;
        }

        TString optionString = "";
        TiXmlElement* optionDefinition = GetElement("physicsList", physicsListDefinition);
        while (optionDefinition) {
            TString optionName = GetFieldValue("name", optionDefinition);
            TString optionValue = GetFieldValue("value", optionDefinition);

            if (optionString != "") optionString += ":";

            optionString += optionName + ":" + optionValue;
        }

        fPhysicsLists.push_back(phName);
        fPhysicsListOptions.push_back(optionString);
        physicsListDefinition = GetNextElement(physicsListDefinition);
    }
}

Int_t TRestGeant4PhysicsLists::FindPhysicsList(TString phName) const {
    if (!PhysicsListExists(phName)) return -1;

    for (unsigned int n = 0; n < fPhysicsLists.size(); n++) {
        if (fPhysicsLists[n] == phName) {
            return (Int_t)n;
        }
    }

    return -1;
}

TString TRestGeant4PhysicsLists::GetPhysicsListOptionString(TString phName) const {
    Int_t index = FindPhysicsList(phName);

    if (index == -1) {
        return "";
    }

    return fPhysicsListOptions[index];
}

TString TRestGeant4PhysicsLists::GetPhysicsListOptionValue(TString phName, TString option,
                                                           TString defaultValue) const {
    vector<string> optList = TRestTools::GetOptions((string)GetPhysicsListOptionString(phName));

    for (unsigned int n = 0; n < optList.size(); n = n + 2) {
        if (optList[n] == option) {
            return optList[n + 1];
        }
    }

    return defaultValue;
}

Bool_t TRestGeant4PhysicsLists::PhysicsListExists(TString phName) const {
    if (phName == "G4DecayPhysics") return true;
    if (phName == "G4RadioactiveDecayPhysics") return true;
    if (phName == "G4RadioactiveDecay") return true;
    if (phName == "G4RadioactiveDecayBase") return true;
    if (phName == "G4Radioactivation") return true;
    if (phName == "G4EmLivermorePhysics") return true;
    if (phName == "G4EmPenelopePhysics") return true;
    if (phName == "G4EmStandardPhysics_option3") return true;
    if (phName == "G4EmStandardPhysics_option4") return true;
    if (phName == "G4HadronElasticPhysicsHP") return true;
    if (phName == "G4IonBinaryCascadePhysics") return true;
    if (phName == "G4HadronPhysicsQGSP_BIC_HP") return true;
    if (phName == "G4NeutronTrackingCut") return true;
    if (phName == "G4EmExtraPhysics") return true;

    cout << endl;
    cout << "REST Warning. TRestGeant4PhysicsLists::PhysicsListExists." << endl;
    cout << "Physics list " << phName << " does NOT exist in TRestGeant4PhysicsLists." << endl;
    cout << endl;

    return false;
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
    if (fIonLimitStepList.size() > 0)
        RESTMetadata << "List of ions where step limit is affecting" << RESTendl;
    for (unsigned int n = 0; n < fIonLimitStepList.size(); n++) {
        RESTMetadata << "   - " << fIonLimitStepList[n] << RESTendl;
    }
    RESTMetadata << "******************************************" << RESTendl;
    RESTMetadata << RESTendl;
    RESTMetadata << RESTendl;
}
