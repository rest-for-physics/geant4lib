//
// Created by lobis on 31/03/2022.
//

#ifndef REST_TRESTGEANT4PHYSICSINFO_H
#define REST_TRESTGEANT4PHYSICSINFO_H

#include <TString.h>

#include <map>
#include <vector>

class TRestGeant4PhysicsInfo {
   private:
    std::map<Int_t, TString> fProcessNamesMap = {};
    std::map<TString, Int_t> fProcessNamesReverseMap = {};

    std::map<Int_t, TString> fParticleNamesMap = {};
    std::map<TString, Int_t> fParticleNamesReverseMap = {};

   public:
    TString GetProcessName(Int_t id) const;
    Int_t GetProcessID(const TString& processName) const;
    void InsertProcessName(Int_t id, const TString& processName);

    TString GetParticleName(Int_t id) const;
    Int_t GetParticleID(const TString& processName) const;
    void InsertParticleName(Int_t id, const TString& particleName);

   public:
    inline TRestGeant4PhysicsInfo() = default;
    inline ~TRestGeant4PhysicsInfo() = default;
    ClassDef(TRestGeant4PhysicsInfo, 1);

    void Print() const;
};
#endif  // REST_TRESTGEANT4PHYSICSINFO_H
