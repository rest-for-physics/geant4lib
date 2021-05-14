//
// Created by lobis on 14/05/2021.
//

#ifndef GEANT4LIB_TRESTGEANT4EXPERIMENTALEVENT_H
#define GEANT4LIB_TRESTGEANT4EXPERIMENTALEVENT_H

#include <TString.h>
#include <TVector3.h>

#include <map>

#include "TRestGeant4ExperimentalTrack.h"

class TRestGeant4ExperimentalEvent : public TObject {
   public:
    TRestGeant4ExperimentalEvent();
    ~TRestGeant4ExperimentalEvent();

   protected:
    Int_t fEventID;
    Int_t fSubEventID;

    Double_t fGenerationTime;

    TString fGenerationVolumeName;

    Double_t fGenerationKineticEnergy;

    TVector3 fGenerationPosition;
    TVector3 fGenerationMomentum;

    std::map<TString, Double_t> fTrackLengthInVolume;
    std::map<TString, Double_t> fEnergyInVolume;

    Double_t fWeight;

    ClassDef(TRestGeant4ExperimentalEvent, 1);
};

#endif  // GEANT4LIB_TRESTGEANT4EXPERIMENTALEVENT_H
