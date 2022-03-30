//
// Created by lobis on 31/03/2022.
//

#include "TRestGeant4PhysicsInfo.h"

using namespace std;

ClassImp(TRestGeant4PhysicsInfo);

void TRestGeant4PhysicsInfo::InsertProcessName(Int_t id, const TString& name) {
    if (fProcessNamesMap.count(id) > 0) {
        fProcessNamesMap[id] = &(*name);
    }
}