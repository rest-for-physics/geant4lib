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
    std::map<Int_t, TString> fVolumeNamesMap = {};

   public:
    void InsertProcessName(Int_t id, const TString& name);

   public:
    inline TRestGeant4PhysicsInfo() = default;
    ClassDef(TRestGeant4PhysicsInfo, 1);
    void Print() const;
};
#endif  // REST_TRESTGEANT4PHYSICSINFO_H
