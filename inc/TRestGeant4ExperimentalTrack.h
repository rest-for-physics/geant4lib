//
// Created by lobis on 14/05/2021.
//

#ifndef GEANT4LIB_TRESTGEANT4EXPERIMENTALTRACK_H
#define GEANT4LIB_TRESTGEANT4EXPERIMENTALTRACK_H

#include <TString.h>
#include <TVector3.h>

#include <map>

#include "TRestGeant4ExperimentalStep.h"

class TRestGeant4ExperimentalTrack : public TObject {
   public:
    TRestGeant4ExperimentalTrack();
    ~TRestGeant4ExperimentalTrack();

   protected:
    Int_t fTrackID;
    Int_t fParentID;

    std::vector<TRestGeant4ExperimentalStep> fSteps;

    Double_t fGenerationTime;

    TString fGenerationVolumeName;
    TString fGenerationProcessName;

    Double_t fGenerationKineticEnergy;
    TVector3 fGenerationPosition;
    TVector3 fGenerationMomentum;

    Double_t fTrackLength;

    Int_t fNumberOfSecondaries;

    std::map<TString, Double_t> fTrackLengthInVolume;
    std::map<TString, Double_t> fEnergyInVolume;

    Double_t fWeight;

    ClassDef(TRestGeant4ExperimentalTrack, 1);
};

#endif  // GEANT4LIB_TRESTGEANT4EXPERIMENTALTRACK_H
