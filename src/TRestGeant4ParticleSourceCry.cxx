#include "TRestGeant4ParticleSourceCry.h"

using namespace std;

ClassImp(TRestGeant4ParticleSourceCry);

TRestGeant4ParticleSourceCry::TRestGeant4ParticleSourceCry() {}

void TRestGeant4ParticleSourceCry::PrintMetadata() {
    TRestGeant4ParticleSource::PrintMetadata();

    RESTMetadata << "Return Neutrons : " << fReturnNeutrons << RESTendl;
    RESTMetadata << "Return Protons : " << fReturnProtons << RESTendl;
    RESTMetadata << "Return Gammas : " << fReturnGammas << RESTendl;
    RESTMetadata << "Return Electrons : " << fReturnElectrons << RESTendl;
    RESTMetadata << "Return Pions : " << fReturnPions << RESTendl;
    RESTMetadata << "Return Kaons : " << fReturnKaons << RESTendl;
    RESTMetadata << "Return Muons : " << fReturnMuons << RESTendl;
    RESTMetadata << " ======= " << RESTendl;

    RESTMetadata << "N particles min : " << fNParticlesMin << RESTendl;
    RESTMetadata << "N particles max : " << fNParticlesMax << RESTendl;
    RESTMetadata << " ======= " << RESTendl;

    RESTMetadata << "X-offset : " << fXOffset << "m" << RESTendl;
    RESTMetadata << "Y-offset : " << fYOffset << "m" << RESTendl;
    RESTMetadata << "Z-offset : " << fZOffset << "m" << RESTendl;
    RESTMetadata << "SubBoxLength : " << fSubBoxLength << "m" << RESTendl;
    RESTMetadata << " ======= " << RESTendl;

    RESTMetadata << "Date : " << fDate << RESTendl;
    RESTMetadata << "Latitude : " << fLatitude << RESTendl;
    RESTMetadata << "Altitude : " << fAltitude << RESTendl;
    RESTMetadata << "----------------------" << RESTendl;
}

void TRestGeant4ParticleSourceCry::InitFromConfigFile() {
    fReturnNeutrons = StringToInteger(GetParameter("returnNeutrons", "1"));
    fReturnProtons = StringToInteger(GetParameter("returnProtons", "1"));
    fReturnGammas = StringToInteger(GetParameter("returnGammas", "1"));
    fReturnElectrons = StringToInteger(GetParameter("returnElectrons", "1"));
    fReturnPions = StringToInteger(GetParameter("returnPions", "1"));
    fReturnKaons = StringToInteger(GetParameter("returnKaons", "1"));
    fReturnMuons = StringToInteger(GetParameter("returnMuons", "1"));

    fNParticlesMin = StringToInteger(GetParameter("nParticlesMin", "1"));
    fNParticlesMax = StringToInteger(GetParameter("nParticlesMax", "1000000"));

    fXOffset = StringToDouble(GetParameter("xoffset", "0.0"));
    fYOffset = StringToDouble(GetParameter("yoffset", "0.0"));
    fZOffset = StringToDouble(GetParameter("zoffset", "0.0"));
    fSubBoxLength = StringToDouble(GetParameter("subBoxLength", "100.0"));

    fDate = GetParameter("date", "7\\1\\2012");
    fDate = REST_StringHelper::Replace(fDate, "\\", "-");
    fLatitude = StringToDouble(GetParameter("latitude", "90.0"));
    fAltitude = StringToDouble(GetParameter("altitude", "0.0"));

    PrintMetadata();

    std::string setupString = "";
    setupString += "returnNeutrons " + IntegerToString(fReturnNeutrons);
    setupString += " returnProtons " + IntegerToString(fReturnProtons);
    setupString += " returnGammas " + IntegerToString(fReturnGammas);
    setupString += " returnElectrons " + IntegerToString(fReturnElectrons);
    setupString += " returnPions " + IntegerToString(fReturnPions);
    setupString += " returnKaons " + IntegerToString(fReturnKaons);
    setupString += " returnMuons " + IntegerToString(fReturnMuons);

    setupString += " xoffset " + DoubleToString(fXOffset);
    setupString += " yoffset " + DoubleToString(fYOffset);
    setupString += " zoffset " + DoubleToString(fZOffset);
    setupString += " subboxLength " + DoubleToString(fSubBoxLength);

    setupString += " date " + fDate;
    setupString += " latitude " + DoubleToString(fLatitude);
    setupString += " altitude " + DoubleToString(fAltitude);

    setupString += " nParticlesMin " + IntegerToString(fNParticlesMin);
    setupString += " nParticlesMax " + IntegerToString(fNParticlesMax);

#ifdef USE_CRY
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

    // std::cout << "CRY particles : " << ev->size() << std::endl;
    // std::cout << "-----" << std::endl;

    for (const auto& cryParticle : *ev) {
        // std::cout << "id: " << cryParticle->id() << std::endl;
        // std::cout << "x: " << cryParticle->x() << " y: " << cryParticle->y() << " z: " << cryParticle->z()
        // << std::endl; std::cout << "u: " << cryParticle->u() << " v: " << cryParticle->v() << " w: " <<
        // cryParticle->w() << std::endl; std::cout << "charge: " << cryParticle->charge() << " energy: " <<
        // cryParticle->ke() << std::endl;

        TRestGeant4Particle particle;

        /// Particle charge
        particle.SetParticleCharge(cryParticle->charge());
        particle.SetExcitationLevel(0);

        /// Particle position
        TVector3 position(cryParticle->x(), cryParticle->y(), cryParticle->z());
        particle.SetOrigin(1000. * position);  // In mm (default REST units)

        /// Momentum direction
        TVector3 momDirection(cryParticle->u(), cryParticle->v(), cryParticle->w());
        momDirection = momDirection.Unit();
        particle.SetDirection(momDirection);

        /// Kinetic energy
        particle.SetEnergy(1000. * cryParticle->ke());  // In keV (default REST units)

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
            if (cryParticle->charge() > 0)
                particle.SetParticleName("pi+");
            else if (cryParticle->charge() == 0)
                particle.SetParticleName("pi0");
            else if (cryParticle->charge() < 0)
                particle.SetParticleName("pi-");
        }
        if (id == 3) {
            if (cryParticle->charge() > 0)
                particle.SetParticleName("kaon+");
            else if (cryParticle->charge() == 0)
                particle.SetParticleName("kaon0");
            else if (cryParticle->charge() < 0)
                particle.SetParticleName("kaon-");
        }
        if (id == 4) {
            if (cryParticle->charge() > 0)
                particle.SetParticleName("mu+");
            else if (cryParticle->charge() < 0)
                particle.SetParticleName("mu-");
        }
        if (id == 5) {
            if (cryParticle->charge() > 0)
                particle.SetParticleName("e+");
            else if (cryParticle->charge() < 0)
                particle.SetParticleName("e-");
        }
        if (id == 6) particle.SetParticleName("gamma");

        AddParticle(particle);
    }
    // std::cout << "-----" << std::endl;
#else
    cout << "TRestGeant4ParticleSourceCry - ERROR: Geant4lib was not linked to CRY libraries" << endl;
    cout << "  " << endl;
    cout << "Please, compile REST using `cmake -DREST_CRY_PATH=/path/to/cry/installation/directory" << endl;
    cout << "  " << endl;
    cout
        << "By default CRY libraries will generate just the static library, but REST needs the shared library"
        << endl;
    cout << "In order to generate the SHARED object from the STATIC libCRY.a object, execute the following "
            "command"
         << endl;
    cout << "inside the CRY lib directory" << endl;
    cout << "  " << endl;
    cout << "```" << endl;
    cout << "gcc -shared -o libCRY.so -Wl,--whole-archive libCRY.a -Wl,--no-whole-archive" << endl;
    cout << "```" << endl;

    exit(1);
#endif
}
