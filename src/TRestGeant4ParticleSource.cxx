///_______________________________________________________________________________
///_______________________________________________________________________________
///_______________________________________________________________________________
///
///
///             RESTSoft: Software for Rare Event Searches with TPCs
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

#include <TRestReflector.h>
#include <TRestStringHelper.h>
#include <TRestStringOutput.h>

#include "TRestGeant4Metadata.h"

using namespace std;
using namespace TRestGeant4PrimaryGeneratorTypes;

ClassImp(TRestGeant4ParticleSource);

TRestGeant4ParticleSource::TRestGeant4ParticleSource() = default;

TRestGeant4ParticleSource::~TRestGeant4ParticleSource() = default;

void TRestGeant4ParticleSource::PrintMetadata() {
    RESTMetadata << " " << RESTendl;
    if (GetParticleName() != "" && GetParticleName() != "NO_SUCH_PARA")
        RESTMetadata << "Particle Source Name: " << GetParticleName() << RESTendl;
    if (!fParticlesTemplate.empty() && fGenFilename != "NO_SUCH_PARA") {
        RESTMetadata << "Generator file: " << GetGenFilename() << RESTendl;
        RESTMetadata << "Stored templates: " << fParticlesTemplate.size() << RESTendl;
        RESTMetadata << "Particles: ";
        for (const auto& particle : fParticles) RESTMetadata << particle.GetParticleName() << ", ";
        RESTMetadata << RESTendl;
    } else {
        if (GetParticleCharge() != 0) {
            RESTMetadata << "Particle charge: " << GetParticleCharge() << RESTendl;
        }
        RESTMetadata << "Angular distribution type: " << GetAngularDistributionType() << RESTendl;
        if ((StringToAngularDistributionTypes(GetAngularDistributionType().Data()) ==
            AngularDistributionTypes::TH1D) || (StringToAngularDistributionTypes(GetAngularDistributionType().Data()) ==
             AngularDistributionTypes::TH2D)) {
            RESTMetadata << "Angular distribution filename: "
                         << TRestTools::GetPureFileName((string)GetAngularDistributionFilename()) << RESTendl;
            RESTMetadata << "Angular distribution histogram name: " << GetAngularDistributionNameInFile()
                         << RESTendl;
        }
        RESTMetadata << "Angular distribution direction: (" << GetDirection().X() << "," << GetDirection().Y()
                     << "," << GetDirection().Z() << ")" << RESTendl;
        if ((StringToAngularDistributionTypes(GetAngularDistributionType().Data()) ==
             AngularDistributionTypes::ISOTROPIC)) {
            if (GetAngularDistributionIsotropicConeHalfAngle() != 0) {
                const double solidAngle =
                    2 * TMath::Pi() * (1 - TMath::Cos(GetAngularDistributionIsotropicConeHalfAngle()));
                RESTMetadata << "Angular distribution isotropic cone half angle (deg): "
                             << GetAngularDistributionIsotropicConeHalfAngle() * TMath::RadToDeg()
                             << " - solid angle: " << solidAngle << " ("
                             << solidAngle / (4 * TMath::Pi()) * 100 << "%)" << RESTendl;
            }
        }
        if ((StringToAngularDistributionTypes(GetAngularDistributionType().Data()) ==
             AngularDistributionTypes::TH1D) ||
            StringToAngularDistributionTypes(GetAngularDistributionType().Data()) ==
                AngularDistributionTypes::TH2D) {
            RESTMetadata << "Angular distribution range (deg): ("
                         << GetAngularDistributionRangeMin() * TMath::RadToDeg() << ", "
                         << GetAngularDistributionRangeMax() * TMath::RadToDeg() << ")" << RESTendl;
            RESTMetadata << "Angular distribution random sampling grid size: "
                         << GetAngularDistributionFormulaNPoints() << RESTendl;
        }
        RESTMetadata << "Energy distribution type: " << GetEnergyDistributionType() << RESTendl;
        if ((StringToEnergyDistributionTypes(GetEnergyDistributionType().Data()) ==
             EnergyDistributionTypes::TH1D) ||
            StringToEnergyDistributionTypes(GetEnergyDistributionType().Data()) ==
                EnergyDistributionTypes::TH2D) {
            RESTMetadata << "Energy distribution filename: "
                         << TRestTools::GetPureFileName((string)GetEnergyDistributionFilename()) << RESTendl;
            RESTMetadata << "Energy distribution histogram name: " << GetEnergyDistributionNameInFile()
                         << RESTendl;
        }
        if (GetEnergyDistributionRangeMin() == GetEnergyDistributionRangeMax()) {
            RESTMetadata << "Energy distribution energy: " << GetEnergy() << " keV" << RESTendl;
        } else {
            RESTMetadata << "Energy distribution range (keV): (" << GetEnergyDistributionRangeMin() << ", "
                         << GetEnergyDistributionRangeMax() << ")" << RESTendl;
        }
        if ((StringToEnergyDistributionTypes(GetEnergyDistributionType().Data()) ==
             EnergyDistributionTypes::FORMULA) ||
            StringToEnergyDistributionTypes(GetEnergyDistributionType().Data()) ==
                EnergyDistributionTypes::FORMULA2) {
            RESTMetadata << "Energy distribution random sampling grid size: "
                         << GetEnergyDistributionFormulaNPoints() << RESTendl;
        }
    }
}

TRestGeant4ParticleSource* TRestGeant4ParticleSource::instantiate(std::string model) {
    if (model.empty() || model == "geant4" || model.find(".dat") != string::npos) {
        // use default generator
        return new TRestGeant4ParticleSource();
    } else {
        // use specific generator
        // naming convention: TRestGeant4ParticleSourceXXX
        // currently supported generators: decay0, source
        // in future we may add wrapper of generators: pythia(for HEP), etc.
        model[0] = *REST_StringHelper::ToUpper(std::string(&model[0], 1)).c_str();
        TClass* c = TClass::GetClass(("TRestGeant4ParticleSource" + model).c_str());
        if (c)  // this means we have the package installed
        {
            return (TRestGeant4ParticleSource*)c->New();
        } else {
            std::cout << "REST ERROR! generator wrapper \"" << ("TRestGeant4ParticleSource" + model)
                      << "\" not found!" << std::endl;
        }
    }
    return nullptr;
}

void TRestGeant4ParticleSource::InitFromConfigFile() {
    TString modelUse = GetParameter("use");
    if (((string)modelUse).find(".dat") != string::npos) {
        string fullPathName = SearchFile((string)modelUse);
        if (fullPathName == "") {
            RESTError << "File not found: " << modelUse << RESTendl;
            RESTError << "Decay0 generator file could not be found!!" << RESTendl;
            exit(1);
        }
        modelUse = fullPathName;
    }
    SetGenFilename(modelUse);

    if (((string)fGenFilename).find(".dat") != std::string::npos) {
        if (TRestTools::fileExists((string)fGenFilename)) {
            ReadEventDataFile(fGenFilename);
        }
    }
}

// base class's generator action: randomize the particle's energy/direction with distribution file
void TRestGeant4ParticleSource::Update() {
    if (!fParticlesTemplate.empty()) {
        // we use particle template to generate particles
        Int_t rndCollection = (Int_t)(fRandomMethod() * fParticlesTemplate.size());
        Int_t pCollectionID = rndCollection % fParticlesTemplate.size();
        fParticles = fParticlesTemplate[pCollectionID];
    } else {
        TRestGeant4Particle p(*this);
        // TODO: implement particle generation for toy simulation
        fParticles = {p};
    }
}

TVector3 TRestGeant4ParticleSource::GetDirection() const {
    // direction should be unit (normalized) vector with a tolerance of 0.001

    const double magnitude = fDirection.Mag();
    constexpr double tolerance = 0.001;
    if (TMath::Abs(magnitude - 1) > tolerance) {
        cerr << "TRestGeant4ParticleSource::GetDirection: Direction must be unit vector" << endl;
        exit(1);
    }

    return fDirection;
}
///////////////////////////////////////////////
/// \brief Reads an input file produced by Decay0.
///
/// The input file should contain the description of several
/// pre-generated events, providing the names (or ids) of
/// particles to be produced, their energy, and momentum.
/// The particles and their properties are stored in a
/// TRestG4ParticleCollection which will be randomly accessed
/// by the restG4 package.
///
/// \param fName The Decay0 filename located at
/// REST_PATH/data/generator/
///
void TRestGeant4ParticleSource::ReadEventDataFile(TString fName) {
    if (!ReadOldDecay0File(fName)) {
        ReadNewDecay0File(fName);
    }
}

///////////////////////////////////////////////
/// \brief Reads particle information using the file format from newer Decay0 versions.
///
/// This is an auxiliar method used in TRestG4Metadata::ReadEventDataFile that will read the Decay0 files
/// produced with the newer Decay0 versions.
///
bool TRestGeant4ParticleSource::ReadNewDecay0File(TString fileName) {
    ifstream infile;
    infile.open(fileName);
    if (!infile.is_open()) {
        printf("Error when opening file %s\n", fileName.Data());
        return false;
    }

    int generatorEvents = 0;
    string s;
    // First lines are discarded.
    for (int i = 0; i < 24; i++) {
        getline(infile, s);
        int pos = s.find("@nevents=");
        if (pos != -1) generatorEvents = StringToInteger(s.substr(10, s.length() - 10));
    }

    if (generatorEvents == 0) {
        RESTError << "TRestG4Metadata::ReadNewDecay0File. Problem reading generator file" << RESTendl;
        exit(1);
    }

    TRestGeant4Particle particle;

    RESTDebug << "Reading generator file: " << fileName << RESTendl;
    RESTDebug << "Total number of events: " << generatorEvents << RESTendl;
    for (int n = 0; n < generatorEvents && !infile.eof(); n++) {
        int pos = -1;
        while (!infile.eof() && pos == -1) {
            getline(infile, s);
            pos = s.find("@event_start");
        }

        // Time - nuclide is skipped
        getline(infile, s);

        Int_t nParticles;
        infile >> nParticles;
        RESTDebug << "Number of particles: " << nParticles << RESTendl;

        // cout << evID <<" "<< time <<" "<< nParticles <<" "<< std::endl;
        for (int i = 0; i < nParticles && !infile.eof(); i++) {
            Int_t pID;
            Double_t momx, momy, momz, mass;
            Double_t energy = -1, momentum2;
            TString pName;

            Double_t time;
            infile >> pID >> time >> momx >> momy >> momz >> pName;

            RESTDebug << " ---- New particle found --- " << RESTendl;
            RESTDebug << " Particle name: " << pName << RESTendl;
            RESTDebug << " - pId: " << pID << RESTendl;
            RESTDebug << " - Relative time: " << time << RESTendl;
            RESTDebug << " - px: " << momx << " py: " << momy << " pz: " << momz << " " << RESTendl;

            if (pID == 3) {
                momentum2 = (momx * momx) + (momy * momy) + (momz * momz);
                mass = 0.511;

                energy = TMath::Sqrt(momentum2 + mass * mass) - mass;
                particle.SetParticleName("e-");
                particle.SetParticleCharge(-1);
                particle.SetExcitationLevel(0);

            } else if (pID == 1) {
                momentum2 = (momx * momx) + (momy * momy) + (momz * momz);

                energy = TMath::Sqrt(momentum2);
                particle.SetParticleName("gamma");
                particle.SetParticleCharge(0);
                particle.SetExcitationLevel(0);
            } else {
                cout << "Particle id " << pID << " not recognized" << std::endl;
            }

            TVector3 momDirection(momx, momy, momz);
            momDirection = momDirection.Unit();

            particle.SetEnergy(1000. * energy);
            particle.SetDirection(momDirection);

            this->AddParticle(particle);
        }
        this->FlushParticlesTemplate();
    }

    return true;
}

///////////////////////////////////////////////
/// \brief Reads particle information using the file format from older Decay0 versions.
///
/// This is an auxiliar method used in TRestG4Metadata::ReadEventDataFile that will read the Decay0 files
/// produced with the newer Decay0 versions.
///
bool TRestGeant4ParticleSource::ReadOldDecay0File(TString fileName) {
    ifstream infile;
    infile.open(fileName);
    if (!infile.is_open()) {
        printf("Error when opening file %s\n", fileName.Data());
        return false;
    }

    string s;
    // First lines are discarded.
    int headerFound = 0;
    for (int i = 0; i < 30; i++) {
        getline(infile, s);
        if (s.find("#!bxdecay0 1.0.0") != string::npos) return false;
        if (s.find("First event and full number of events:") != string::npos) {
            headerFound = 1;
            break;
        }
    }
    if (!headerFound) {
        RESTError
            << "TRestG4Metadata::ReadOldDecay0File. Problem reading generator file: no \"First event and "
               "full number of events:\" header.\n";
        abort();
    }
    int tmpInt;
    int fGeneratorEvents;
    infile >> tmpInt >> fGeneratorEvents;

    // cout << "i: " << tmpInt << " fN: " << fGeneratorEvents << std::endl;
    // TRestGeant4ParticleSource* src = TRestGeant4ParticleSource::instantiate();
    // this->SetGenFilename(fileName);
    // this->SetAngularDistributionType("flux");
    // this->SetEnergyDistributionType("mono");

    TRestGeant4Particle particle;

    cout << "Reading generator file: " << fileName << std::endl;
    cout << "Total number of events: " << fGeneratorEvents << std::endl;
    for (int n = 0; n < fGeneratorEvents && !infile.eof(); n++) {
        Int_t nParticles;
        Int_t evID;
        Double_t time;

        infile >> evID >> time >> nParticles;

        // cout << evID <<" "<< time <<" "<< nParticles <<" "<< std::endl;
        for (int i = 0; i < nParticles && !infile.eof(); i++) {
            Int_t pID;
            Double_t momx, momy, momz, mass;
            Double_t energy = -1, momentum2;
            Double_t x = 0, y = 0, z = 0;

            infile >> pID >> momx >> momy >> momz >> time;
            // infile >> x >> y >> z;

            // cout << momx << " " << momy << " " << momz << " " << std::endl;

            bool ise = 2 <= pID && pID <= 3, ismu = 5 <= pID && pID <= 6, isp = pID == 14, isg = pID == 1;
            if (ise || ismu || isp || isg) {
                momentum2 = (momx * momx) + (momy * momy) + (momz * momz);
                if (ise) {
                    mass = 0.511;
                    particle.SetParticleName(pID == 2 ? "e+" : "e-");
                    particle.SetParticleCharge(pID == 2 ? 1 : -1);
                } else if (ismu) {
                    mass = 105.7;
                    particle.SetParticleName(pID == 5 ? "mu+" : "mu-");
                    particle.SetParticleCharge(pID == 5 ? 1 : -1);
                } else if (isp) {
                    mass = 938.3;
                    particle.SetParticleName("proton");
                    particle.SetParticleCharge(1);
                } else {
                    mass = 0;
                    particle.SetParticleName("gamma");
                    particle.SetParticleCharge(0);
                }

                energy = TMath::Sqrt(momentum2 + mass * mass) - mass;
                particle.SetExcitationLevel(0);
            } else {
                cout << "Particle id " << pID << " not recognized" << std::endl;
            }

            TVector3 momDirection(momx, momy, momz);
            momDirection = momDirection.Unit();

            particle.SetEnergy(1000. * energy);
            particle.SetDirection(momDirection);
            particle.SetOrigin(TVector3(x, y, z));

            this->AddParticle(particle);
        }

        this->FlushParticlesTemplate();
    }

    return true;
}
