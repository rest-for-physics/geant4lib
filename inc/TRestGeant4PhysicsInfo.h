#ifndef REST_TRESTGEANT4PHYSICSINFO_H
#define REST_TRESTGEANT4PHYSICSINFO_H

#include <TString.h>

#include <map>
#include <mutex>
#include <set>
#include <vector>

class G4VProcess;

class TRestGeant4PhysicsInfo {
    ClassDef(TRestGeant4PhysicsInfo, 2);

   private:
    std::map<Int_t, TString> fProcessNamesMap = {};
    std::map<TString, Int_t> fProcessNamesReverseMap = {};

    std::map<Int_t, TString> fParticleNamesMap = {};
    std::map<TString, Int_t> fParticleNamesReverseMap = {};

    std::map<TString, TString> fProcessTypesMap = {};  // process name -> process type

    std::mutex fMutex;  //!

   public:
    TString GetProcessName(Int_t id) const;
    Int_t GetProcessID(const TString& processName) const;
    void InsertProcessName(Int_t id, const TString& processName, const TString& processType);
    std::set<TString> GetAllParticles() const;

    TString GetParticleName(Int_t id) const;
    Int_t GetParticleID(const TString& processName) const;
    void InsertParticleName(Int_t id, const TString& particleName);
    std::set<TString> GetAllProcesses() const;

    TString GetProcessType(const TString& processName) const;
    std::set<TString> GetAllProcessTypes() const;

   public:
    inline TRestGeant4PhysicsInfo() = default;
    inline ~TRestGeant4PhysicsInfo() = default;

    void Print() const;
    void PrintProcesses() const;
    void PrintParticles() const;

    static Int_t GetProcessIDFromGeant4Process(const G4VProcess*);  // implemented in restG4
};
#endif  // REST_TRESTGEANT4PHYSICSINFO_H
