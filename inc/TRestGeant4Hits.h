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

#include "TObject.h"
#include "TRestGeant4Metadata.h"

class G4Step;

class TRestGeant4Hits : public TRestHits {
   protected:
    TArrayI fVolumeID;
    TArrayI fProcessID;      // [fNHits]
    TArrayF fKineticEnergy;  // [fNHits]

   public:
    TArrayF fMomentumDirectionX;
    TArrayF fMomentumDirectionY;
    TArrayF fMomentumDirectionZ;

    TVector3 GetMomentumDirection(int n) const {
        return {fMomentumDirectionX[n], fMomentumDirectionY[n], fMomentumDirectionZ[n]};
    }

    Int_t GetProcess(int n) const { return fProcessID[n]; }

    void AddG4Hit(TVector3 pos, Double_t en, Double_t hit_global_time, Int_t process, Int_t volume,
                  Double_t eKin, TVector3 momentumDirection);
    void RemoveG4Hits();

    Int_t GetHitProcess(int n) const { return fProcessID[n]; }
    Int_t GetHitVolume(int n) const { return fVolumeID[n]; }
    Int_t GetVolumeId(int n) const { return fVolumeID[n]; }
    Double_t GetKineticEnergy(int n) const { return fKineticEnergy[n]; }

    Double_t GetEnergyInVolume(Int_t volID) const;

    TVector3 GetMeanPositionInVolume(Int_t volID) const;
    TVector3 GetFirstPositionInVolume(Int_t volID) const;
    TVector3 GetLastPositionInVolume(Int_t volID) const;

    // Constructor
    TRestGeant4Hits();
    // Destructor
    virtual ~TRestGeant4Hits();

    ClassDef(TRestGeant4Hits, 6);  // REST event superclass

    // restG4
   public:
    void InsertStep(const G4Step*, TRestGeant4Metadata&);
};
#endif
