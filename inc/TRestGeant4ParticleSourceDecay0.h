#ifndef RestCore_TRestGeant4ParticleSourceD0
#define RestCore_TRestGeant4ParticleSourceD0

#include <bxdecay0/decay0_generator.h>
#include <bxdecay0/event.h>
#include <bxdecay0/std_random.h>

#include <iostream>

#include "TObject.h"
#include "TRestGeant4ParticleSource.h"
#include "TRestMetadata.h"

using namespace std;

class TRestGeant4ParticleSourceDecay0 : public TRestGeant4ParticleSource, public TRestMetadata {
   protected:
    bxdecay0::decay0_generator* fDecay0Model;  //!
    bxdecay0::std_random* prng;                 //!
    std::default_random_engine* generator;      //!

    string fParentName;
    string fDecayType;

    int fSeed;
    int fDaugherLevel;

   public:
    void Config(std::string modelstring);
    void Update();
    void InitFromConfigFile();
    Int_t GetNumberOfParticles() { return fParticles.size(); }
    void PrintParticleSource();

    TRestGeant4ParticleSourceDecay0();
    ~TRestGeant4ParticleSourceDecay0() { delete fDecay0Model; }
    ClassDef(TRestGeant4ParticleSourceDecay0, 1);
};
#endif