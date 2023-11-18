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

#include <TF1.h>
#include <TF2.h>
#include <TRestMetadata.h>
#include <TString.h>
#include <TVector2.h>
#include <TVector3.h>

#include <iostream>

#include "TRestGeant4Particle.h"
#include "TRestGeant4PrimaryGeneratorInfo.h"

class TRestGeant4ParticleSource : public TRestGeant4Particle, public TRestMetadata {
   private:
    void ReadEventDataFile(TString fName);
    bool ReadNewDecay0File(TString fileName);
    bool ReadOldDecay0File(TString fileName);

   protected:
    TString fAngularDistributionType = "Flux";
    TString fAngularDistributionFilename;
    TString fAngularDistributionNameInFile;
    size_t fAngularDistributionFormulaNPoints = 500;
    TF1* fAngularDistributionFunction = nullptr;
    TVector2 fAngularDistributionRange;

    TString fEnergyDistributionType = "Mono";
    TString fEnergyDistributionFilename;
    TString fEnergyDistributionNameInFile;
    size_t fEnergyDistributionFormulaNPoints = 5000;
    TF1* fEnergyDistributionFunction = nullptr;
    TVector2 fEnergyDistributionRange;

    TF2* fEnergyAndAngularDistributionFunction = nullptr;

    TString fGenFilename;
    // store a set of generated particles
    std::vector<TRestGeant4Particle> fParticles;
    // store a list of particle set templates that could be used to generate fParticles
    std::vector<std::vector<TRestGeant4Particle>> fParticlesTemplate;  //!
    // the random method to generate 0~1 equally distributed numbers
    double (*fRandomMethod)();  //!

   public:
    virtual void Update();
    virtual void InitFromConfigFile() override;
    static TRestGeant4ParticleSource* instantiate(std::string model = "");

    inline TVector3 GetDirection() const {
        if (fDirection.Mag() <= 0) {
            std::cout << "TRestGeant4ParticleSource::GetDirection: "
                      << "Direction cannot be the zero vector" << std::endl;
            exit(1);
        }
        return fDirection;
    }

    inline TString GetEnergyDistributionType() const { return fEnergyDistributionType; }
    inline TVector2 GetEnergyDistributionRange() const { return fEnergyDistributionRange; }
    inline Double_t GetEnergyDistributionRangeMin() const { return fEnergyDistributionRange.X(); }
    inline Double_t GetEnergyDistributionRangeMax() const { return fEnergyDistributionRange.Y(); }
    inline size_t GetEnergyDistributionFormulaNPoints() const { return fEnergyDistributionFormulaNPoints; }
    inline TString GetEnergyDistributionFilename() const { return fEnergyDistributionFilename; }
    inline TString GetEnergyDistributionNameInFile() const { return fEnergyDistributionNameInFile; }
    inline const TF1* GetEnergyDistributionFunction() const { return fEnergyDistributionFunction; }

    inline TString GetAngularDistributionType() const { return fAngularDistributionType; }
    inline TVector2 GetAngularDistributionRange() const { return fAngularDistributionRange; }
    inline Double_t GetAngularDistributionRangeMin() const { return fAngularDistributionRange.X(); }
    inline Double_t GetAngularDistributionRangeMax() const { return fAngularDistributionRange.Y(); }
    inline size_t GetAngularDistributionFormulaNPoints() const { return fAngularDistributionFormulaNPoints; }
    inline TString GetAngularDistributionFilename() const { return fAngularDistributionFilename; }
    inline TString GetAngularDistributionNameInFile() const { return fAngularDistributionNameInFile; }
    inline const TF1* GetAngularDistributionFunction() const { return fAngularDistributionFunction; }

    inline const TF2* GetEnergyAndAngularDistributionFunction() const {
        return fEnergyAndAngularDistributionFunction;
    }

    inline TString GetGenFilename() const { return fGenFilename; }

    inline std::vector<TRestGeant4Particle> GetParticles() const { return fParticles; }

    inline void SetAngularDistributionType(const TString& type) {
        fAngularDistributionType = TRestGeant4PrimaryGeneratorTypes::AngularDistributionTypesToString(
            TRestGeant4PrimaryGeneratorTypes::StringToAngularDistributionTypes(type.Data()));
    }
    inline void SetAngularDistributionRange(const TVector2& range) {
        fAngularDistributionRange = range;
        if (fAngularDistributionRange.X() < 0) {
            fAngularDistributionRange.Set(0, fAngularDistributionRange.Y());
        }
        if (fAngularDistributionRange.Y() < 0) {
            fAngularDistributionRange.Set(fAngularDistributionRange.X(), 0);
        }
        if (fAngularDistributionRange.Y() > TMath::Pi()) {
            fAngularDistributionRange.Set(fAngularDistributionRange.X(), TMath::Pi());
        }
        if (fAngularDistributionRange.X() > fAngularDistributionRange.Y()) {
            fAngularDistributionRange.Set(fAngularDistributionRange.Y(), fAngularDistributionRange.X());
        }
    }
    inline void SetAngularDistributionFormulaNPoints(size_t nPoints) {
        const auto nPointsMax = 10000;
        if (nPoints > nPointsMax) {
            nPoints = nPointsMax;
        }
        fAngularDistributionFormulaNPoints = nPoints;
    }
    inline void SetAngularDistributionFilename(const TString& filename) {
        fAngularDistributionFilename = filename;
    }
    inline void SetAngularDistributionNameInFile(const TString& name) {
        fAngularDistributionNameInFile = name;
    }
    inline void SetAngularDistributionFormula(const TString& formula) {
        fAngularDistributionFunction =
            (TF1*)TRestGeant4PrimaryGeneratorTypes::AngularDistributionFormulasToRootFormula(
                TRestGeant4PrimaryGeneratorTypes::StringToAngularDistributionFormulas(formula.Data()))
                .Clone();
    }

    inline void SetEnergyDistributionType(const TString& type) {
        fEnergyDistributionType = TRestGeant4PrimaryGeneratorTypes::EnergyDistributionTypesToString(
            TRestGeant4PrimaryGeneratorTypes::StringToEnergyDistributionTypes(type.Data()));
    }
    inline void SetEnergyDistributionRange(const TVector2& range) {
        fEnergyDistributionRange = range;
        if (fEnergyDistributionRange.X() < 0) {
            fEnergyDistributionRange.Set(0, fEnergyDistributionRange.Y());
        }
        if (fEnergyDistributionRange.Y() < 0) {
            fEnergyDistributionRange.Set(fEnergyDistributionRange.X(), 0);
        }
        if (fEnergyDistributionRange.X() > fEnergyDistributionRange.Y()) {
            fEnergyDistributionRange.Set(fEnergyDistributionRange.Y(), fEnergyDistributionRange.X());
        }
    }
    inline void SetEnergyDistributionFormulaNPoints(size_t nPoints) {
        const auto nPointsMax = 10000;
        if (nPoints > nPointsMax) {
            nPoints = nPointsMax;
        }
        fEnergyDistributionFormulaNPoints = nPoints;
    }
    inline void SetEnergyDistributionFilename(const TString& filename) {
        fEnergyDistributionFilename = filename;
    }
    inline void SetEnergyDistributionNameInFile(const TString& name) { fEnergyDistributionNameInFile = name; }
    inline void SetEnergyDistributionFormula(const TString& formula) {
        fEnergyDistributionFunction =
            (TF1*)TRestGeant4PrimaryGeneratorTypes::EnergyDistributionFormulasToRootFormula(
                TRestGeant4PrimaryGeneratorTypes::StringToEnergyDistributionFormulas(formula.Data()))
                .Clone();
    }

    inline void SetEnergyAndAngularDistributionFormula(const TString& formula) {
        fEnergyAndAngularDistributionFunction =
            (TF2*)TRestGeant4PrimaryGeneratorTypes::EnergyAndAngularDistributionFormulasToRootFormula(
                TRestGeant4PrimaryGeneratorTypes::StringToEnergyAndAngularDistributionFormulas(
                    formula.Data()))
                .Clone();
    }

    inline void SetGenFilename(const TString& name) { fGenFilename = name; }

    inline void SetRandomMethod(double (*method)()) { fRandomMethod = method; }

    inline void AddParticle(const TRestGeant4Particle& particle) { fParticles.push_back(particle); }

    inline void RemoveParticles() { fParticles.clear(); }
    inline void RemoveTemplates() { fParticlesTemplate.clear(); }
    inline void FlushParticlesTemplate() {
        fParticlesTemplate.push_back(fParticles);
        fParticles.clear();
    }

    virtual void PrintMetadata() override;

    // Constructor
    TRestGeant4ParticleSource();
    // Destructor
    virtual ~TRestGeant4ParticleSource();

    ClassDefOverride(TRestGeant4ParticleSource, 5);
};
#endif
