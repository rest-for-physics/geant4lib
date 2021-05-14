///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4ParticleCollection.cxx
///
///             Base class from which to inherit all other event classes in REST
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#include "TRestGeant4ParticleCollection.h"

#include <TClass.h>

#include "TRestStringHelper.h"

using namespace std;

ClassImp(TRestGeant4ParticleCollection);

TRestGeant4ParticleCollection::TRestGeant4ParticleCollection() = default;

TRestGeant4ParticleCollection::~TRestGeant4ParticleCollection() = default;

TRestGeant4ParticleCollection* TRestGeant4ParticleCollection::instantiate(string model) {
    if (model == "" || model == "geant4") {
        // use default generator
        return new TRestGeant4ParticleCollection();
    } else {
        // use specific generator
        // naming convention: TRestGeant4ParticleCollectionXXX
        // currently supported generator: decay0
        // in future we may add wrapper of generators: cry (for muon), pythia (for HEP), etc.
        model[0] = *REST_StringHelper::ToUpper(string(&model[0], 1)).c_str();
        TClass* c = TClass::GetClass(("TRestGeant4ParticleCollection" + model).c_str());
        if (c)  // this means we have the package installed
        {
            return (TRestGeant4ParticleCollection*)c->New();
        } else {
            cout << "REST ERROR! generator wrapper \"" << ("TRestGeant4ParticleCollection" + model)
                 << "\" not found!" << endl;
        }
    }
    return nullptr;
}
