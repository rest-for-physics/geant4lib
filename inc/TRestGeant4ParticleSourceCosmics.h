
#ifndef REST_TRESTGEANT4PARTICLESOURCECOSMICS_H
#define REST_TRESTGEANT4PARTICLESOURCECOSMICS_H

#include <TRestGeant4ParticleSource.h>

class TRestGeant4ParticleSourceCosmics : public TRestGeant4ParticleSource {
   private:
    std::set<std::string> fParticleNames;
    std::string fFilename;
    std::map<std::string, double> fParticleWeights;

    double (*fRandomGenerator)() = nullptr;

   public:
    void Update() override;
    void InitFromConfigFile() override;

    void SetGenerator(double (*generator)()) { fRandomGenerator = generator; }

    TRestGeant4ParticleSourceCosmics();
    ~TRestGeant4ParticleSourceCosmics() = default;

    ClassDefOverride(TRestGeant4ParticleSourceCosmics, 1);
};

#endif  // REST_TRESTGEANT4PARTICLESOURCECOSMICS_H
