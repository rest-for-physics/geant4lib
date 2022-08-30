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
#include <TRestEvent.h>
#include <TVector3.h>

#include <iostream>
#include <map>
#include <utility>

#include "TRestGeant4Track.h"

class G4Event;
class G4Track;
class G4Step;
class TRestGeant4Metadata;

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

    TVector3 fPrimaryPosition;
    std::vector<TString> fPrimaryParticleNames;
    std::vector<Double_t> fPrimaryEnergies;
    std::vector<TVector3> fPrimaryDirections;

    TString fSubEventPrimaryParticleName;
    Double_t fSubEventPrimaryEnergy;
    TVector3 fSubEventPrimaryPosition;
    TVector3 fSubEventPrimaryDirection;

    Double_t fTotalDepositedEnergy;
    Double_t fSensitiveVolumeEnergy;

    Int_t fNVolumes;
    std::vector<Int_t> fVolumeStored;
    std::vector<std::string> fVolumeStoredNames;
    std::vector<Double_t> fVolumeDepositedEnergy;
    std::map<std::string, std::map<std::string, std::map<std::string, double>>>
        fEnergyInVolumePerParticlePerProcess;
    std::vector<TRestGeant4Track> fTracks;

    Int_t fMaxSubEventID;

   public:
    void SetBoundaries();
    void SetBoundaries(Double_t xMin, Double_t xMax, Double_t yMin, Double_t yMax, Double_t zMin,
                       Double_t zMax);

    inline size_t GetNumberOfPrimaries() const { return fPrimaryParticleNames.size(); }

    inline TString GetPrimaryEventParticleName(size_t n = 0) const { return fPrimaryParticleNames[n]; }
    inline TVector3 GetPrimaryEventDirection(size_t n = 0) const { return fPrimaryDirections[n]; }
    inline TVector3 GetPrimaryEventOrigin() const { return fPrimaryPosition; }
    inline Double_t GetPrimaryEventEnergy(size_t n = 0) const { return fPrimaryEnergies[n]; }

    inline Bool_t IsSubEvent() const { return fSubEventID > 0; }
    inline TString GetSubEventPrimaryEventParticleName() const { return fSubEventPrimaryParticleName; }
    inline TVector3 GetSubEventPrimaryEventDirection() const { return fSubEventPrimaryDirection; }
    inline TVector3 GetSubEventPrimaryEventOrigin() const { return fSubEventPrimaryPosition; }
    inline Double_t GetSubEventPrimaryEventEnergy() const { return fSubEventPrimaryEnergy; }

    size_t GetNumberOfHits(Int_t volID = -1) const;
    size_t GetNumberOfPhysicalHits(Int_t volID = -1) const;

    inline const std::vector<TRestGeant4Track>& GetTracks() const { return fTracks; }
    inline size_t GetNumberOfTracks() const { return fTracks.size(); }
    inline Int_t GetNumberOfActiveVolumes() const { return fNVolumes; }

    inline Int_t isVolumeStored(int n) const { return fVolumeStored[n]; }
    inline const TRestGeant4Track& GetTrack(int n) const { return fTracks[n]; }
    inline TRestGeant4Track* GetTrackPointer(int n) { return &fTracks[n]; }
    TRestGeant4Track* GetTrackByID(Int_t trackID) const;
    inline Int_t GetNumberOfSubEventIDTracks() const { return fMaxSubEventID + 1; }

    inline Double_t GetTotalDepositedEnergy() const { return fTotalDepositedEnergy; }
    inline Double_t GetEnergyDepositedInVolume(Int_t volID) const { return fVolumeDepositedEnergy[volID]; }
    inline Double_t GetSensitiveVolumeEnergy() const { return fSensitiveVolumeEnergy; }
    TVector3 GetMeanPositionInVolume(Int_t volID) const;
    TVector3 GetFirstPositionInVolume(Int_t volID) const;
    TVector3 GetLastPositionInVolume(Int_t volID) const;
    TVector3 GetPositionDeviationInVolume(Int_t volID) const;

    std::map<std::string, std::map<std::string, std::map<std::string, double>>>
    GetEnergyInVolumePerParticlePerProcessMap() const;
    std::map<std::string, std::map<std::string, double>> GetEnergyInVolumePerProcessMap() const;
    std::map<std::string, std::map<std::string, double>> GetEnergyInVolumePerParticleMap() const;
    std::map<std::string, double> GetEnergyPerProcessMap() const;
    std::map<std::string, double> GetEnergyPerParticleMap() const;
    std::map<std::string, double> GetEnergyInVolumeMap() const;

    TRestHits GetHits(Int_t volID = -1) const;
    inline TRestHits GetHitsInVolume(Int_t volID) const { return GetHits(volID); }

    Int_t GetNumberOfTracksForParticle(const TString& parName) const;
    Int_t GetEnergyDepositedByParticle(const TString& parName) const;

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

    std::set<std::string> GetUniqueParticles() const;

    Bool_t ContainsProcessInVolume(Int_t processID, Int_t volumeID = -1) const;
    inline Bool_t ContainsProcess(Int_t processID) const { return ContainsProcessInVolume(processID, -1); }

    Bool_t ContainsProcessInVolume(const TString& processName, Int_t volumeID = -1) const;
    inline Bool_t ContainsProcess(const TString& processName) const {
        return ContainsProcessInVolume(processName, -1);
    }

    Bool_t ContainsParticle(const TString& particleName) const;
    Bool_t ContainsParticleInVolume(const TString& particleName, Int_t volumeID = -1) const;

    void Initialize() override;

    void InitializeReferences(TRestRun* run) override;

    const TRestGeant4Metadata* GetGeant4Metadata(const char* name = "TRestGeant4Metadata") const;

    /// maxTracks : number of tracks to print, 0 = all
    void PrintActiveVolumes() const;
    void PrintEvent(int maxTracks = 0, int maxHits = 0) const;

    inline TPad* DrawEvent(const TString& option = "") override { return DrawEvent(option, true); }
    TPad* DrawEvent(const TString& option, Bool_t autoBoundaries);

    // Constructor
    TRestGeant4Event();
    // Destructor
    virtual ~TRestGeant4Event();

    ClassDefOverride(TRestGeant4Event, 8);  // REST event superclass

    // restG4
   public:
    TRestGeant4Event(const G4Event*);  //! // Implemented in restG4
    bool InsertTrack(const G4Track*);  //!
    void UpdateTrack(const G4Track*);  //!
    void InsertStep(const G4Step*);    //!

    friend class OutputManager;

   private:
    std::map<Int_t, Int_t> fTrackIDToTrackIndex = {};
    TRestGeant4Hits fInitialStep;  //!
};
#endif
