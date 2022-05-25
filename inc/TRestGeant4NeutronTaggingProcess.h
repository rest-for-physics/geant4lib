/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2020 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

#ifndef RestCore_TRestGeant4NeutronTaggingProcess
#define RestCore_TRestGeant4NeutronTaggingProcess

#include <TRestGeant4Event.h>
#include <TRestGeant4Metadata.h>

#include "TRestEventProcess.h"

class TRestGeant4NeutronTaggingProcess : public TRestEventProcess {
   private:
    /// A pointer to the specific TRestGeant4Event input
    TRestGeant4Event* fInputG4Event;  //!
    /// A pointer to the specific TRestGeant4Event output
    TRestGeant4Event* fOutputG4Event;  //!
    /// A pointer to the simulation metadata information accessible to TRestRun
    TRestGeant4Metadata* fG4Metadata;  //!

    /// TODO these members should be documented
    std::vector<int> fVetoVolumeIds;       //!
    std::vector<int> fCaptureVolumeIds;    //!
    std::vector<int> fShieldingVolumeIds;  //!

    std::string fVetoKeyword = "";                                          //!
    std::string fCaptureKeyword = "";                                       //!
    std::string fShieldingKeyword = "";                                     //!
    std::vector<std::string> fVetoGroupKeywords;                            //!
    std::map<std::string, std::vector<std::string>> fVetoGroupVolumeNames;  //!
    std::vector<Float_t> fQuenchingFactors;                                 //!

    // neutrons that undergo neutron capture
    Int_t fNeutronsCapturedNumber;  //!
    /// TODO it might be simplified using std::vector<TVector3>
    std::vector<Double_t> fNeutronsCapturedPosX;  //!
    std::vector<Double_t> fNeutronsCapturedPosY;  //!
    std::vector<Double_t> fNeutronsCapturedPosZ;  //!
    /// If documentation is added perhaps they could be shorter names
    std::vector<Int_t> fNeutronsCapturedIsCaptureVolume;                       //!
    std::vector<Double_t> fNeutronsCapturedProductionE;                        //!
    std::vector<Double_t> fNeutronsCapturedEDepByNeutron;                      //!
    std::vector<Double_t> fNeutronsCapturedEDepByNeutronAndChildren;           //!
    std::vector<Double_t> fNeutronsCapturedEDepByNeutronInVeto;                //!
    std::vector<Double_t> fNeutronsCapturedEDepByNeutronAndChildrenInVeto;     //!
    std::vector<Double_t> fNeutronsCapturedEDepByNeutronAndChildrenInVetoMax;  //!
    std::vector<Double_t> fNeutronsCapturedEDepByNeutronAndChildrenInVetoMin;  //!

    // gammas that are produced from neutron capture
    Int_t fGammasNeutronCaptureNumber;                        //!
    std::vector<Double_t> fGammasNeutronCapturePosX;          //!
    std::vector<Double_t> fGammasNeutronCapturePosY;          //!
    std::vector<Double_t> fGammasNeutronCapturePosZ;          //!
    std::vector<Int_t> fGammasNeutronCaptureIsCaptureVolume;  //!
    std::vector<Double_t> fGammasNeutronCaptureProductionE;   //!

    // secondary neutrons that exit the lead shielding
    Int_t fSecondaryNeutronsShieldingNumber;                                  //!
    std::vector<Double_t> fSecondaryNeutronsShieldingExitPosX;                //!
    std::vector<Double_t> fSecondaryNeutronsShieldingExitPosY;                //!
    std::vector<Double_t> fSecondaryNeutronsShieldingExitPosZ;                //!
    std::vector<Int_t> fSecondaryNeutronsShieldingIsCaptured;                 //!
    std::vector<Int_t> fSecondaryNeutronsShieldingIsCapturedInCaptureVolume;  //!
    std::vector<Double_t> fSecondaryNeutronsShieldingProductionE;             //!
    std::vector<Double_t> fSecondaryNeutronsShieldingExitE;                   //!

    void InitFromConfigFile() override;
    void Initialize() override;
    void LoadDefaultConfig();
    void Reset();

   protected:
    // add here the members of your event process

   public:
    any GetInputEvent() const override { return fInputG4Event; }
    any GetOutputEvent() const override { return fOutputG4Event; }

    void InitProcess() override;
    TRestEvent* ProcessEvent(TRestEvent* inputEvent) override;
    void EndProcess() override;

    void LoadConfig(const std::string& configFilename, const std::string& name = "");

    /// It prints out the process parameters stored in the metadata structure
    void PrintMetadata() override {
        BeginPrintProcess();

        RESTDebug << "VETO KEYWORD: " << fVetoKeyword << RESTendl;
        RESTDebug << RESTendl;

        RESTDebug << "VETO GROUP KEYWORDS:" << RESTendl;
        for (unsigned int i = 0; i < fVetoGroupKeywords.size(); i++) {
            RESTDebug << "\t" << fVetoGroupKeywords[i] << RESTendl;
        }
        RESTDebug << RESTendl;

        RESTDebug << "Found " << fVetoVolumeIds.size() << " veto volumes:" << RESTendl;
        for (unsigned int i = 0; i < fVetoVolumeIds.size(); i++) {
            RESTDebug << "\t" << fG4Metadata->GetActiveVolumeName(fVetoVolumeIds[i]) << RESTendl;
        }
        RESTDebug << RESTendl;

        // capture volumes

        RESTDebug << "CAPTURE KEYWORD: " << fCaptureKeyword << RESTendl;
        RESTDebug << RESTendl;

        RESTDebug << "Found " << fCaptureVolumeIds.size() << " Capture volumes:" << RESTendl;
        for (unsigned int i = 0; i < fCaptureVolumeIds.size(); i++) {
            RESTDebug << "\t" << fG4Metadata->GetActiveVolumeName(fCaptureVolumeIds[i]) << RESTendl;
        }
        RESTDebug << RESTendl;

        // shielding volume/s

        RESTDebug << "SHIELDING KEYWORD: " << fShieldingKeyword << RESTendl;
        RESTDebug << RESTendl;

        RESTDebug << "Found " << fShieldingVolumeIds.size() << " Shielding volumes:" << RESTendl;
        for (unsigned int i = 0; i < fShieldingVolumeIds.size(); i++) {
            RESTDebug << "\t" << fG4Metadata->GetActiveVolumeName(fShieldingVolumeIds[i]) << RESTendl;
        }
        RESTDebug << RESTendl;

        RESTDebug << "QUENCHING FACTORS (" << fQuenchingFactors.size() << " Total)" << RESTendl;
        for (unsigned int i = 0; i < fQuenchingFactors.size(); i++) {
            RESTDebug << "\t" << fQuenchingFactors[i] << RESTendl;
        }
        RESTDebug << RESTendl;

        EndPrintProcess();
    }

    /// Returns a new instance of this class
    TRestEventProcess* Maker() { return new TRestGeant4NeutronTaggingProcess; }
    /// Returns the name of this process
    const char* GetProcessName() const override { return "geant4NeutronTagging"; }

    TRestGeant4NeutronTaggingProcess();
    TRestGeant4NeutronTaggingProcess(const char* configFilename);
    ~TRestGeant4NeutronTaggingProcess();

    ClassDefOverride(TRestGeant4NeutronTaggingProcess, 1);
};
#endif  // RestCore_TRestGeant4NeutronTaggingProcess
