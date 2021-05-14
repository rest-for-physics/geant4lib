///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
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

#ifndef RestGeant4Lib_TRestGeant4Track
#define RestGeant4Lib_TRestGeant4Track

#include <TArrayI.h>
#include <TColor.h>
#include <TObject.h>
#include <TRestGeant4Hits.h>
#include <TString.h>
#include <TVector3.h>

#include <iostream>

// Perhaps there might be need for a mother class TRestTrack (if there is future
// need)
class TRestGeant4Track : public TObject {
   protected:
    Int_t fTrack_ID;
    Int_t fParent_ID;

    Int_t fSubEventId;

    // We must change this to save space! (Might be not needed afterall)
    // Int_t fParticle_ID;
    TString fParticleName;

    Double_t fGlobalTimestamp;  // in seconds precision
    Double_t fTrackTimestamp;   // in ns precision (seconds have been removed)
    Double_t fKineticEnergy;

    TRestGeant4Hits fHits;

    TVector3 fTrackOrigin;

   public:
    void Initialize() {
        RemoveHits();
        fSubEventId = 0.;
    }

    TRestGeant4Hits* GetHits() { return &fHits; }

    Double_t GetEnergy() { return GetHits()->GetEnergy(); }

    Int_t GetNumberOfHits(Int_t volID = -1);
    Int_t GetTrackID() { return fTrack_ID; }
    Int_t GetParentID() { return fParent_ID; }

    TString GetParticleName() { return fParticleName; }
    EColor GetParticleColor();

    Double_t GetGlobalTime() { return fGlobalTimestamp; }
    Double_t GetTrackTimeLength() { return fTrackTimestamp; }
    Double_t GetKineticEnergy() { return fKineticEnergy; }
    Double_t GetTotalDepositedEnergy() { return fHits.GetTotalDepositedEnergy(); }
    TVector3 GetTrackOrigin() { return fTrackOrigin; }
    Int_t GetSubEventID() { return fSubEventId; }

    Double_t GetEnergyInVolume(Int_t volID) { return GetHits()->GetEnergyInVolume(volID); }
    TVector3 GetMeanPositionInVolume(Int_t volID) { return GetHits()->GetMeanPositionInVolume(volID); }
    TVector3 GetFirstPositionInVolume(Int_t volID) { return GetHits()->GetFirstPositionInVolume(volID); }
    TVector3 GetLastPositionInVolume(Int_t volID) { return GetHits()->GetLastPositionInVolume(volID); }

    void SetSubEventID(Int_t id) { fSubEventId = id; }

    void SetTrackID(Int_t id) { fTrack_ID = id; }
    void SetParentID(Int_t id) { fParent_ID = id; }
    //       void SetParticleID( Int_t id ) { fParticle_ID = id; }
    void SetParticleName(TString ptcleName) { fParticleName = ptcleName; }
    void SetGlobalTrackTime(Double_t time) { fGlobalTimestamp = time; }
    void SetTrackTimeLength(Double_t time) { fTrackTimestamp = time; }
    void SetKineticEnergy(Double_t en) { fKineticEnergy = en; }
    void SetTrackOrigin(TVector3 pos) { fTrackOrigin = pos; }
    void SetTrackOrigin(Double_t x, Double_t y, Double_t z) { fTrackOrigin.SetXYZ(x, y, z); }

    void AddG4Hit(TVector3 pos, Double_t en, Double_t hit_global_time, Int_t pcs, Int_t vol, Double_t eKin,
                  TVector3 momentumDirection) {
        fHits.AddG4Hit(pos, en, hit_global_time, pcs, vol, eKin, momentumDirection);
    }

    Double_t GetTrackLength();

    Double_t GetDistance(TVector3 v1, TVector3 v2) {
        return TMath::Sqrt((v1.X() - v2.X()) * (v1.X() - v2.X()) + (v1.Y() - v2.Y()) * (v1.Y() - v2.Y()) +
                           (v1.Z() - v2.Z()) * (v1.Z() - v2.Z()));
    }

    void RemoveHits() { fHits.RemoveHits(); }

    /// Prints the track information. N number of hits to print, 0 = all
    void PrintTrack(int maxHits = 0);

    //    Int_t GetElement( Int_t n ) { return X.At(n); }

    //    Int_t GetParticleID();
    // Constructor
    TRestGeant4Track();
    // Destructor
    virtual ~TRestGeant4Track();

    ClassDef(TRestGeant4Track, 2);  // REST event superclass
};

#endif
