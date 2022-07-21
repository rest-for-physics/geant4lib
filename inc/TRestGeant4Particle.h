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

#ifndef RestCore_TRestGeant4Particle
#define RestCore_TRestGeant4Particle

#include <TString.h>
#include <TVector3.h>

#include <iostream>

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

    void SetParticle(TRestGeant4Particle particle) {
        fExcitationLevel = particle.GetExcitationLevel();
        fParticleName = particle.GetParticleName();
        fEnergy = particle.GetEnergy();
        fDirection = particle.GetMomentumDirection();
        fOrigin = particle.fOrigin;
    }

    void SetParticleName(TString particle) { fParticleName = particle; }
    void SetExcitationLevel(Double_t excitationEnergy) {
        fExcitationLevel = excitationEnergy;
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

    ClassDef(TRestGeant4Particle, 4);
};
#endif
