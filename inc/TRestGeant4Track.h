///_______________________________________________________________________________
///_______________________________________________________________________________
///_______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestTrack.h
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#ifndef RestCore_TRestGeant4Track
#define RestCore_TRestGeant4Track

#include <TArrayI.h>
#include <TColor.h>
#include <TRestGeant4Hits.h>
#include <TString.h>
#include <TVector3.h>

#include <iostream>
#include <vector>

#include "TObject.h"

class TRestGeant4Event;
class TRestGeant4Metadata;

// Perhaps there might be need for a mother class TRestTrack (if there is future need)
class TRestGeant4Track : public TObject {
   protected:
    Int_t fTrack_ID;
    Int_t fParent_ID;

    Int_t fSubEventId;

    // We must change this to save space! (Might be not needed after all)
    TString fParticleName;

    Double_t fGlobalTimestamp;  // in seconds precision
    Double_t fTrackTimestamp;   // in ns precision (seconds have been removed)
    Double_t fKineticEnergy;

    TRestGeant4Hits fHits;

    TVector3 fTrackOrigin;

    TRestGeant4Event* fEvent = nullptr;  //!

   public:
    inline void Initialize() {
        RemoveHits();
        fSubEventId = 0.;
    }

    inline const TRestGeant4Hits& GetHits() const { return fHits; }

    inline const TRestGeant4Event* GetEvent() const { return fEvent; }
    inline void SetEvent(TRestGeant4Event* event) { fEvent = event; }

    inline Double_t GetEnergy() const { return fHits.GetEnergy(); }

    size_t GetNumberOfHits(Int_t volID = -1) const;
    inline Int_t GetTrackID() const { return fTrack_ID; }
    inline Int_t GetParentID() const { return fParent_ID; }

    inline TString GetParticleName() const { return fParticleName; }
    EColor GetParticleColor() const;

    inline Double_t GetGlobalTime() const { return fGlobalTimestamp; }
    inline Double_t GetTrackTimeLength() const { return fTrackTimestamp; }
    inline Double_t GetKineticEnergy() const { return fKineticEnergy; }
    inline Double_t GetTotalDepositedEnergy() const { return fHits.GetTotalDepositedEnergy(); }
    inline TVector3 GetTrackOrigin() const { return fTrackOrigin; }
    inline Int_t GetSubEventID() const { return fSubEventId; }

    inline Double_t GetEnergyInVolume(Int_t volID) const { return fHits.GetEnergyInVolume(volID); }
    inline TVector3 GetMeanPositionInVolume(Int_t volID) const {
        return fHits.GetMeanPositionInVolume(volID);
    }
    inline TVector3 GetFirstPositionInVolume(Int_t volID) const {
        return fHits.GetFirstPositionInVolume(volID);
    }
    inline TVector3 GetLastPositionInVolume(Int_t volID) const {
        return fHits.GetLastPositionInVolume(volID);
    }

    void SetSubEventID(Int_t id) { fSubEventId = id; }
    void SetTrackID(Int_t id) { fTrack_ID = id; }
    void SetParentID(Int_t id) { fParent_ID = id; }
    //       void SetParticleID( Int_t id ) { fParticle_ID = id; }
    void SetParticleName(const TString& particleName) { fParticleName = particleName; }
    void SetGlobalTrackTime(Double_t time) { fGlobalTimestamp = time; }
    void SetTrackTimeLength(Double_t time) { fTrackTimestamp = time; }
    void SetKineticEnergy(Double_t en) { fKineticEnergy = en; }
    void SetTrackOrigin(const TVector3& pos) { fTrackOrigin = pos; }
    void SetTrackOrigin(Double_t x, Double_t y, Double_t z) { fTrackOrigin.SetXYZ(x, y, z); }

    void AddG4Hit(const TVector3& pos, Double_t en, Double_t hit_global_time, Int_t pcs, Int_t vol,
                  Double_t eKin, const TVector3& momentumDirection) {
        fHits.AddG4Hit(pos, en, hit_global_time, pcs, vol, eKin, momentumDirection);
    }

    Double_t GetTrackLength() const;

    inline static Double_t GetDistance(const TVector3& v1, const TVector3& v2) {
        return TMath::Sqrt((v1.X() - v2.X()) * (v1.X() - v2.X()) + (v1.Y() - v2.Y()) * (v1.Y() - v2.Y()) +
                           (v1.Z() - v2.Z()) * (v1.Z() - v2.Z()));
    }

    inline void RemoveHits() { fHits.RemoveHits(); }

    const TRestGeant4Metadata* GetGeant4Metadata() const;

    Int_t GetProcessID(const TString& processName) const;
    TString GetProcessName(Int_t id) const;

    Bool_t ContainsProcessInVolume(Int_t processID, Int_t volumeID = -1) const;
    inline Bool_t ContainsProcess(Int_t processID) const { return ContainsProcessInVolume(processID, -1); }

    /// Prints the track information. N number of hits to print, 0 = all
    void PrintTrack(size_t maxHits = 0) const;

    // Constructor
    TRestGeant4Track();
    // Destructor
    virtual ~TRestGeant4Track();

    ClassDef(TRestGeant4Track, 3);  // REST event superclass
};

#endif
