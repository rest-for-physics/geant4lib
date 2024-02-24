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

#include <TColor.h>
#include <TString.h>
#include <TVector3.h>

#include "TRestGeant4Hits.h"

class TRestGeant4Event;
class TRestGeant4Metadata;
class G4Track;
class G4Step;

// Perhaps there might be need for a mother class TRestTrack (if there is future need)
class TRestGeant4Track {
   protected:
    Int_t fTrackID;
    Int_t fParentID;

    TString fParticleName;

    TRestGeant4Hits fHits;

    TString fCreatorProcess;

    std::vector<Int_t> fSecondaryTrackIDs;

    Double_t fGlobalTimestamp;
    Double_t fTimeLength;

    Double_t fInitialKineticEnergy;
    Double_t fLength;

    TVector3 fInitialPosition;

    Double_t fWeight = 1;  // Used for biasing

    TRestGeant4Event* fEvent = nullptr;  //!

   public:
    inline const TRestGeant4Hits& GetHits() const { return fHits; }
    inline TRestGeant4Hits* GetHitsPointer() { return &fHits; }
    inline const TRestGeant4Event* GetEvent() const { return fEvent; }
    const TRestGeant4Metadata* GetGeant4Metadata() const;

    inline void SetEvent(TRestGeant4Event* event) { fEvent = event; }
    inline void SetHits(const TRestGeant4Hits& hits) {
        fHits = hits;
        fHits.SetTrack(this);
    }

    inline TString GetCreatorProcess() const { return fCreatorProcess; }

    inline void AddSecondaryTrackID(Int_t trackID) { fSecondaryTrackIDs.push_back(trackID); }

    size_t GetNumberOfHits(Int_t volID = -1) const;
    size_t GetNumberOfPhysicalHits(Int_t volID = -1) const;

    inline Int_t GetTrackID() const { return fTrackID; }
    inline Int_t GetParentID() const { return fParentID; }
    inline TString GetParticleName() const { return fParticleName; }
    inline Double_t GetGlobalTime() const { return fGlobalTimestamp; }
    inline Double_t GetTimeLength() const { return fTimeLength; }
    inline Double_t GetInitialKineticEnergy() const { return fInitialKineticEnergy; }
    inline TVector3 GetInitialPosition() const { return fInitialPosition; }
    inline Double_t GetWeight() const { return fWeight; }
    inline Double_t GetTotalEnergy() const { return fHits.GetTotalEnergy(); }
    inline Double_t GetLength() const { return fLength; }

    TString GetInitialVolume() const;
    TString GetFinalVolume() const;

    inline std::vector<Int_t> GetSecondaryTrackIDs() const { return fSecondaryTrackIDs; }
    std::vector<const TRestGeant4Track*> GetSecondaryTracks() const;
    inline std::vector<const TRestGeant4Track*> GetChildrenTracks() const { return GetSecondaryTracks(); }

    TRestGeant4Track* GetParentTrack() const;

    inline TVector3 GetTrackOrigin() const { return GetInitialPosition(); }

    EColor GetParticleColor() const;

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

    Int_t GetProcessID(const TString& processName) const;
    TString GetProcessName(Int_t id) const;

    Bool_t ContainsProcessInVolume(Int_t processID, Int_t volumeID = -1) const;
    inline Bool_t ContainsProcess(Int_t processID) const { return ContainsProcessInVolume(processID, -1); }

    Bool_t ContainsProcessInVolume(const TString& processName, Int_t volumeID = -1) const;
    inline Bool_t ContainsProcess(const TString& processName) const {
        return ContainsProcessInVolume(processName, -1);
    }

    Double_t GetEnergyInVolume(const TString& volumeName, bool children = false) const;

    TString GetLastProcessName() const;

    /// Prints the track information. N number of hits to print, 0 = all
    void PrintTrack(size_t maxHits = 0) const;

    inline void RemoveHits() { fHits.RemoveHits(); }

    // Constructor
    TRestGeant4Track();

    // Destructor
    virtual ~TRestGeant4Track();

    friend class TRestGeant4Event;  // allows TRestGeant4Event to access private members

    ClassDef(TRestGeant4Track, 5);  // REST event superclass

    // restG4
   public:
    explicit TRestGeant4Track(const G4Track*);  //!
    void UpdateTrack(const G4Track*);           //!
    void InsertStep(const G4Step*);             //!
};

#endif
