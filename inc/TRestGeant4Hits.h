///_______________________________________________________________________________
///_______________________________________________________________________________
///_______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4Hits.h
///
///             Base class from which to inherit all other event classes in REST
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#ifndef RestCore_TRestGeant4Hits
#define RestCore_TRestGeant4Hits

#include <TArrayF.h>
#include <TArrayI.h>
#include <TRestHits.h>

#include <iostream>

#include "TRestGeant4Metadata.h"

class G4Step;
class TRestGeant4Track;
class TRestGeant4Event;

class TRestGeant4Hits : public TRestHits {
   protected:
    std::vector<Int_t> fProcessID;
    std::vector<Int_t> fVolumeID;
    std::vector<Float_t> fKineticEnergy;

    TRestGeant4Track* fTrack = nullptr;  //!
    TRestGeant4Event* fEvent = nullptr;  //!

   public:
    TRestGeant4Metadata* GetGeant4Metadata() const;

    const TRestGeant4Track* GetTrack() const { return fTrack; }
    void SetTrack(TRestGeant4Track* track) { fTrack = track; }

    const TRestGeant4Event* GetEvent() const { return fEvent; }
    void SetEvent(TRestGeant4Event* event) { fEvent = event; }

    TArrayF fMomentumDirectionX;
    TArrayF fMomentumDirectionY;
    TArrayF fMomentumDirectionZ;

    TVector3 GetMomentumDirection(int n) const {
        return {fMomentumDirectionX[n], fMomentumDirectionY[n], fMomentumDirectionZ[n]};
    }

    Int_t GetProcess(int n) const { return fProcessID[n]; }

    void RemoveG4Hits();

    Int_t GetHitProcess(int n) const { return fProcessID[n]; }
    Int_t GetHitVolume(int n) const { return fVolumeID[n]; }
    Int_t GetVolumeId(int n) const { return fVolumeID[n]; }
    Double_t GetKineticEnergy(int n) const { return fKineticEnergy[n]; }

    Double_t GetEnergyInVolume(Int_t volumeID) const;

    TVector3 GetMeanPositionInVolume(Int_t volumeID) const;
    TVector3 GetFirstPositionInVolume(Int_t volumeID) const;
    TVector3 GetLastPositionInVolume(Int_t volumeID) const;

    size_t GetNumberOfHitsInVolume(Int_t volumeID) const;

    // Constructor
    TRestGeant4Hits();
    // Destructor
    virtual ~TRestGeant4Hits();

    ClassDef(TRestGeant4Hits, 8);  // REST event superclass

    // restG4
   public:
    void InsertStep(const G4Step*);
};
#endif
