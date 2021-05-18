#ifndef RestCore_TRestGeant4ParticleCollectionD0
#define RestCore_TRestGeant4ParticleCollectionD0

#include <TObject.h>
#include <bxdecay0/decay0_generator.h>
#include <bxdecay0/event.h>
#include <bxdecay0/std_random.h>

#include <iostream>

#include "TRestGeant4Particle.h"
#include "TRestGeant4ParticleCollection.h"
#include "TRestMetadata.h"

using namespace std;

class TRestGeant4ParticleCollectionDecay0 : public TRestGeant4ParticleCollection, public TRestMetadata {
   protected:
    bxdecay0::decay0_generator* fDecay0Model;  //!
    bxdecay0::std_random prng;                 //!
    std::default_random_engine generator;      //!

    string fParentName;
    string fDecayType;

    int fDaugherLevel;

   public:
    void SetParticleModel(std::string modelstring);
    void VirtualUpdate();
    void InitFromConfigFile();
    virtual Int_t GetNumberOfParticles() { return fParticles.size(); }
    virtual TRestGeant4Particle GetParticle(int i) { return fParticles[i]; }

    virtual void RemoveParticles() { fParticles.clear(); }
    virtual void AddParticle(TRestGeant4Particle ptcle) { fParticles.push_back(ptcle); }

    TRestGeant4ParticleCollectionDecay0() : generator((uintptr_t)this), prng(generator) {
        fDecay0Model = new bxdecay0::decay0_generator();
    }
    ~TRestGeant4ParticleCollectionDecay0() { delete fDecay0Model; }
    ClassDef(TRestGeant4ParticleCollectionDecay0, 1);
};
#endif
