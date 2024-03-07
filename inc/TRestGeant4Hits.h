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

#include <TRestHits.h>

#include <iostream>

#include "TRestGeant4Metadata.h"

class G4Step;
class TRestGeant4Track;
class TRestGeant4Event;

class TRestGeant4Hits : public TRestHits {
   protected:
    std::vector<Int_t> fProcessID = {};
    std::vector<Int_t> fVolumeID = {};
    std::vector<Float_t> fKineticEnergy = {};
    std::vector<TVector3> fMomentumDirection = {};

    std::vector<std::string> fHadronicTargetIsotopeName = {};
    std::vector<int> fHadronicTargetIsotopeA = {};
    std::vector<int> fHadronicTargetIsotopeZ = {};

    TRestGeant4Track* fTrack = nullptr;  //!
    TRestGeant4Event* fEvent = nullptr;  //!

   public:
    TRestGeant4Metadata* GetGeant4Metadata() const;

    inline const TRestGeant4Track* GetTrack() const { return fTrack; }
    inline void SetTrack(TRestGeant4Track* track) { fTrack = track; }

    inline const TRestGeant4Event* GetEvent() const { return fEvent; }
    inline void SetEvent(TRestGeant4Event* event) { fEvent = event; }

    inline TVector3 GetMomentumDirection(size_t n) const { return fMomentumDirection[n]; }

    inline Int_t GetProcessId(size_t n) const { return fProcessID[n]; }
    inline Int_t GetProcess(size_t n) const { return GetProcessId(n); }
    inline Int_t GetHitProcess(size_t n) const { return GetProcessId(n); }
    TString GetProcessName(size_t n) const;

    inline Int_t GetVolumeId(size_t n) const { return fVolumeID[n]; }
    inline Int_t GetHitVolume(size_t n) const { return GetVolumeId(n); }
    TString GetVolumeName(size_t n) const;

    inline bool GetHadronicOk() const { return fHadronicTargetIsotopeName.size() > 0; }
    inline std::string GetHadronicTargetIsotopeName(size_t n) const { return fHadronicTargetIsotopeName[n]; }
    inline int GetHadronicTargetIsotopeA(size_t n) const { return fHadronicTargetIsotopeA[n]; }
    inline int GetHadronicTargetIsotopeZ(size_t n) const { return fHadronicTargetIsotopeZ[n]; }

    void RemoveG4Hits();

    inline Double_t GetKineticEnergy(size_t n) const { return fKineticEnergy[n]; }

    Double_t GetEnergyInVolume(Int_t volumeID) const;

    TVector3 GetMeanPositionInVolume(Int_t volumeID) const;
    TVector3 GetFirstPositionInVolume(Int_t volumeID) const;
    TVector3 GetLastPositionInVolume(Int_t volumeID) const;

    size_t GetNumberOfHitsInVolume(Int_t volumeID) const;

    // non-const methods (should only be used on the analysis, carefully)
    std::vector<Float_t>& GetEnergyRef() { return fEnergy; }

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
