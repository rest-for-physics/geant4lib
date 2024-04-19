
#include "TRestGeant4ParticleSourceCosmics.h"

#include <TFile.h>
#include <TH2D.h>

using namespace std;

mutex TRestGeant4ParticleSourceCosmics::fMutex;
unique_ptr<TRandom3> TRestGeant4ParticleSourceCosmics::fRandom = nullptr;

const map<string, string> geant4ParticleNames = {
    {"neutron", "neutron"}, {"proton", "proton"}, {"gamma", "gamma"}, {"electron", "e-"}, {"muon", "mu-"}};

TRestGeant4ParticleSourceCosmics::TRestGeant4ParticleSourceCosmics() = default;

void TRestGeant4ParticleSourceCosmics::InitFromConfigFile() {
    lock_guard<mutex> lock(fMutex);

    cout << "TRestGeant4ParticleSourceCosmics::InitFromConfigFile" << endl;
    fFilename = GetParameter("filename");
    const auto particles = GetParameter("particles", "neutron,proton,gamma,electron,muon");

    fDirection = Get3DVectorParameterWithUnits("direction", {0, -1, 0});

    std::istringstream iss(particles);
    std::string name;
    while (std::getline(iss, name, ',')) {
        fParticleNames.insert(name);
    }

    for (const auto& particle : fParticleNames) {
        if (geant4ParticleNames.find(particle) == geant4ParticleNames.end()) {
            cerr << "Particle name '" << particle << "' is not allowed." << endl;
            exit(1);
        }
    }

    auto file = TFile::Open(fFilename.c_str(), "READ");
    if (file == nullptr) {
        cerr << "File '" << fFilename << "' not found." << endl;
        exit(1);
    }
    for (const auto& particle : fParticleNames) {
        auto histogram = file->Get<TH2D>(string(particle + "_energy_zenith").c_str());
        if (histogram == nullptr) {
            cerr << "Histogram '" << particle << "' not found in file '" << fFilename << "'." << endl;
            exit(1);
        }
        fHistograms[particle] = histogram;
    }

    double sum = 0;
    for (const auto& particle : fParticleNames) {
        auto hist = fHistograms.at(particle);
        fParticleWeights[particle] += hist->GetEntries();
        sum += fParticleWeights.at(particle);
    }
    for (auto& entry : fParticleWeights) {
        entry.second /= sum;
        cout << "TRestGeant4ParticleSourceCosmics::InitFromConfigFile: particle: " << entry.first
             << " sampling weight: " << entry.second << endl;
    }

    // TODO: how to safely close the file?
}

void TRestGeant4ParticleSourceCosmics::Update() {
    lock_guard<mutex> lock(fMutex);

    RemoveParticles();

    string particleName;
    if (fParticleNames.size() == 1) {
        particleName = *fParticleNames.begin();
    } else {
        const auto random = fRandom->Uniform(0, 1);
        // choose a random particle based on the weights
        double sum = 0;
        for (const auto& entry : fParticleWeights) {
            sum += entry.second;
            if (random <= sum) {
                particleName = entry.first;
                break;
            }
        }
    }

    if (fHistogramsTransformed.find(particleName) == fHistogramsTransformed.end()) {
        auto histOriginal = fHistograms.at(particleName);
        TH2D* hist = new TH2D(*histOriginal);
        // same as original but Y axis is multiplied by 1/cos(zenith). Integral should be the same.
        for (int i = 1; i <= hist->GetNbinsX(); i++) {
            for (int j = 1; j <= hist->GetNbinsY(); j++) {
                const double zenith = hist->GetYaxis()->GetBinCenter(j);
                const double value = hist->GetBinContent(i, j) / TMath::Cos(zenith * TMath::DegToRad());
                hist->SetBinContent(i, j, value);
            }
        }
        fHistogramsTransformed[particleName] = hist;
    }
    auto hist = fHistogramsTransformed.at(particleName);
    double energy, zenith;
    hist->GetRandom2(energy, zenith, fRandom.get());

    particleName = geant4ParticleNames.at(particleName);

    TRestGeant4Particle particle;
    particle.SetParticleName(particleName);

    particle.SetEnergy(energy * 1000);  // Convert from MeV to keV

    double phi = fRandom->Uniform(0, 1) * TMath::TwoPi();
    double zenithRad = zenith * TMath::DegToRad();

    // direction towards -y (can be rotated later)
    const TVector3 direction = {TMath::Sin(zenithRad) * TMath::Cos(phi), -TMath::Cos(zenithRad),
                                TMath::Sin(zenithRad) * TMath::Sin(phi)};

    particle.SetDirection(direction);

    AddParticle(particle);
}

void TRestGeant4ParticleSourceCosmics::SetSeed(unsigned int seed) {
    cout << "TRestGeant4ParticleSourceCosmics::SetSeed: " << seed << endl;
    fRandom = std::make_unique<TRandom3>(seed);
}
