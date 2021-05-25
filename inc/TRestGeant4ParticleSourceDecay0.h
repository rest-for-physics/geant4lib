#ifndef TRestParticleSourceDecay0_Class
#define TRestParticleSourceDecay0_Class

#include <bxdecay0/decay0_generator.h>
#include <bxdecay0/event.h>
#include <bxdecay0/std_random.h>

#include <iostream>

#include "TObject.h"
#include "TRestMetadata.h"
#include "TRestGeant4ParticleSource.h"

class TRestGeant4ParticleSourceDecay0 : public TRestGeant4ParticleSource {
   protected:
    bxdecay0::decay0_generator* fDecay0Model;  //!
    bxdecay0::std_random* prng;                //!
    std::default_random_engine* generator;     //!

    string fParentName;
    string fDecayType;

    int fSeed;
    int fDaugherLevel;

   public:
    void Update() override;
    void InitFromConfigFile() override;
    Int_t GetNumberOfParticles() { return fParticles.size(); }
    void PrintParticleSource() override;

    TRestGeant4ParticleSourceDecay0();
    ~TRestGeant4ParticleSourceDecay0() { delete fDecay0Model; }
    ClassDefOverride(TRestGeant4ParticleSourceDecay0, 1);
};
#endif