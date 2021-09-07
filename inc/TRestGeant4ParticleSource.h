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
    TString fSpectrumEnergyUnits;

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

    TString GetParticleName() { return fParticleName; }
    TString GetAngularDistType() { return fAngularDistType; }
    TVector3 GetDirection() { return fDirection; }
    TString GetEnergyDistType() { return fEnergyDistType; }
    TVector2 GetEnergyRange() { return fEnergyRange; }
    Double_t GetMinEnergy() { return fEnergyRange.X(); }
    Double_t GetMaxEnergy() { return fEnergyRange.Y(); }
    TString GetSpectrumFilename() { return fSpectrumFilename; }
    TString GetSpectrumName() { return fSpectrumName; }
    inline TString GetSpectrumEnergyUnits() const { return fSpectrumEnergyUnits; }
    TString GetAngularFilename() { return fAngularFilename; }
    TString GetAngularName() { return fAngularName; }
    TString GetGenFilename() { return fGenFilename; }

    std::vector<TRestGeant4Particle> GetParticles() { return fParticles; }

    void SetAngularDistType(TString type) { fAngularDistType = type; }
    void SetEnergyDistType(TString type) { fEnergyDistType = type; }
    void SetEnergyRange(TVector2 range) { fEnergyRange = range; }
    void SetSpectrumFilename(TString spctFilename) { fSpectrumFilename = spctFilename; }
    void SetSpectrumName(TString spctName) { fSpectrumName = spctName; }
    void SetSpectrumEnergyUnits(TString units) { fSpectrumEnergyUnits = type; }
    void SetAngularFilename(TString angFilename) { fAngularFilename = angFilename; }
    void SetAngularName(TString angName) { fAngularName = angName; }
    void SetGenFilename(TString name) { fGenFilename = name; }

    void SetRndmMethod(double (*method)()) { fRndmMethod = method; }

    void AddParticle(TRestGeant4Particle p) { fParticles.push_back(p); }
    void RemoveParticles() { fParticles.clear(); }
    void FlushParticlesTemplate() {
        fParticlesTemplate.push_back(fParticles);
        fParticles.clear();
    }
    void RemoveTemplates() { fParticlesTemplate.clear(); }

    virtual void PrintParticleSource();

    // Construtor
    TRestGeant4ParticleSource();
    // Destructor
    virtual ~TRestGeant4ParticleSource();

    ClassDef(TRestGeant4ParticleSource, 3);
};
#endif
