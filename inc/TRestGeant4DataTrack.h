//
// Created by lobis on 11/10/2021.
//

#ifndef REST_TRESTGEANT4DATATRACK_H
#define REST_TRESTGEANT4DATATRACK_H

#include <vector>

#include "TRestGeant4DataSteps.h"

class G4Track;
class G4Step;

class TRestGeant4Track;

class TRestGeant4DataTrack {
   protected:
    Int_t fTrackID{};
    Int_t fParentID{};

    TString fParticleName;
    Int_t fParticleID{};

    TString fParticleType;
    TString fParticleSubType;

    TString fCreatorProcess;
    Int_t fNumberOfSecondaries{};
    Float_t fInitialKineticEnergy{};
    Float_t fTrackLength{};

    Float_t fWeight{};
    TRestGeant4DataSteps fSteps;

   public:
    inline TRestGeant4DataTrack() = default;
    explicit TRestGeant4DataTrack(const G4Track*);

   public:
    void UpdateTrack(const G4Track*);  //!
    void InsertStep(const G4Step*);    //!

    operator TRestGeant4Track() const;

    inline void UpdateSteps(const TRestGeant4DataSteps& steps) { fSteps = steps; }  //!

    ClassDef(TRestGeant4DataTrack, 1);
};
#endif  // REST_TRESTGEANT4DATATRACK_H
