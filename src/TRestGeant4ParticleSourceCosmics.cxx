
#include "TRestGeant4ParticleSourceCosmics.h"

#include <TFile.h>
#include <TH2D.h>

using namespace std;

mutex TRestGeant4ParticleSourceCosmics::fMutex;
unique_ptr<TRandom3> TRestGeant4ParticleSourceCosmics::fRandom = nullptr;

const map<string, string> geant4ParticleNames = {
    {"neutron", "neutron"},
    {"proton", "proton"},
    {"gamma", "gamma"},
    {"electron_minus", "e-"},
    {"electron_plus", "e+"},
    {"muon_minus", "mu-"},
    {"muon_plus", "mu+"},
    //
    {"neutron_below_1MeV", "neutron"},
    {"neutron_above_10GeV", "neutron"},
    {"neutron_between_1MeV_and_10GeV", "neutron"},
};

TRestGeant4ParticleSourceCosmics::TRestGeant4ParticleSourceCosmics() = default;

void TRestGeant4ParticleSourceCosmics::InitFromConfigFile() {
    lock_guard<mutex> lock(fMutex);

    cout << "TRestGeant4ParticleSourceCosmics::InitFromConfigFile" << endl;
    fFilename = GetParameter("filename");
    const auto particles =
        GetParameter("particles", "neutron,proton,gamma,electron_minus,electron_plus,muon_minus,muon_plus");

    const TVector2 energy = Get2DVectorParameterWithUnits("energy", {0, 0});
    // sort it so that the lower energy is first
    if (energy.X() > energy.Y()) {
        fEnergyRange = {energy.Y(), energy.X()};
    } else {
        fEnergyRange = {energy.X(), energy.Y()};
    }
    // convert energy to MeV
    fEnergyRange = {fEnergyRange.first / 1000, fEnergyRange.second / 1000};

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
        auto histogramFromFile = file->Get<TH2D>(string(particle + "_energy_zenith").c_str());
        if (histogramFromFile == nullptr) {
            cerr << "Histogram '" << particle << "' not found in file '" << fFilename << "'." << endl;
            exit(1);
        }
        fHistograms[particle] = new TH2D(*histogramFromFile);
        const auto& histogram = fHistograms.at(particle);
        TH2D* hist = new TH2D(*histogram);
        // same as original but Y axis is multiplied by 1/cos(zenith). Integral is also scaled properly when
        // computing the time of the simulation.
        for (int i = 1; i <= hist->GetNbinsX(); i++) {
            for (int j = 1; j <= hist->GetNbinsY(); j++) {
                const double zenith = hist->GetYaxis()->GetBinCenter(j);
                const double value = hist->GetBinContent(i, j) / TMath::Cos(zenith * TMath::DegToRad());
                hist->SetBinContent(i, j, value);
            }
        }
        fHistogramsTransformed[particle] = hist;
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

    // file->Close();
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

    auto hist = fHistogramsTransformed.at(particleName);

    double energy, zenith;
    if (abs(fEnergyRange.first - fEnergyRange.second) <
        1e-12) {  // user has not defined a range TODO: improve how we check for this...
        hist->GetRandom2(energy, zenith, fRandom.get());
    } else {
        // attempt to get a value in range, then use the counters to update simulation time
        while (true) {
            hist->GetRandom2(energy, zenith, fRandom.get());
            // check counter does not overflow. If counter is at limit, stop updating it, we should have
            // enough stats by now
            if (fCounterEnergyTotal < std::numeric_limits<unsigned long long>::max()) {
                fCounterEnergyTotal++;
            }

            if (energy >= fEnergyRange.first && energy <= fEnergyRange.second) {
                if (fCounterEnergyTotal < std::numeric_limits<unsigned long long>::max()) {
                    fCounterEnergyAccepted++;
                }
                break;
            }
        }
    }

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

double TRestGeant4ParticleSourceCosmics::GetEnergyRangeScalingFactor() const {
    if (fCounterEnergyTotal == 0) {
        return 1;
    }

    if (fParticleNames.size() > 1) {
        throw std::runtime_error(
            "TRestGeant4ParticleSourceCosmics::GetEnergyRangeScalingFactor is not implemented for multiple "
            "particles.");
    }

    return double(fCounterEnergyAccepted) / double(fCounterEnergyTotal);
}
