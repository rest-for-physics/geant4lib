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

#ifndef RestGeant4Lib_TRestGeant4Event
#define RestGeant4Lib_TRestGeant4Event

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

class vector;

/// An event class to store geant4 generated event information
class TRestGeant4Event : public TRestEvent {
   private:
    Double_t fMinX, fMaxX;            //!
    Double_t fMinY, fMaxY;            //!
    Double_t fMinZ, fMaxZ;            //!
    Double_t fMinEnergy, fMaxEnergy;  //!

    void AddEnergyDepositToVolume(Int_t volID, Double_t eDep);

   protected:
    /*
     * Data members should be placed here if possible
     */
    vector<TString> fPrimaryParticleName;

    vector<TVector3> fPrimaryEventOrigin;
    vector<TVector3> fPrimaryEventDirection;
    vector<Double_t> fPrimaryEventEnergy;

    Double_t fTotalDepositedEnergy;
    Double_t fSensitiveVolumeEnergy;

    Int_t fNVolumes;
    vector<Int_t> fVolumeStored;
    vector<string> fVolumeStoredNames;
    vector<Double_t> fVolumeDepositedEnergy;

    Int_t fNTracks;
    vector<TRestGeant4Track> fTrack;

    Int_t fMaxSubEventID;

    /* ---------------------------------------------------------------------------------------------------- */

    // TODO These graphs should be placed in TRestTrack?
    // (following GetGraph implementation in TRestDetectorSignal)
    TGraph* fXZHitGraph;  //!
    TGraph* fYZHitGraph;  //!
    TGraph* fXYHitGraph;  //!
    // TGraph2D *fXYZHitGraph; //! (TODO to implement XYZ visualization)

    vector<TGraph*> fXYPcsMarker;  //!
    vector<TGraph*> fYZPcsMarker;  //!
    vector<TGraph*> fXZPcsMarker;  //!

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

    vector<Int_t> legendAdded;  //!

    Int_t fTotalHits;  //!

    TMultiGraph* GetXZMultiGraph(Int_t gridElement, vector<TString> pcsList, Double_t minPointSize = 0.4,
                                 Double_t maxPointSize = 4);
    TMultiGraph* GetYZMultiGraph(Int_t gridElement, vector<TString> pcsList, Double_t minPointSize = 0.4,
                                 Double_t maxPointSize = 4);
    TMultiGraph* GetXYMultiGraph(Int_t gridElement, vector<TString> pcsList, Double_t minPointSize = 0.4,
                                 Double_t maxPointSize = 4);

    TH2F* GetXYHistogram(Int_t gridElement, vector<TString> optList);
    TH2F* GetXZHistogram(Int_t gridElement, vector<TString> optList);
    TH2F* GetYZHistogram(Int_t gridElement, vector<TString> optList);

    TH1D* GetXHistogram(Int_t gridElement, vector<TString> optList);
    TH1D* GetYHistogram(Int_t gridElement, vector<TString> optList);
    TH1D* GetZHistogram(Int_t gridElement, vector<TString> optList);

   public:
    void SetBoundaries();
    void SetBoundaries(Double_t xMin, Double_t xMax, Double_t yMin, Double_t yMax, Double_t zMin,
                       Double_t zMax);

    TString GetPrimaryEventParticleName(int n) {
        if (fPrimaryParticleName.size() > n) return fPrimaryParticleName[n];
        return "Not defined";
    }

    TVector3 GetPrimaryEventDirection(Int_t n = 0) { return fPrimaryEventDirection[n]; }
    TVector3 GetPrimaryEventOrigin(Int_t n = 0) { return fPrimaryEventOrigin[n]; }
    Double_t GetPrimaryEventEnergy(Int_t n = 0) { return fPrimaryEventEnergy[n]; }

    Int_t GetNumberOfHits(Int_t volID = -1);
    Int_t GetNumberOfTracks() { return fNTracks; }
    Int_t GetNumberOfPrimaries() { return fPrimaryEventDirection.size(); }
    Int_t GetNumberOfActiveVolumes() { return fNVolumes; }

    Int_t isVolumeStored(int n) { return fVolumeStored[n]; }
    TRestGeant4Track* GetTrack(int n) { return &fTrack[n]; }
    TRestGeant4Track* GetTrackByID(int id);
    Int_t GetNumberOfSubEventIDTracks() { return fMaxSubEventID + 1; }

    Double_t GetTotalDepositedEnergy() { return fTotalDepositedEnergy; }
    Double_t GetTotalDepositedEnergyFromTracks();
    Double_t GetEnergyDepositedInVolume(Int_t volID) { return fVolumeDepositedEnergy[volID]; }
    Double_t GetSensitiveVolumeEnergy() { return fSensitiveVolumeEnergy; }
    TVector3 GetMeanPositionInVolume(Int_t volID);
    TVector3 GetFirstPositionInVolume(Int_t volID);
    TVector3 GetLastPositionInVolume(Int_t volID);
    TVector3 GetPositionDeviationInVolume(Int_t volID);

    TRestHits GetHits(Int_t volID = -1);
    TRestHits GetHitsInVolume(Int_t volID) { return GetHits(volID); }

    Int_t GetNumberOfTracksForParticle(const TString& parName);
    Int_t GetEnergyDepositedByParticle(const TString& parName);

    inline void SetPrimaryEventOrigin(TVector3 position) { fPrimaryEventOrigin.push_back(position); }
    inline void SetPrimaryEventDirection(TVector3 direction) { fPrimaryEventDirection.push_back(direction); }
    inline void SetPrimaryEventParticleName(TString parName) { fPrimaryParticleName.push_back(parName); }
    inline void SetPrimaryEventEnergy(Double_t energy) { fPrimaryEventEnergy.push_back(energy); }

    void ActivateVolumeForStorage(Int_t n) { fVolumeStored[n] = 1; }
    void DisableVolumeForStorage(Int_t n) { fVolumeStored[n] = 0; }

    void AddActiveVolume(const string& volumeName);
    void ClearVolumes();
    void AddEnergyToSensitiveVolume(Double_t en) { fSensitiveVolumeEnergy += en; }

    void SetEnergyDepositedInVolume(Int_t volID, Double_t eDep) { fVolumeDepositedEnergy[volID] = eDep; }
    void SetSensitiveVolumeEnergy(Double_t en) { fSensitiveVolumeEnergy = en; }

    Int_t GetLowestTrackID() {
        Int_t lowestID = 0;
        if (fNTracks > 0) lowestID = GetTrack(0)->GetTrackID();

        for (int i = 0; i < fNTracks; i++) {
            TRestGeant4Track* tr = GetTrack(i);
            if (tr->GetTrackID() < lowestID) lowestID = tr->GetTrackID();
        }

        return lowestID;
    }

    void SetTrackSubEventID(Int_t n, Int_t id);
    void AddTrack(TRestGeant4Track trk);

    void Initialize();

    /// maxTracks : number of tracks to print, 0 = all
    void PrintActiveVolumes();
    void PrintEvent(int maxTracks = 0, int maxHits = 0);

    TPad* DrawEvent(TString option = "") { return DrawEvent(option, true); }
    TPad* DrawEvent(const TString& option, Bool_t autoBoundaries);

    // Constructor
    TRestGeant4Event();
    // Destructor
    virtual ~TRestGeant4Event();

    ClassDef(TRestGeant4Event, 6);  // REST event superclass
};
#endif
