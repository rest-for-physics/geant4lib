#ifndef TRestParticleSourceCry_Class
#define TRestParticleSourceCry_Class

#include <iostream>

#ifdef USE_CRY
#include <CRYGenerator.h>
#include <CRYSetup.h>
#endif

#include <TRestGeant4ParticleSource.h>

class TRestGeant4ParticleSourceCry : public TRestGeant4ParticleSource {
   protected:
#ifdef USE_CRY
    CRYGenerator* fCRYGenerator = nullptr;
#endif

   public:
    void Update() override;
    void InitFromConfigFile() override;
    inline Int_t GetNumberOfParticles() const { return fParticles.size(); }
    void PrintParticleSource() override;

    TRestGeant4ParticleSourceCry();
    ~TRestGeant4ParticleSourceCry() {}
    ClassDefOverride(TRestGeant4ParticleSourceCry, 1);
};
#endif
