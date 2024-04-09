/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

#ifndef RestCore_TRestGeant4Particle
#define RestCore_TRestGeant4Particle

#include <TString.h>
#include <TVector3.h>

#include <iostream>

/// A class used to store particle properties
class TRestGeant4Particle {
   protected:
    TString fParticleName;
    Double_t fExcitationLevel = 0;
    TVector3 fDirection = {1, 0, 0};
    Double_t fEnergy = 0;
    Int_t fCharge = 0;
    TVector3 fOrigin;

   public:
    inline TString GetParticleName() const { return fParticleName; }
    inline Double_t GetExcitationLevel() const { return fExcitationLevel; }
    inline Double_t GetEnergy() const { return fEnergy; }
    inline TVector3 GetMomentumDirection() const { return fDirection; }
    inline Int_t GetParticleCharge() const { return fCharge; }
    inline TVector3 GetOrigin() const { return fOrigin; }

    void SetParticle(TRestGeant4Particle particle);

    void SetParticleName(TString particle) { fParticleName = particle; }

    void SetExcitationLevel(Double_t excitationEnergy) {
        fExcitationLevel = excitationEnergy;
        if (fExcitationLevel < 0) fExcitationLevel = 0;
    }

    void SetParticleCharge(Int_t charge) { fCharge = charge; }

    void SetDirection(const TVector3& dir) { fDirection = dir.Unit(); }
    void SetEnergy(Double_t en) { fEnergy = en; }
    void SetOrigin(const TVector3& pos) { fOrigin = pos; }

    void Print() const;

    // Constructor
    TRestGeant4Particle();
    // Destructor
    virtual ~TRestGeant4Particle();

    ClassDef(TRestGeant4Particle, 4);
};
#endif
