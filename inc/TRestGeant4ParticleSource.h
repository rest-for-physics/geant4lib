//////////////////////////////////////////////////////////////////////////
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4ParticleSource.h
///
///             Class to store a particle definition
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///
//////////////////////////////////////////////////////////////////////////

#ifndef RestCore_TRestGeant4ParticleSource
#define RestCore_TRestGeant4ParticleSource

#include <TRestMetadata.h>
#include <TString.h>
#include <TVector2.h>
#include <TVector3.h>

#include <iostream>

#include "TRestGeant4Particle.h"

class TRestGeant4ParticleSource : public TRestGeant4Particle, public TRestMetadata {
   private:
    void ReadEventDataFile(TString fName);
    bool ReadNewDecay0File(TString fileName);
    bool ReadOldDecay0File(TString fileName);

   protected:
    TString fAngularDistributionType = "Flux";
    TString fAngularDistributionFilename;
    TString fAngularDistributionNameInFile;
    TString fAngularDistributionFormulaString;

    TString fEnergyDistributionType = "Mono";
    TString fEnergyDistributionFilename;
    TString fEnergyDistributionNameInFile;
    TString fEnergyDistributionFormulaString;
    TVector2 fEnergyDistributionRange;

    TString fGenFilename;
    // store a set of generated particles
    std::vector<TRestGeant4Particle> fParticles;
    // store a list of particle set templates that could be used to generate fParticles
    std::vector<std::vector<TRestGeant4Particle>> fParticlesTemplate;  //!
    // the random method to generate 0~1 equally distributed numbers
    double (*fRandomMethod)();  //!

   public:
    virtual void Update();
    virtual void InitFromConfigFile();
    static TRestGeant4ParticleSource* instantiate(std::string model = "");

    inline TString GetParticleName() const { return fParticleName; }
    inline TVector3 GetDirection() const { return fDirection; }

    inline TString GetEnergyDistributionType() const { return fEnergyDistributionType; }
    inline TVector2 GetEnergyDistributionRange() const { return fEnergyDistributionRange; }
    inline Double_t GetEnergyDistributionRangeMin() const { return fEnergyDistributionRange.X(); }
    inline Double_t GetEnergyDistributionRangeMax() const { return fEnergyDistributionRange.Y(); }
    inline TString GetEnergyDistributionFilename() const { return fEnergyDistributionFilename; }
    inline TString GetEnergyDistributionNameInFile() const { return fEnergyDistributionNameInFile; }

    inline TString GetAngularDistributionType() const { return fAngularDistributionType; }
    inline TString GetAngularDistributionFilename() const { return fAngularDistributionFilename; }
    inline TString GetAngularDistributionNameInFile() const { return fAngularDistributionNameInFile; }

    inline TString GetGenFilename() const { return fGenFilename; }

    inline std::vector<TRestGeant4Particle> GetParticles() const { return fParticles; }

    inline void SetAngularDistributionType(const TString& type) { fAngularDistributionType = type; }
    inline void SetAngularDistributionFilename(const TString& filename) {
        fAngularDistributionFilename = filename;
    }
    inline void SetAngularDistributionNameInFile(const TString& name) {
        fAngularDistributionNameInFile = name;
    }

    inline void SetEnergyDistributionType(const TString& type) { fEnergyDistributionType = type; }
    inline void SetEnergyDistributionRange(const TVector2& range) { fEnergyDistributionRange = range; }
    inline void SetEnergyDistributionFilename(const TString& filename) {
        fEnergyDistributionFilename = filename;
    }
    inline void SetEnergyDistributionNameInFile(const TString& name) { fEnergyDistributionNameInFile = name; }

    inline void SetGenFilename(const TString& name) { fGenFilename = name; }

    inline void SetRandomMethod(double (*method)()) { fRandomMethod = method; }

    inline void AddParticle(const TRestGeant4Particle& particle) { fParticles.push_back(particle); }

    inline void RemoveParticles() { fParticles.clear(); }
    inline void RemoveTemplates() { fParticlesTemplate.clear(); }
    inline void FlushParticlesTemplate() {
        fParticlesTemplate.push_back(fParticles);
        fParticles.clear();
    }

    virtual void PrintParticleSource();

    // Constructor
    TRestGeant4ParticleSource();
    // Destructor
    virtual ~TRestGeant4ParticleSource();

    ClassDef(TRestGeant4ParticleSource, 4);
};
#endif
