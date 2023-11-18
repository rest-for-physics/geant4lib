#ifndef TRestParticleSourceCry_Class
#define TRestParticleSourceCry_Class

#include <TRandom3.h>

#include <iostream>

#ifdef USE_CRY
#include <CRYGenerator.h>
#include <CRYSetup.h>
#endif

#include <TRestGeant4ParticleSource.h>

class TRestGeant4ParticleSourceCry : public TRestGeant4ParticleSource {
   private:
    /// It defines if secondary neutrons will be produced by the generator
    Int_t fReturnNeutrons = 1;
    /// It defines if secondary protons will be produced by the generator
    Int_t fReturnProtons = 1;
    /// It defines if secondary photons will be produced by the generator
    Int_t fReturnGammas = 1;
    /// It defines if secondary gammas will be produced by the generator
    Int_t fReturnElectrons = 1;
    /// It defines if secondary electrons will be produced by the generator
    Int_t fReturnPions = 1;
    /// It defines if secondary pions will be produced by the generator
    Int_t fReturnKaons = 1;
    /// It defines if secondary muons will be produced by the generator
    Int_t fReturnMuons = 1;

    /// Showers with number of particles below this number will be truncated.
    Int_t fNParticlesMin = 1;
    /// Showers with number of particles above this number will be truncated.
    Int_t fNParticlesMax = 1000000;

    /// This is likely the X-coordinate where the box of generated particles is centered.
    Double_t fXOffset = 0;
    /// This is likely the Y-coordinate where the box of generated particles is centered.
    Double_t fYOffset = 0;
    /// This is likely the Z-coordinate where the box of generated particles is centered.
    Double_t fZOffset = 0;

    /// It will adjust the cosmic-ray distributions to the 11-year solar cycle.
    std::string fDate = "7\1\2012";

    /// The allowed range is -90 to 90. 0 defines the magnetic equator.
    Double_t fLatitude = 90.0;
    ;

    /// Allowed values are 0, 2100 and 11300 m.
    Double_t fAltitude = 0.0;
    ;

    /// The size of the box where the CRY generator produces particles (in m).
    Double_t fSubBoxLength = 100.0;
    ;

    /// Seed used in random generator
    Int_t fSeed = 0;  //<

    /// Internal process random generator
    TRandom3* fRandom = nullptr;  //!

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
