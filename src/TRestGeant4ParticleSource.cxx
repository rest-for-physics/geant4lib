///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4ParticleSource.cxx
///
///             Base class from which to inherit all other event classes in REST
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#include "TRestGeant4ParticleSource.h"

#include "TRestStringOutput.h"

using namespace std;

ClassImp(TRestGeant4ParticleSource);

TRestGeant4ParticleSource::TRestGeant4ParticleSource() = default;

TRestGeant4ParticleSource::~TRestGeant4ParticleSource() = default;

void TRestGeant4ParticleSource::PrintParticleSource() {
    metadata << "---------------------------------------" << endl;
    metadata << "Particle : " << GetParticle() << endl;
    metadata << "Charge : " << GetParticleCharge() << endl;
    metadata << "Angular distribution type : " << GetAngularDistType() << endl;
    if (GetAngularDistType() == "TH1D") {
        metadata << "Angular distribution filename : " << GetAngularFilename() << endl;
        metadata << "Angular histogram name  : " << GetAngularName() << endl;
    }
    metadata << "Direction : (" << GetDirection().X() << "," << GetDirection().Y() << ","
             << GetDirection().Z() << ")" << endl;
    metadata << "Energy distribution : " << GetEnergyDistType() << endl;
    if (GetEnergyDistType() == "TH1D") {
        metadata << "Energy distribution filename : " << GetSpectrumFilename() << endl;
        metadata << "Energy histogram name  : " << GetSpectrumName() << endl;
    }
    if (GetEnergyRange().X() == GetEnergyRange().Y())
        metadata << "Energy : " << GetEnergy() << " keV" << endl;
    else
        metadata << "Energy range : (" << GetEnergyRange().X() << "," << GetEnergyRange().Y() << ") keV"
                 << endl;
}
