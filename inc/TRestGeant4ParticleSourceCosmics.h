
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
    std::pair<double, double> fEnergyRange = {0, 0};

    unsigned long long fCounterEnergyTotal = 0;
    unsigned long long fCounterEnergyAccepted = 0;

    std::map<std::string, TH2D*> fHistograms;
    std::map<std::string, TH2D*> fHistogramsTransformed;

    static std::mutex fMutex;
    static std::unique_ptr<TRandom3> fRandom;

   public:
    void Update() override;
    void InitFromConfigFile() override;

    static void SetSeed(unsigned int seed);

    TRestGeant4ParticleSourceCosmics();
    ~TRestGeant4ParticleSourceCosmics() = default;

    const char* GetName() const override { return "TRestGeant4ParticleSourceCosmics"; }

    std::map<std::string, TH2D*> GetHistogramsTransformed() const { return fHistogramsTransformed; }
    std::set<std::string> GetParticleNames() const { return fParticleNames; }

    double GetEnergyRangeScalingFactor() const;

    ClassDefOverride(TRestGeant4ParticleSourceCosmics, 3);
};

#endif  // REST_TRESTGEANT4PARTICLESOURCECOSMICS_H
