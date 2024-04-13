
#ifndef REST_TRESTGEANT4PARTICLESOURCECOSMICS_H
#define REST_TRESTGEANT4PARTICLESOURCECOSMICS_H

#include <TH2D.h>
#include <TRandom3.h>
#include <TRestGeant4ParticleSource.h>

class TRestGeant4ParticleSourceCosmics : public TRestGeant4ParticleSource {
   private:
    std::set<std::string> fParticleNames;
    std::string fFilename;
    std::map<std::string, double> fParticleWeights;

    std::map<std::string, TH2D*> fHistograms;
    std::map<std::string, TH2D*> fHistogramsTransformed;  //!

    static std::mutex fMutex;
    static std::unique_ptr<TRandom3> fRandom;

   public:
    void Update() override;
    void InitFromConfigFile() override;

    static void SetSeed(unsigned int seed);

    TRestGeant4ParticleSourceCosmics();
    ~TRestGeant4ParticleSourceCosmics() = default;

    const char* GetName() const override { return "TRestGeant4ParticleSourceCosmics"; }

    ClassDefOverride(TRestGeant4ParticleSourceCosmics, 1);
};

#endif  // REST_TRESTGEANT4PARTICLESOURCECOSMICS_H
