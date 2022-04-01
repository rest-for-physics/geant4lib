//
// Created by lobis on 31/03/2022.
//

#include "TRestGeant4PhysicsInfo.h"

#include <iostream>

using namespace std;

ClassImp(TRestGeant4PhysicsInfo);

void TRestGeant4PhysicsInfo::Print() const {
    vector<Int_t> ids = {};
    for (const auto& [id, _] : fProcessNamesMap) {
        ids.push_back(id);
    }
    sort(ids.begin(), ids.end());

    cout << "ProcessNames:" << endl;
    for (const auto& id : ids) {
        cout << "\t" << id << " - " << GetProcessName(id) << endl;
    }
}

void TRestGeant4PhysicsInfo::InsertProcessName(Int_t id, const TString& processName) {
    fProcessNamesMap[id] = processName;
    fProcessNamesReverseMap[processName] = id;
}

void TRestGeant4PhysicsInfo::InsertParticleName(Int_t id, const TString& particleName) {
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
    return GetOrDefaultMapValueFromKey<Int_t, TString>(&fProcessNamesMap, id);
}

Int_t TRestGeant4PhysicsInfo::GetParticleID(const TString& processName) const {
    return GetOrDefaultMapValueFromKey<TString, Int_t>(&fProcessNamesReverseMap, processName);
}
