//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4Particle.h
///
///             Class to store a particle definition
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestGeant4Lib_TRestGeant4Particle
#define RestGeant4Lib_TRestGeant4Particle

#include <TObject.h>
#include <TString.h>
#include <TVector3.h>

#include <iostream>

class TRestGeant4Particle : public TObject {
   protected:
    TString fParticleName;
    Double_t fExcitationLevel = 0;
    TVector3 fDirection;
    Double_t fEnergy;
    Int_t fCharge = 0;
    TVector3 fOrigin;

   public:
    TString GetParticleName() { return fParticleName; }
    Double_t GetExcitationLevel() { return fExcitationLevel; }
    Double_t GetEnergy() { return fEnergy; }
    TVector3 GetMomentumDirection() { return fDirection; }
    Int_t GetParticleCharge() { return fCharge; }
    TVector3 GetOrigin() { return fOrigin; }

    void SetParticle(TRestGeant4Particle particle) {
        fExcitationLevel = particle.GetExcitationLevel();
        fParticleName = particle.GetParticleName();
        fEnergy = particle.GetEnergy();
        fDirection = particle.GetMomentumDirection();
        fOrigin = particle.fOrigin;
    }

    void SetParticleName(TString particle) { fParticleName = particle; }
    void SetExcitationLevel(Double_t eEnergy) {
        fExcitationLevel = eEnergy;
        if (fExcitationLevel < 0) fExcitationLevel = 0;
    }

    void SetParticleCharge(Int_t charge) { fCharge = charge; }

    void SetDirection(TVector3 dir) { fDirection = dir; }
    void SetEnergy(Double_t en) { fEnergy = en; }
    void SetOrigin(TVector3 pos) { fOrigin = pos; }

    // Constructor
    TRestGeant4Particle();
    // Destructor
    virtual ~TRestGeant4Particle();

    ClassDef(TRestGeant4Particle, 3);
};
#endif
