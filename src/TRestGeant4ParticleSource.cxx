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

//#include "TFile.h"

#include "TRestGeant4ParticleSource.h"
#include "TRestStringOutput.h"
#include "TRestStringHelper.h"
#include "TRestReflector.h"
using namespace std;

// REST_Verbose_Level fLevel = REST_Essential;  //!
////	TRestLeveledOutput(REST_Verbose_Level& vref, string _color =
//// COLOR_RESET, string BorderOrHeader = "", REST_Display_Format style =
//// kBorderedLeft)
// TRestLeveledOutput<REST_Essential> metadata =
//    TRestLeveledOutput<REST_Essential>(fLevel, COLOR_BOLDGREEN, "||",
//                                       (REST_Display_Format)kBorderedMiddle);  //!

ClassImp(TRestGeant4ParticleSource)
    //______________________________________________________________________________
    TRestGeant4ParticleSource::TRestGeant4ParticleSource() {
    // TRestGeant4ParticleSource default constructor
    fAngularDistType = "flux";
    fEnergyDistType = "mono";
}

//______________________________________________________________________________
TRestGeant4ParticleSource::~TRestGeant4ParticleSource() {
    // TRestGeant4ParticleSource destructor
}

void TRestGeant4ParticleSource::PrintParticleSource() {
    metadata << "---------------------------------------" << endl;
    metadata << "Particle Source Name: " << GetParticleName() << endl;
    if (fParticlesTemplate.size() > 0 || fGenFilename!= "" ) {
        metadata << "Generator file: " << GetGenFilename() << endl;
        metadata << "Stored templates: " << fParticlesTemplate.size() << endl;
        metadata << "Particles: ";
        for (auto p : fParticles) metadata << p.GetParticleName() << ", ";
        metadata << endl;
    } else {
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

}

TRestGeant4ParticleSource* TRestGeant4ParticleSource::instantiate(std::string model) {
    if (model == "" || model == "geant4") {
        // use default generator
        return new TRestGeant4ParticleSource();
    } else {
        // use specific generator
        // naming convension: TRestGeant4ParticleSourceXXX
        // currently supported generator: decay0
        // in future we may add wrapper of generators: cry(for muon), pythia(for HEP), etc.
        model[0] = *REST_StringHelper::ToUpper(std::string(&model[0], 1)).c_str();
        TClass* c = TClass::GetClass(("TRestGeant4ParticleSource" + model).c_str());
        if (c != NULL)  // this means we have the package installed
        {
            return (TRestGeant4ParticleSource*)c->New();
        } else {
            std::cout << "REST ERROR! generator wrapper \"" << ("TRestGeant4ParticleSource" + model)
                      << "\" not found!" << std::endl;
        }
    }
    return NULL;
}

void TRestGeant4ParticleSource::Config(std::string configstring) {
}

// base class's generator action: randomize the particle's energy/direction with distribution file
void TRestGeant4ParticleSource::Update() {
    if (fParticlesTemplate.size() > 0) {
        // we use particle template to generate particles
        Int_t rndCollection = (Int_t)(fRndmMethod() * fParticlesTemplate.size());
        Int_t pCollectionID = rndCollection % fParticlesTemplate.size();
        fParticles = fParticlesTemplate[pCollectionID];
    } else {
        TRestGeant4Particle p(*this);
        // Future: implement particle generation for toy simulation
        //
        //

        fParticles = {p};
    }
}
