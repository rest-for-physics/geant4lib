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
    TString fParticleType;

    Double_t fExcitationLevel;
    Int_t fCharge;

    TVector3 fDirection;
    TVector3 fOrigin;
    Double_t fEnergy;

   public:
    // Constructor and destructor
    TRestGeant4Particle();
    virtual ~TRestGeant4Particle();
    //

    // Setters and Getters
    inline TString GetParticleName() const { return fParticleName; }
    inline void SetParticleName(TString particle) { fParticleName = particle; }

    inline Double_t GetExcitationLevel() const { return fExcitationLevel; }
    inline void SetExcitationLevel(Double_t excitationLevel) {
        fExcitationLevel = fExcitationLevel < 0 ? 0 : excitationLevel;
    }

    inline Double_t GetEnergy() const { return fEnergy; }
    inline void SetEnergy(Double_t energy) { fEnergy = energy; }

    inline TVector3 GetMomentumDirection() const { return fDirection; }
    inline void SetMomentumDirection(TVector3 direction) { fDirection = direction; }

    inline Int_t GetParticleCharge() const { return fCharge; }
    inline void SetParticleCharge(Int_t charge) { fCharge = charge; }

    inline TVector3 GetOrigin() const { return fOrigin; }
    inline void SetOrigin(TVector3 origin) { fOrigin = origin; }

    void SetParticle(TRestGeant4Particle particle);

    ClassDef(TRestGeant4Particle, 3);
};
#endif
