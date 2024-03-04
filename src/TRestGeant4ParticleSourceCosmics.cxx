
#include "TRestGeant4ParticleSourceCosmics.h"

using namespace std;

const set<string> allowedParticleNames = {"neutron", "proton", "gamma", "electron", "muon"};

TRestGeant4ParticleSourceCosmics::TRestGeant4ParticleSourceCosmics() {}

void TRestGeant4ParticleSourceCosmics::InitFromConfigFile() {
    fFilename = GetParameter("filename");
    const auto particles = GetParameter("particleNames", "neutron,proton,gamma,electron,muon");

    std::istringstream iss(particles);
    std::string name;
    while (std::getline(iss, name, ',')) {
        fParticleNames.insert(name);
    }

    for (const auto& particle : fParticleNames) {
        if (allowedParticleNames.find(particle) == allowedParticleNames.end()) {
            cerr << "Particle name '" << particle << "' is not allowed." << endl;
            exit(1);
        }
    }
}

void TRestGeant4ParticleSourceCosmics::Update() {
    RemoveParticles();
    const auto randomNumber = fRandomGenerator();

    string particleName;
    if (fParticleNames.size() == 1) {
        particleName = *fParticleNames.begin();
    } else {
        const auto random = fRandomGenerator();

        // choose a random particle based on the weights
        double sum = 0;
        for (const auto& entry : fParticleWeights) {
            sum += entry.second;
            if (random < sum) {
                particleName = entry.first;
            }
        }
    }
}
