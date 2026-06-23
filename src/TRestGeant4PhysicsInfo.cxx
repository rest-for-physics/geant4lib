
#include "TRestGeant4PhysicsInfo.h"

#include <iostream>

using namespace std;

ClassImp(TRestGeant4PhysicsInfo);

std::mutex insertMutex;

set<TString> TRestGeant4PhysicsInfo::GetAllParticles() const {
    set<TString> particles = {};
    for (const auto& [_, name] : fParticleNamesMap) {
        particles.insert(name);
    }
    return particles;
}

std::set<TString> TRestGeant4PhysicsInfo::GetAllProcesses() const {
    set<TString> processes = {};
    for (const auto& [_, name] : fProcessNamesMap) {
        processes.insert(name);
    }
    return processes;
}

std::set<TString> TRestGeant4PhysicsInfo::GetAllProcessTypes() const {
    set<TString> types = {};
    for (const auto& [_, type] : fProcessTypesMap) {
        types.insert(type);
    }
    return types;
}

void TRestGeant4PhysicsInfo::PrintParticles() const {
    const auto particleNames = GetAllParticles();
    cout << "Particles:" << endl;
    for (const auto& name : particleNames) {
        const auto id = GetParticleID(name);
        cout << "\t" << name << " - " << id << endl;
    }
}

void TRestGeant4PhysicsInfo::PrintProcesses() const {
    const auto processNames = GetAllProcesses();
    cout << "Processes:" << endl;
    for (const auto& name : processNames) {
        const auto id = GetProcessID(name);
        cout << "\t" << name << " - " << id << endl;
    }
}

void TRestGeant4PhysicsInfo::Print() const {
    PrintParticles();
    PrintProcesses();
}

void TRestGeant4PhysicsInfo::InsertProcessName(Int_t id, const TString& processName,
                                               const TString& processType) {
    if (fProcessNamesMap.count(id) > 0) {
        return;
    }
    std::lock_guard<std::mutex> lock(insertMutex);
    fProcessNamesMap[id] = processName;
    fProcessNamesReverseMap[processName] = id;

    fProcessTypesMap[processName] = processType;
}

void TRestGeant4PhysicsInfo::InsertParticleName(Int_t id, const TString& particleName) {
    if (fParticleNamesMap.count(id) > 0) {
        return;
    }
    std::lock_guard<std::mutex> lock(insertMutex);
    fParticleNamesMap[id] = particleName;
    fParticleNamesReverseMap[particleName] = id;
}

template <typename T, typename U>
U GetOrDefaultMapValueFromKey(const map<T, U>* pMap, const T& key) {
    if (pMap->count(key) > 0) {
        return pMap->at(key);
    }
    return {};
}

TString TRestGeant4PhysicsInfo::GetProcessName(Int_t id) const {
    return GetOrDefaultMapValueFromKey<Int_t, TString>(&fProcessNamesMap, id);
}

Int_t TRestGeant4PhysicsInfo::GetProcessID(const TString& processName) const {
    return GetOrDefaultMapValueFromKey<TString, Int_t>(&fProcessNamesReverseMap, processName);
}

TString TRestGeant4PhysicsInfo::GetParticleName(Int_t id) const {
    return GetOrDefaultMapValueFromKey<Int_t, TString>(&fParticleNamesMap, id);
}

Int_t TRestGeant4PhysicsInfo::GetParticleID(const TString& processName) const {
    return GetOrDefaultMapValueFromKey<TString, Int_t>(&fParticleNamesReverseMap, processName);
}

TString TRestGeant4PhysicsInfo::GetProcessType(const TString& processName) const {
    return GetOrDefaultMapValueFromKey<TString, TString>(&fProcessTypesMap, processName);
}

bool TRestGeant4PhysicsInfo::Merge(const TRestGeant4PhysicsInfo& other, string* conflictReason) {
    const auto setConflict = [&](const string& message) {
        if (conflictReason != nullptr) {
            *conflictReason = message;
        }
    };

    for (const auto& [id, name] : other.fProcessNamesMap) {
        if (fProcessNamesMap.count(id) > 0 && fProcessNamesMap.at(id) != name) {
            setConflict("process id " + to_string(id) + " maps to both '" +
                        string(fProcessNamesMap.at(id)) + "' and '" + string(name) + "'");
            return false;
        }
    }
    for (const auto& [name, id] : other.fProcessNamesReverseMap) {
        if (fProcessNamesReverseMap.count(name) > 0 && fProcessNamesReverseMap.at(name) != id) {
            setConflict("process '" + string(name) + "' maps to both id " +
                        to_string(fProcessNamesReverseMap.at(name)) + " and id " + to_string(id));
            return false;
        }
    }
    for (const auto& [name, type] : other.fProcessTypesMap) {
        if (fProcessTypesMap.count(name) > 0 && fProcessTypesMap.at(name) != type) {
            setConflict("process '" + string(name) + "' maps to both type '" +
                        string(fProcessTypesMap.at(name)) + "' and type '" + string(type) + "'");
            return false;
        }
    }
    for (const auto& [id, name] : other.fParticleNamesMap) {
        if (fParticleNamesMap.count(id) > 0 && fParticleNamesMap.at(id) != name) {
            setConflict("particle id " + to_string(id) + " maps to both '" +
                        string(fParticleNamesMap.at(id)) + "' and '" + string(name) + "'");
            return false;
        }
    }
    for (const auto& [name, id] : other.fParticleNamesReverseMap) {
        if (fParticleNamesReverseMap.count(name) > 0 && fParticleNamesReverseMap.at(name) != id) {
            setConflict("particle '" + string(name) + "' maps to both id " +
                        to_string(fParticleNamesReverseMap.at(name)) + " and id " + to_string(id));
            return false;
        }
    }

    lock_guard<mutex> lock(insertMutex);
    fProcessNamesMap.insert(other.fProcessNamesMap.begin(), other.fProcessNamesMap.end());
    fProcessNamesReverseMap.insert(other.fProcessNamesReverseMap.begin(), other.fProcessNamesReverseMap.end());
    fProcessTypesMap.insert(other.fProcessTypesMap.begin(), other.fProcessTypesMap.end());
    fParticleNamesMap.insert(other.fParticleNamesMap.begin(), other.fParticleNamesMap.end());
    fParticleNamesReverseMap.insert(other.fParticleNamesReverseMap.begin(), other.fParticleNamesReverseMap.end());

    return true;
}
