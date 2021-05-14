//
// Created by lobis on 14/05/2021.
//

#ifndef GEANT4LIB_TRestGeant4ExperimentalStep_H
#define GEANT4LIB_TRestGeant4ExperimentalStep_H

#include <TObject.h>
#include <TString.h>

#include "TVector3.h"

class TRestGeant4ExperimentalStep : public TObject {
   private:
    TString fProcessName;
    TString fProcessType;

    // TString fVolumeName;
    TString fPreStepVolumeName;
    TString fPostStepVolumeName;

    TVector3 fPosition;
    TVector3 fDirection;

    Double_t fEnergy;

    Double_t fTimeGlobal;

    Bool_t fIsFirstStepInVolume;
    Int_t fNumberOfSecondaries;

   public:
    // Constructor & Destructor
    TRestGeant4ExperimentalStep();
    ~TRestGeant4ExperimentalStep();

    // TODO: Add these methods when design is finished
    // Copy Constructor and assignment operator
    // TRestGeant4ExperimentalStep(const TRestGeant4ExperimentalStep&);
    // TRestGeant4ExperimentalStep& operator=(const TRestGeant4ExperimentalStep&);

    ClassDef(TRestGeant4ExperimentalStep, 1);
};

#endif  // GEANT4LIB_TRestGeant4ExperimentalStep_H
