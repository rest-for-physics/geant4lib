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

#include <TString.h>
#include <TVector2.h>
#include <TVector3.h>

#include <iostream>

#include "TObject.h"
//
#include "TRestGeant4Particle.h"
#include "TRestMetadata.h"

class TRestGeant4ParticleSource : public TRestGeant4Particle, public TRestMetadata {
   private:
    void ReadEventDataFile(TString fName);
    bool ReadNewDecay0File(TString fileName);
    bool ReadOldDecay0File(TString fileName);

   protected:
    TString fAngularDistType;
    TString fEnergyDistType;
    TVector2 fEnergyRange;

    TString fSpectrumFilename;
    TString fSpectrumName;

    TString fAngularFilename;
    TString fAngularName;

    TString fGenFilename;
    // store a set of generated particles
    std::vector<TRestGeant4Particle> fParticles;  //
    // store a list of particle set templates that could be used to generate fParticles
    std::vector<std::vector<TRestGeant4Particle>> fParticlesTemplate;  //!
    // the random method to generate 0~1 equally distributed numbers
    double (*fRndmMethod)();  //!

   public:
    virtual void Update();
    virtual void InitFromConfigFile();
    static TRestGeant4ParticleSource* instantiate(std::string model = "");

    inline TString GetParticleName() const { return fParticleName; }
    inline TString GetAngularDistType() const { return fAngularDistType; }
    inline TVector3 GetDirection() const { return fDirection; }
    inline TString GetEnergyDistType() const { return fEnergyDistType; }
    inline TVector2 GetEnergyRange() const { return fEnergyRange; }
    inline Double_t GetMinEnergy() const { return fEnergyRange.X(); }
    inline Double_t GetMaxEnergy() const { return fEnergyRange.Y(); }
    inline TString GetSpectrumFilename() const { return fSpectrumFilename; }
    inline TString GetSpectrumName() const { return fSpectrumName; }
    inline TString GetAngularFilename() const { return fAngularFilename; }
    inline TString GetAngularName() const { return fAngularName; }
    inline TString GetGenFilename() const { return fGenFilename; }

    inline std::vector<TRestGeant4Particle> GetParticles() const { return fParticles; }

    inline void SetAngularDistType(const TString& type) { fAngularDistType = type; }
    inline void SetEnergyDistType(const TString& type) { fEnergyDistType = type; }
    inline void SetEnergyRange(const TVector2& range) { fEnergyRange = range; }
    inline void SetSpectrumFilename(const TString& spectrumFilename) { fSpectrumFilename = spectrumFilename; }
    inline void SetSpectrumName(const TString& spectrumName) { fSpectrumName = spectrumName; }
    inline void SetAngularFilename(const TString& angFilename) { fAngularFilename = angFilename; }
    inline void SetAngularName(const TString& angularName) { fAngularName = angularName; }
    inline void SetGenFilename(const TString& name) { fGenFilename = name; }
    inline void SetRndmMethod(double (*method)()) { fRndmMethod = method; }

    inline void AddParticle(const TRestGeant4Particle& particle) { fParticles.push_back(particle); }
    inline void RemoveParticles() { fParticles.clear(); }
    inline void FlushParticlesTemplate() {
        fParticlesTemplate.push_back(fParticles);
        fParticles.clear();
    }
    inline void RemoveTemplates() { fParticlesTemplate.clear(); }

    virtual void PrintParticleSource();

    // Constructor
    TRestGeant4ParticleSource();
    // Destructor
    virtual ~TRestGeant4ParticleSource();

    ClassDef(TRestGeant4ParticleSource, 3);
};
#endif
