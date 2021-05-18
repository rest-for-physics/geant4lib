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

#include "TRestGeant4Metadata.h"
#include "TRestGeant4ParticleSource.h"
#include "TRestReflector.h"
#include "TRestStringHelper.h"
#include "TRestStringOutput.h"
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
    if (fParticlesTemplate.size() > 0 || fGenFilename != "") {
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
    if (model == "" || model == "geant4" || model.find(".dat") != -1) {
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

void TRestGeant4ParticleSource::InitFromConfigFile() {
    TString modelUse = GetParameter("use");
    if (((string)modelUse).find(".dat") != -1) {
        string fullPathName = SearchFile((string)modelUse);
        if (fullPathName == "") {
            ferr << "File not found : " << modelUse << endl;
            ferr << "Decay0 generator file could not be found!!" << endl;
            exit(1);
        }
        modelUse = fullPathName;
    }
    SetGenFilename(modelUse);

    if (((string)fGenFilename).find(".dat") != -1) {
        if (TRestTools::fileExists((string)fGenFilename)) {
            ReadEventDataFile(fGenFilename);
        }
    }
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
        ferr << "TRestG4Metadata::ReadNewDecay0File. Problem reading generator file" << endl;
        exit(1);
    }

    // TRestGeant4ParticleSource* src = TRestGeant4ParticleSource::instantiate();
    // this->SetGenFilename(fileName);
    // this->SetAngularDistType("flux");
    // this->SetEnergyDistType("mono");

    TRestGeant4Particle particle;

    debug << "Reading generator file : " << fileName << endl;
    debug << "Total number of events : " << generatorEvents << endl;
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
        debug << "Number of particles : " << nParticles << endl;

        // cout << evID <<" "<< time <<" "<< nParticles <<" "<< endl;
        for (int i = 0; i < nParticles && !infile.eof(); i++) {
            Int_t pID;
            Double_t momx, momy, momz, mass;
            Double_t energy = -1, momentum2;
            TString pName;

            Double_t time;
            infile >> pID >> time >> momx >> momy >> momz >> pName;

            debug << " ---- New particle found --- " << endl;
            debug << " Particle name : " << pName << endl;
            debug << " - pId : " << pID << endl;
            debug << " - Relative time : " << time << endl;
            debug << " - px: " << momx << " py: " << momy << " pz: " << momz << " " << endl;

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
                cout << "Particle id " << pID << " not recognized" << endl;
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
        if (s.find("#!bxdecay0 1.0.0") != -1) return false;
        if (s.find("First event and full number of events:") != -1) {
            headerFound = 1;
            break;
        }
    }
    if (!headerFound) {
        ferr << "TRestG4Metadata::ReadOldDecay0File. Problem reading generator file: no \"First event and "
                "full number of events:\" header.\n";
        abort();
    }
    int tmpInt;
    int fGeneratorEvents;
    infile >> tmpInt >> fGeneratorEvents;

    // cout << "i : " << tmpInt << " fN : " << fGeneratorEvents << endl;
    // TRestGeant4ParticleSource* src = TRestGeant4ParticleSource::instantiate();
    // this->SetGenFilename(fileName);
    // this->SetAngularDistType("flux");
    // this->SetEnergyDistType("mono");

    TRestGeant4Particle particle;

    cout << "Reading generator file : " << fileName << endl;
    cout << "Total number of events : " << fGeneratorEvents << endl;
    for (int n = 0; n < fGeneratorEvents && !infile.eof(); n++) {
        Int_t nParticles;
        Int_t evID;
        Double_t time;

        infile >> evID >> time >> nParticles;

        // cout << evID <<" "<< time <<" "<< nParticles <<" "<< endl;
        for (int i = 0; i < nParticles && !infile.eof(); i++) {
            Int_t pID;
            Double_t momx, momy, momz, mass;
            Double_t energy = -1, momentum2;
            Double_t x, y, z;

            infile >> pID >> momx >> momy >> momz >> time;
            // infile >> x >> y >> z;

            // cout << momx << " " << momy << " " << momz << " " << endl;

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
                cout << "Particle id " << pID << " not recognized" << endl;
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