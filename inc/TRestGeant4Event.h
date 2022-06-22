/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
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

#ifndef RestCore_TRestGeant4Event
#define RestCore_TRestGeant4Event

#include <TGraph.h>
#include <TGraph2D.h>
#include <TH1D.h>
#include <TH2F.h>
#include <TLegend.h>
#include <TMultiGraph.h>
#include <TObject.h>
#include <TRestEvent.h>
#include <TRestGeant4Track.h>
#include <TVector3.h>

#include <iostream>
#include <map>
#include <utility>

class G4Event;
class G4Track;
class G4Step;

/// An event class to store geant4 generated event information
class TRestGeant4Event : public TRestEvent {
   private:
#ifndef __CINT__
    Double_t fMinX, fMaxX;            //!
    Double_t fMinY, fMaxY;            //!
    Double_t fMinZ, fMaxZ;            //!
    Double_t fMinEnergy, fMaxEnergy;  //!
#endif

    void AddEnergyDepositToVolume(Int_t volID, Double_t eDep);

    Bool_t PerProcessEnergyInitFlag = false;
    std::map<std::string, Double_t> PerProcessEnergyInSensitive;

    std::map<std::string, std::map<std::string, Double_t>> fEnergyInVolumePerProcess;

    // TODO: review this method
    void InitializePerProcessEnergyInSensitive();

   protected:
#ifndef __CINT__

    // TODO These graphs should be placed in TRestTrack?
    // (following GetGraph implementation in TRestDetectorSignal)
    TGraph* fXZHitGraph;  //!
    TGraph* fYZHitGraph;  //!
    TGraph* fXYHitGraph;  //!
    // TGraph2D *fXYZHitGraph; //! (TODO to implement XYZ visualization)

    std::vector<TGraph*> fXYPcsMarker;  //!
    std::vector<TGraph*> fYZPcsMarker;  //!
    std::vector<TGraph*> fXZPcsMarker;  //!

    TMultiGraph* fXZMultiGraph;  //!
    TMultiGraph* fYZMultiGraph;  //!
    TMultiGraph* fXYMultiGraph;  //!
    // TMultiGraph *fXYZMultiGraph; //! (TODO to implement XYZ visualization)

    TH2F* fXYHisto;  //!
    TH2F* fXZHisto;  //!
    TH2F* fYZHisto;  //!

    TH1D* fXHisto;  //!
    TH1D* fYHisto;  //!
    TH1D* fZHisto;  //!

    TLegend* fLegend_XY;  //!
    TLegend* fLegend_XZ;  //!
    TLegend* fLegend_YZ;  //!

    std::vector<Int_t> legendAdded;  //!

    Int_t fTotalHits;  //!

    TMultiGraph* GetXZMultiGraph(Int_t gridElement, std::vector<TString> pcsList, Double_t minPointSize = 0.4,
                                 Double_t maxPointSize = 4);
    TMultiGraph* GetYZMultiGraph(Int_t gridElement, std::vector<TString> pcsList, Double_t minPointSize = 0.4,
                                 Double_t maxPointSize = 4);
    TMultiGraph* GetXYMultiGraph(Int_t gridElement, std::vector<TString> pcsList, Double_t minPointSize = 0.4,
                                 Double_t maxPointSize = 4);

    TH2F* GetXYHistogram(Int_t gridElement, std::vector<TString> optList);
    TH2F* GetXZHistogram(Int_t gridElement, std::vector<TString> optList);
    TH2F* GetYZHistogram(Int_t gridElement, std::vector<TString> optList);

    TH1D* GetXHistogram(Int_t gridElement, std::vector<TString> optList);
    TH1D* GetYHistogram(Int_t gridElement, std::vector<TString> optList);
    TH1D* GetZHistogram(Int_t gridElement, std::vector<TString> optList);
#endif

    TVector3 fPrimaryEventOrigin;

    std::vector<TString> fPrimaryParticleName;
    std::vector<TVector3> fPrimaryEventDirection;
    std::vector<Double_t> fPrimaryEventEnergy;

    Double_t fTotalDepositedEnergy;
    Double_t fSensitiveVolumeEnergy;

    Int_t fNVolumes;
    std::vector<Int_t> fVolumeStored;
    std::vector<std::string> fVolumeStoredNames;
    std::vector<Double_t> fVolumeDepositedEnergy;
    std::map<std::string, double> fEnergyInVolumeMap;

    std::vector<TRestGeant4Track> fTrack;

    Int_t fMaxSubEventID;

   public:
    void SetBoundaries();
    void SetBoundaries(Double_t xMin, Double_t xMax, Double_t yMin, Double_t yMax, Double_t zMin,
                       Double_t zMax);

    inline TString GetPrimaryEventParticleName(int n) const {
        if (fPrimaryParticleName.size() > n) {
            return fPrimaryParticleName[n];
        }
        return "Not defined";
    }

    TVector3 GetPrimaryEventDirection(Int_t n = 0) const { return fPrimaryEventDirection[n]; }
    TVector3 GetPrimaryEventOrigin() const { return fPrimaryEventOrigin; }
    Double_t GetPrimaryEventEnergy(Int_t n = 0) const { return fPrimaryEventEnergy[n]; }

    size_t GetNumberOfHits(Int_t volID = -1) const;
    size_t GetNumberOfPhysicalHits(Int_t volID = -1) const;

    inline size_t GetNumberOfTracks() const { return fTrack.size(); }
    inline Int_t GetNumberOfPrimaries() const { return fPrimaryEventDirection.size(); }
    inline Int_t GetNumberOfActiveVolumes() const { return fNVolumes; }

    inline Int_t isVolumeStored(int n) const { return fVolumeStored[n]; }
    inline const TRestGeant4Track& GetTrack(int n) const { return fTrack[n]; }
    inline TRestGeant4Track* GetTrackPointer(int n) { return &fTrack[n]; }
    TRestGeant4Track* GetTrackByID(int id);
    inline Int_t GetNumberOfSubEventIDTracks() const { return fMaxSubEventID + 1; }

    inline Double_t GetTotalDepositedEnergy() const { return fTotalDepositedEnergy; }
    inline Double_t GetEnergyDepositedInVolume(Int_t volID) const { return fVolumeDepositedEnergy[volID]; }
    inline Double_t GetSensitiveVolumeEnergy() const { return fSensitiveVolumeEnergy; }
    TVector3 GetMeanPositionInVolume(Int_t volID) const;
    TVector3 GetFirstPositionInVolume(Int_t volID) const;
    TVector3 GetLastPositionInVolume(Int_t volID) const;
    TVector3 GetPositionDeviationInVolume(Int_t volID) const;

    TRestHits GetHits(Int_t volID = -1) const;
    inline TRestHits GetHitsInVolume(Int_t volID) const { return GetHits(volID); }

    Int_t GetNumberOfTracksForParticle(const TString& parName) const;
    Int_t GetEnergyDepositedByParticle(const TString& parName) const;

    const std::map<std::string, std::map<std::string, Double_t>> GetEnergyInVolumePerProcess() const {
        return fEnergyInVolumePerProcess;
    }

    inline Double_t GetEnergyInSensitiveFromProcessPhoto() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["photoelectric"];
    }
    inline Double_t GetEnergyInSensitiveFromProcessCompton() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["compton"];
    }
    inline Double_t GetEnergyInSensitiveFromProcessEIoni() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["electron_ionization"];
    }
    inline Double_t GetEnergyInSensitiveFromProcessIonIoni() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["ion_ionization"];
    }
    inline Double_t GetEnergyInSensitiveFromProcessAlphaIoni() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["alpha_ionization"];
    }
    inline Double_t GetEnergyInSensitiveFromProcessMsc() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["msc"];
    }
    inline Double_t GetEnergyInSensitiveFromProcessHadronIoni() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["hadronic_ionization"];
    }
    inline Double_t GetEnergyInSensitiveFromProcessProtonIoni() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["proton_ionization"];
    }
    inline Double_t GetEnergyInSensitiveFromProcessHadronElastic() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["hadronic_elastic"];
    }
    inline Double_t GetEnergyInSensitiveFromProcessNeutronElastic() {
        if (!PerProcessEnergyInitFlag) {
            InitializePerProcessEnergyInSensitive();
        }
        return PerProcessEnergyInSensitive["neutron_elastic"];
    }

    inline void SetPrimaryEventDirection(const TVector3& dir) { fPrimaryEventDirection.push_back(dir); }
    inline void SetPrimaryEventParticleName(const TString& pName) { fPrimaryParticleName.push_back(pName); }
    inline void SetPrimaryEventEnergy(Double_t en) { fPrimaryEventEnergy.push_back(en); }
    inline void ActivateVolumeForStorage(Int_t n) { fVolumeStored[n] = 1; }
    inline void DisableVolumeForStorage(Int_t n) { fVolumeStored[n] = 0; }

    void AddActiveVolume(const std::string& volumeName);
    void ClearVolumes();
    inline void AddEnergyToSensitiveVolume(Double_t en) { fSensitiveVolumeEnergy += en; }

    inline void SetEnergyDepositedInVolume(Int_t volID, Double_t eDep) {
        fVolumeDepositedEnergy[volID] = eDep;
    }
    inline void SetSensitiveVolumeEnergy(Double_t en) { fSensitiveVolumeEnergy = en; }

    inline Int_t GetLowestTrackID() const {
        Int_t lowestID = 0;
        if (GetNumberOfTracks() > 0) {
            lowestID = GetTrack(0).GetTrackID();
        }

        for (int i = 0; i < GetNumberOfTracks(); i++) {
            auto tr = GetTrack(i);
            if (tr.GetTrackID() < lowestID) lowestID = tr.GetTrackID();
        }

        return lowestID;
    }

    void SetTrackSubEventID(Int_t n, Int_t id);
    void AddTrack(TRestGeant4Track trk);

    std::set<std::string> GetUniqueParticles() const;

    inline Bool_t isRadiactiveDecay() const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isRadiactiveDecay()) return true;
        return false;
    }

    inline Bool_t isPhotoElectric() const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isPhotoElectric()) return true;
        return false;
    }
    inline Bool_t isCompton() const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isCompton()) return true;
        return false;
    }
    inline Bool_t isBremstralung() const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isBremstralung()) return true;
        return false;
    }

    inline Bool_t ishadElastic() const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).ishadElastic()) return true;
        return false;
    }
    inline Bool_t isneutronInelastic() const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isneutronInelastic()) return true;
        return false;
    }

    inline Bool_t isnCapture() const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isnCapture()) return true;
        return false;
    }

    inline Bool_t ishIoni() const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).ishIoni()) return true;
        return false;
    }

    inline Bool_t isphotonNuclear() const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isphotonNuclear()) return true;
        return false;
    }

    inline Bool_t isAlpha() const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).GetParticleName() == "alpha") return true;
        return false;
    }

    inline Bool_t isNeutron() const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).GetParticleName() == "neutron") return true;
        return false;
    }

    inline Bool_t isArgon() const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if ((GetTrack(n).GetParticleName()).Contains("Ar")) return true;
        return false;
    }

    inline Bool_t isXenon() const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if ((GetTrack(n).GetParticleName()).Contains("Xe")) return true;
        return false;
    }

    inline Bool_t isNeon() const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if ((GetTrack(n).GetParticleName()).Contains("Ne")) return true;
        return false;
    }
    /// Processes and particles in a given volume

    inline Bool_t isRadiactiveDecayInVolume(Int_t volID) const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isRadiactiveDecayInVolume(volID)) return true;
        return false;
    }

    inline Bool_t isPhotoElectricInVolume(Int_t volID) const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isPhotoElectricInVolume(volID)) return true;
        return false;
    }
    inline Bool_t isPhotonNuclearInVolume(Int_t volID) const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isPhotonNuclearInVolume(volID)) return true;
        return false;
    }
    inline Bool_t isComptonInVolume(Int_t volID) const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isComptonInVolume(volID)) return true;
        return false;
    }
    inline Bool_t isBremstralungInVolume(Int_t volID) const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isBremstralungInVolume(volID)) return true;
        return false;
    }

    inline Bool_t isHadElasticInVolume(Int_t volID) const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isHadElasticInVolume(volID)) return true;
        return false;
    }
    inline Bool_t isNeutronInelasticInVolume(Int_t volID) const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isNeutronInelasticInVolume(volID)) return true;
        return false;
    }

    inline Bool_t isNCaptureInVolume(Int_t volID) const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isNCaptureInVolume(volID)) return true;
        return false;
    }

    inline Bool_t ishIoniInVolume(Int_t volID) const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isHIoniInVolume(volID)) return true;
        return false;
    }

    inline Bool_t isAlphaInVolume(Int_t volID) const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isAlphaInVolume(volID)) return true;
        return false;
    }

    inline Bool_t isNeutronInVolume(Int_t volID) const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isNeutronInVolume(volID)) return true;
        return false;
    }

    inline Bool_t isArgonInVolume(Int_t volID) const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isArgonInVolume(volID)) return true;
        return false;
    }

    inline Bool_t isXenonInVolume(Int_t volID) const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isXenonInVolume(volID)) return true;
        return false;
    }

    inline Bool_t isNeonInVolume(Int_t volID) const {
        for (int n = 0; n < GetNumberOfTracks(); n++)
            if (GetTrack(n).isNeonInVolume(volID)) return true;
        return false;
    }

    void Initialize();

    /// maxTracks : number of tracks to print, 0 = all
    void PrintActiveVolumes() const;
    void PrintEvent(int maxTracks = 0, int maxHits = 0,
                    const TRestGeant4Metadata* geant4Metadata = nullptr) const;
    inline void PrintEvent(const TRestGeant4Metadata* geant4Metadata) const {
        PrintEvent(0, 0, geant4Metadata);
    }

    inline TPad* DrawEvent(const TString& option = "") { return DrawEvent(option, true); }
    TPad* DrawEvent(const TString& option, Bool_t autoBoundaries);

    // Constructor
    TRestGeant4Event();
    // Destructor
    virtual ~TRestGeant4Event();

    ClassDef(TRestGeant4Event, 7);  // REST event superclass

    // restG4
   public:
    TRestGeant4Event(const G4Event*, const TRestGeant4Metadata&);  //! // Implemented in restG4
    bool InsertTrack(const G4Track*);                              //!
    void UpdateTrack(const G4Track*);                              //!
    void InsertStep(const G4Step*, TRestGeant4Metadata&);          //!

    friend class OutputManager;

   private:
    std::map<int, int> fTrackIDToTrackIndex = {};  //!
    TRestGeant4Hits fInitialStep;                  //!
};
#endif
