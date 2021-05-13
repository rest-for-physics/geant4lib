//////////////////////////////////////////////////////////////////////////
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4ParticleCollection.h
///
///             Class to store a particle definition
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestGeant4ParticleCollection
#define RestCore_TRestGeant4ParticleCollection

#include <TObject.h>
#include <TRestGeant4Particle.h>

#include <iostream>

class TRestGeant4ParticleCollection : public TObject {
   protected:
    std::vector<TRestGeant4Particle> fParticles;

   public:
    static TRestGeant4ParticleCollection* instantiate(std::string model = "");

    virtual void SetParticleModel(std::string modelString) {
        std::cout << "REST ERROR: SetParticleModel() called in base class "
                     "TRestGeant4ParticleCollection"
                  << std::endl;
        std::cout << "Package not properly installed? LD_LIBRARY_PATH not properly set?" << std::endl;
        exit(1);
    }
    virtual void VirtualUpdate() {}
    virtual Int_t GetNumberOfParticles() { return fParticles.size(); }
    virtual TRestGeant4Particle GetParticle(int i) { return fParticles[i]; }

    virtual void RemoveParticles() { fParticles.clear(); }
    virtual void AddParticle(TRestGeant4Particle particle) { fParticles.push_back(particle); }

    // Constructor
    TRestGeant4ParticleCollection();
    // Destructor
    virtual ~TRestGeant4ParticleCollection();

    ClassDef(TRestGeant4ParticleCollection, 1);
};
#endif
