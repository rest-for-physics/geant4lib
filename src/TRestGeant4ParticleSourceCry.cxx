#include "TRestGeant4ParticleSourceCry.h"

using namespace std;

ClassImp(TRestGeant4ParticleSourceCry);

TRestGeant4ParticleSourceCry::TRestGeant4ParticleSourceCry() {

}

void TRestGeant4ParticleSourceCry::PrintParticleSource() {
	/*
    metadata << "---------------------------------------" << endl;
    if (!fParticleName.empty() && fParticleName != "NO_SUCH_PARA")
        metadata << "Particle Source Name: " << fParticleName << endl;
    metadata << "Parent Nuclide: " << fParentName << endl;
    metadata << "Decay Mode: " << fDecayType << endl;
    metadata << "Daughter Level: " << fDaughterLevel << endl;
    metadata << "Seed: " << fSeed << endl;
	*/
}

void TRestGeant4ParticleSourceCry::InitFromConfigFile() {
	/*
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
	*/

#ifdef USE_CRY
	/// Of course, we should be able to configure this inside InitFromConfigFile
	std::string setupString = "returnNeutrons 1";
	//std::string setupString = "returnNeutrons 1\nreturnProtons 1\nreturnGammas 1\ndate 7-1-2012\nlatitude 90.0\naltitude 0\nsubboxLength 100\n";
	CRYSetup* setup = new CRYSetup(setupString, CRY_DATA_PATH);
	fCRYGenerator = new CRYGenerator(setup);
#endif

    Update();
}

void TRestGeant4ParticleSourceCry::Update() {
	RemoveParticles();

#ifdef USE_CRY
	std::vector<CRYParticle*>* ev = new std::vector<CRYParticle*>;
	ev->clear();
	fCRYGenerator->genEvent(ev);

	std::cout << "CRY particles : " << ev->size() << std::endl;
	std::cout << "-----" << std::endl;

	for(const auto &cryParticle : *ev )
	{
		std::cout << "id: " << cryParticle->id() << std::endl;

		std::cout << "x: " << cryParticle->x() << " y: " << cryParticle->y() << " z: " << cryParticle->z() << std::endl;
		std::cout << "u: " << cryParticle->u() << " v: " << cryParticle->v() << " w: " << cryParticle->w() << std::endl;
		std::cout << "charge: " << cryParticle->charge() << " energy: " << cryParticle->ke() << std::endl;

		TRestGeant4Particle particle;

		/// Particle charge
		particle.SetParticleCharge(cryParticle->charge());
		particle.SetExcitationLevel(0);

		/// Particle position
		TVector3 position( cryParticle->x(), cryParticle->y(), cryParticle->z() );
		particle.SetOrigin( position );

		/// Momentum direction
		TVector3 momDirection(cryParticle->u(), cryParticle->v(), cryParticle->w());
		momDirection = momDirection.Unit();
		particle.SetDirection(momDirection);

		/// Kinetic energy
		particle.SetEnergy(1000. * cryParticle->ke()); // In keV (default REST units)

		/*
		 * 0 : Neutron
		 * 1 : Proton
		 * 2 : Pion
		 * 3 : Kaon
		 * 4 : Muon
		 * 5 : Electron
		 * 6 : Gamma
		 */

		int id = cryParticle->id();
		if (id == 0) particle.SetParticleName("neutron");
		if (id == 1) particle.SetParticleName("proton");
		if (id == 2) {
			if( cryParticle->charge() > 0 )
				particle.SetParticleName("pi+");
			else if( cryParticle->charge() == 0 )
				particle.SetParticleName("pi0");
			else if( cryParticle->charge() < 0 )
				particle.SetParticleName("pi-");
		}
		if (id == 3) {
			if( cryParticle->charge() > 0 )
				particle.SetParticleName("kaon+");
			else if( cryParticle->charge() == 0 )
				particle.SetParticleName("kaon0");
			else if( cryParticle->charge() < 0 )
				particle.SetParticleName("kaon-");
		}
		if (id == 4) {
			if( cryParticle->charge() > 0 )
				particle.SetParticleName("mu+");
			else if( cryParticle->charge() < 0 )
				particle.SetParticleName("mu-");
		}
		if (id == 5) {
			if( cryParticle->charge() > 0 )
				particle.SetParticleName("e+");
			else if( cryParticle->charge() < 0 )
				particle.SetParticleName("e-");
		}
		if (id == 6) particle.SetParticleName("gamma");

		AddParticle(particle);
	}
	std::cout << "-----" << std::endl;
#else
	cout << "TRestGeant4ParticleSourceCry - ERROR: Geant4lib was not linked to CRY libraries" << endl;
	cout << "  " << endl;
	cout << "Please, compile REST using `cmake -DREST_CRY_PATH=/path/to/cry/installation/directory" << endl;
	cout << "  " << endl;
	cout << "By default CRY libraries will generate just the static library, but REST needs the shared library" << endl;
	cout << "In order to generate the SHARED object from the STATIC libCRY.a object, execute the following command" << endl;
	cout << "inside the CRY lib directory" << endl;
	cout << "  " << endl;
	cout << "```" << endl;
	cout << "gcc -shared -o libCRY.so -Wl,--whole-archive libCRY.a -Wl,--no-whole-archive" << endl;
	cout << "```" << endl;

	exit(1);
#endif

}
