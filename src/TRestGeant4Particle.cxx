///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4Particle.cxx
///
///             Base class from which to inherit all other event classes in REST
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#include "TRestGeant4Particle.h"

using namespace std;

ClassImp(TRestGeant4Particle);

TRestGeant4Particle::TRestGeant4Particle() : fExcitationLevel(0), fCharge(0){};

TRestGeant4Particle::~TRestGeant4Particle() = default;

void TRestGeant4Particle::SetParticle(TRestGeant4Particle particle) {
    fExcitationLevel = particle.GetExcitationLevel();
    fParticleName = particle.GetParticleName();
    fEnergy = particle.GetEnergy();
    fDirection = particle.GetMomentumDirection();
    fOrigin = particle.fOrigin;
}