//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4PhysicsLists.h
///
///             G4 class to be used to design REST metadata classes to be
///             inherited from TRestMetadata
///             How to use: replace TRestGeant4PhysicsLists by your class name,
///             fill the required functions following instructions and add all
///             needed additional members and functionality
///
///             Apr 2017:   First concept. Javier Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestGeant4PhysicsLists
#define RestCore_TRestGeant4PhysicsLists

#include <TRestMetadata.h>
#include <TString.h>
#include <stdio.h>
#include <stdlib.h>

#include <cstdio>
#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

class TRestGeant4PhysicsLists : public TRestMetadata {
   private:
    void Initialize() override;

    void InitFromConfigFile() override;

    TString GetPhysicsListOptionString(const TString& physicsListName) const;

    std::vector<TString> fPhysicsLists;
    std::vector<TString> fPhysicsListOptions;

    Double_t fCutForElectron;
    Double_t fCutForGamma;
    Double_t fCutForPositron;
    Double_t fCutForMuon;
    Double_t fCutForNeutron;
    Double_t fMinEnergyRangeProductionCuts;
    Double_t fMaxEnergyRangeProductionCuts;

    std::vector<std::string> fIonLimitStepList;

   public:
    inline Double_t GetCutForGamma() const { return fCutForGamma; }
    inline Double_t GetCutForElectron() const { return fCutForElectron; }
    inline Double_t GetCutForPositron() const { return fCutForPositron; }
    inline Double_t GetCutForMuon() const { return fCutForMuon; }
    inline Double_t GetCutForNeutron() const { return fCutForNeutron; }

    std::vector<std::string> GetIonStepList() const { return fIonLimitStepList; }

    inline Double_t GetMinimumEnergyProductionCuts() const { return fMinEnergyRangeProductionCuts; }
    inline Double_t GetMaximumEnergyProductionCuts() const { return fMaxEnergyRangeProductionCuts; }

    Int_t FindPhysicsList(const TString& physicsListName) const;
    Bool_t PhysicsListExists(const TString& physicsListName) const;

    TString GetPhysicsListOptionValue(const TString& physicsListName, const TString& option,
                                      const TString& defaultValue = "NotDefined") const;

    void PrintMetadata() override;

    // Constructors
    TRestGeant4PhysicsLists();
    TRestGeant4PhysicsLists(const char* configFilename, std::string name = "");
    // Destructor
    ~TRestGeant4PhysicsLists();

    ClassDefOverride(TRestGeant4PhysicsLists, 2);
};
#endif
