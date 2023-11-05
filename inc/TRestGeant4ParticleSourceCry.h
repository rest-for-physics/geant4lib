#ifndef TRestParticleSourceCry_Class
#define TRestParticleSourceCry_Class

#include <iostream>

#ifdef USE_CRY
#include <CRYGenerator.h>
#include <CRYSetup.h>
#endif

#include <TRestGeant4ParticleSource.h>

class TRestGeant4ParticleSourceCry : public TRestGeant4ParticleSource {
   private:
    Int_t fReturnNeutrons = 1;
    Int_t fReturnProtons = 1;
    Int_t fReturnGammas = 1;
    Int_t fReturnElectrons = 1;
    Int_t fReturnPions = 1;
    Int_t fReturnKaons = 1;
    Int_t fReturnMuons = 1;

    Int_t fNParticlesMin = 1;
    Int_t fNParticlesMax = 1000000;

    Double_t fXOffset = 0;
    Double_t fYOffset = 0;
    Double_t fZOffset = 0;

    std::string fDate = "7-1-2012";

    Double_t fLatitude = 90.0;
    ;
    Double_t fAltitude = 0.0;
    ;

    Double_t fSubBoxLength = 100.0;
    ;

   protected:
#ifdef USE_CRY
    CRYGenerator* fCRYGenerator = nullptr;
#endif

   public:
    void Update() override;
    void InitFromConfigFile() override;
    inline Int_t GetNumberOfParticles() const { return fParticles.size(); }
    void PrintMetadata() override;

    TRestGeant4ParticleSourceCry();
    ~TRestGeant4ParticleSourceCry() {}
    ClassDefOverride(TRestGeant4ParticleSourceCry, 1);
};
#endif
