#include "TRestGeant4ParticleSourceDecay0.h"

using namespace std;

ClassImp(TRestGeant4ParticleSourceDecay0);

TRestGeant4ParticleSourceDecay0::TRestGeant4ParticleSourceDecay0()
/* : generator((uintptr_t)this), prng(generator)*/ {
    fDecay0Model = new bxdecay0::decay0_generator();
}

void TRestGeant4ParticleSourceDecay0::PrintMetadata() {
    RESTMetadata << "---------------------------------------" << RESTendl;
    if (!fParticleName.empty() && fParticleName != "NO_SUCH_PARA")
        RESTMetadata << "Particle Source Name: " << fParticleName << RESTendl;
    RESTMetadata << "Parent Nuclide: " << fParentName << RESTendl;
    RESTMetadata << "Decay Mode: " << fDecayType << RESTendl;
    RESTMetadata << "Daughter Level: " << fDaughterLevel << RESTendl;
    RESTMetadata << "Seed: " << fSeed << RESTendl;
}

void TRestGeant4ParticleSourceDecay0::InitFromConfigFile() {
    // unsigned int seed = (uintptr_t)this;
    // std::default_random_engine generator(seed);
    // prng = bxdecay0::std_random(generator);
    fParticleName = ((TRestMetadata*)this)->ClassName();
    fParentName = GetParameter("particle");
    fDecayType = GetParameter("decayMode");
    fDaughterLevel = StringToInteger(GetParameter("daughterLevel"));
    fSeed = StringToInteger(GetParameter("seed", "0"));
    if (fSeed != 0) {
    } else {
        fSeed = (uintptr_t)this;
    }
    generator = new std::default_random_engine(fSeed);
    prng = new bxdecay0::std_random(*generator);

    fDecay0Model->set_decay_category(bxdecay0::decay0_generator::DECAY_CATEGORY_DBD);

    if (fParentName != "Xe136") {
        ferr << "Only Xe136 double beta decay is supported by restDecay0" << endl;
        exit(1);
    }
    if (fDaughterLevel < 0 || fDaughterLevel > 3) {
        ferr << "Supported Ba136 excitation level: 0, 1, 2, 3" << endl;
        exit(1);
    }

    fDecay0Model->set_decay_isotope(fParentName);

    fDecay0Model->set_decay_dbd_level(fDaughterLevel);

    if (fDecayType == "2vbb") {
        if (fDaughterLevel == 0 || fDaughterLevel == 3) {
            fDecay0Model->set_decay_dbd_mode(bxdecay0::DBDMODE_2NUBB_0_2N);
        } else if (fDaughterLevel == 1 || fDaughterLevel == 2) {
            fDecay0Model->set_decay_dbd_mode(bxdecay0::DBDMODE_2NUBB_2_2N);
        }
    } else if (fDecayType == "0vbb") {
        if (fDaughterLevel == 0 || fDaughterLevel == 3) {
            fDecay0Model->set_decay_dbd_mode(bxdecay0::DBDMODE_0NUBB_MN_0_2N);
        } else if (fDaughterLevel == 1 || fDaughterLevel == 2) {
            fDecay0Model->set_decay_dbd_mode(bxdecay0::DBDMODE_0NUBB_RHC_LAMBDA_2_2N);
        }
    }

    fDecay0Model->initialize(*prng);

    Update();
}

void TRestGeant4ParticleSourceDecay0::Update() {
    RemoveParticles();

    bxdecay0::event gendecay;
    fDecay0Model->shoot(*prng, gendecay);

    auto ps = gendecay.get_particles();
    for (auto p : ps) {
        TRestGeant4Particle particle;
        double mass;
        double energy;

        double momx = p.get_px();
        double momy = p.get_py();
        double momz = p.get_pz();
        double momentum2 = p.get_p() * p.get_p();
        int pID = p.get_code();

        /*
                0, ///< Invalid particle code
                1,  ///< Gamma
                2,  ///< Positron
                3,  ///< Electron
                13, ///< Neutron
                14, ///< Proton
                47  ///< Alpha
        */

        if (pID == 1) {
            energy = TMath::Sqrt(momentum2);
            particle.SetParticleName("gamma");
            particle.SetParticleCharge(0);
            particle.SetExcitationLevel(0);
        } else if (pID == 2) {
            mass = 0.511;
            energy = TMath::Sqrt(momentum2);
            particle.SetParticleName("positron");
            particle.SetParticleCharge(1);
            particle.SetExcitationLevel(0);
        } else if (pID == 3) {
            mass = 0.511;
            energy = TMath::Sqrt(momentum2 + mass * mass) - mass;
            particle.SetParticleName("e-");
            particle.SetParticleCharge(-1);
            particle.SetExcitationLevel(0);
        } else if (pID == 13) {
            mass = 939.6;
            energy = TMath::Sqrt(momentum2);
            particle.SetParticleName("neutron");
            particle.SetParticleCharge(0);
            particle.SetExcitationLevel(0);
        } else if (pID == 14) {
            mass = 938.3;
            energy = TMath::Sqrt(momentum2);
            particle.SetParticleName("proton");
            particle.SetParticleCharge(1);
            particle.SetExcitationLevel(0);
        } else if (pID == 47) {
            mass = 3727;
            energy = TMath::Sqrt(momentum2);
            particle.SetParticleName("alpha");
            particle.SetParticleCharge(2);
            particle.SetExcitationLevel(0);
        }

        TVector3 momDirection(momx, momy, momz);
        momDirection = momDirection.Unit();

        particle.SetEnergy(1000. * energy);
        particle.SetDirection(momDirection);

        // cout << energy * 1000 << " ";

        AddParticle(particle);
    }

    // cout << endl;
}
