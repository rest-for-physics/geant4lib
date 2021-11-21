//
// Created by lobis on 15/11/2021.
//

#include "TRestGeant4Geometry.h"

#include <TGeoManager.h>

#include "TRestStringHelper.h"

using namespace std;

void addDaughter(TGeoNode* node, set<TString>* logicalVolumesSet, set<TString>* physicalVolumesSet,
                 set<TString>* assembliesSet) {
    assembliesSet->insert(node->GetVolume()->GetName());
    for (size_t i = 0; i < node->GetNdaughters(); i++) {
        TGeoNode* daughter = node->GetDaughter(i);
        if (daughter->GetVolume()->IsAssembly()) {
            addDaughter(daughter, logicalVolumesSet, physicalVolumesSet, assembliesSet);
        } else {
            logicalVolumesSet->insert(daughter->GetVolume()->GetName());
            physicalVolumesSet->insert(daughter->GetName());
        }
    }
};

void TRestGeant4Geometry::LoadGdml(const TiXmlElement& geometryDefinition, const TString& gdmlFilename) {
    LoadGdml(gdmlFilename);

    TGeoManager::Import((TString)gdmlFilename);

    // recursively add all non-assembly volume names, which should be unique according to GDML definition

    addDaughter(gGeoManager->GetTopNode(), &fGdmlLogicalVolumes, &fGdmlPhysicalVolumes,
                &fGdmlAssemblyVolumes);

    // debugging
    const map<TString, set<TString>> m = {{"Physical", fGdmlPhysicalVolumes},
                                          {"Logical", fGdmlLogicalVolumes},
                                          {"Assembly", fGdmlAssemblyVolumes}};
    for (auto const& kv : m) {
        cout << kv.first << " Volumes:" << endl;
        for (const auto& volume : kv.second) {
            cout << "\t" << volume << endl;
        }
        cout << endl;
    }
}

void TRestGeant4Geometry::LoadVolumes(const TiXmlElement& geometryDefinition) {
    auto allVolumesActive = GetFieldValue("allVolumesActive", const_cast<TiXmlElement*>(&geometryDefinition));
    if (allVolumesActive != "Not defined") {
        fAllVolumesActive = REST_StringHelper::StringToBool(allVolumesActive);
        metadata << "Setting 'fAllVolumesActive' to " << allVolumesActive << endl;
    }

    TiXmlElement* volumeDefinition = GetElement("volume", const_cast<TiXmlElement*>(&geometryDefinition));
    while (volumeDefinition) {
        Double_t chance = StringToDouble(GetFieldValue("chance", volumeDefinition));
        if (chance == -1) chance = fDefaultStorageChance;

        Double_t maxStepSize = GetDblParameterWithUnits("maxStepSize", volumeDefinition);
        if (maxStepSize < 0) maxStepSize = fDefaultMaxStepSize;

        TString name = GetFieldValue("name", volumeDefinition);

        InsertActiveVolume(name, chance, maxStepSize);
        auto sensitive = GetParameter("sensitive", volumeDefinition, "false");
        if (REST_StringHelper::StringToBool(sensitive)) {
            InsertSensitiveVolume(name);
            metadata << "Added sensitive volume: " << name << endl;
        } else {
            metadata << "Added active volume: " << name << endl;
        }

        volumeDefinition = GetNextElement(volumeDefinition);
    }

    exit(1);
}

Bool_t TRestGeant4Geometry::IsSensitiveVolume(const TString& inputPhysicalVolume) const {
    for (const auto& volume : fSensitiveVolumes) {
        if (inputPhysicalVolume.EqualTo(volume)) {
            return true;
        }
    }
    return false;
}

Bool_t TRestGeant4Geometry::IsActiveVolume(const TString& inputPhysicalVolume) const {
    for (const auto& volume : fActiveVolumes) {
        if (inputPhysicalVolume.EqualTo(volume)) {
            return true;
        }
    }
    return false;
}

Bool_t TRestGeant4Geometry::IsPhysicalVolume(const TString& inputPhysicalVolume) const {
    for (const auto& volume : fPhysicalVolumes) {
        if (inputPhysicalVolume.EqualTo(volume)) {
            return true;
        }
    }
    return false;
}

Bool_t TRestGeant4Geometry::IsLogicalVolume(const TString& inputPhysicalVolume) const {
    for (const auto& volume : fLogicalVolumes) {
        if (inputPhysicalVolume.EqualTo(volume)) {
            return true;
        }
    }
    return false;
}

TVector3 TRestGeant4Geometry::GetPhysicalVolumePosition(const TString& inputPhysicalVolume) const {
    /*
     * This method will throw exception if the requested volume does not exist, if not sure, check with
     * `IsPhysicalVolume`
     */
    return fPhysicalToPositionInWorldMap.at(inputPhysicalVolume);
}

/// Attempts to return the unique physical volume associated with a given logical volume.
/// Returns "" if not found
TString TRestGeant4Geometry::GetUniquePhysicalFromLogical(const TString& inputLogicalVolume) const {
    if (IsLogicalVolume(inputLogicalVolume)) {
        auto physicalVolumes = fLogicalToPhysicalMap.at(inputLogicalVolume);
        if (physicalVolumes.size() == 1) {
            return physicalVolumes[0];
        }
    }
    return "";
}

vector<TString> TRestGeant4Geometry::GetAllPhysicalFromLogical(const TString& inputLogicalVolume) const {
    return fLogicalToPhysicalMap.at(inputLogicalVolume);
}

TString TRestGeant4Geometry::GetLogicalFromPhysical(const TString& inputPhysicalVolume) const {
    return fPhysicalToLogicalVolumeMap.at(inputPhysicalVolume);
}

TString TRestGeant4Geometry::GetMaterialFromLogical(const TString& inputLogicalVolume) const {
    return fLogicalToMaterialMap.at(inputLogicalVolume);
}

TString TRestGeant4Geometry::GetMaterialFromPhysical(const TString& inputPhysicalVolume) const {
    return fLogicalToMaterialMap.at(GetLogicalFromPhysical(inputPhysicalVolume));
}

TString TRestGeant4Geometry::GetPhysicalFromGeant4Physical(const TString& inputGeant4PhysicalVolume) const {
    if (fGeant4PhysicalToPhysicalMap.empty()) {
        return inputGeant4PhysicalVolume;
    }
    return fGeant4PhysicalToPhysicalMap.at(inputGeant4PhysicalVolume);
}

size_t TRestGeant4Geometry::GetActiveVolumeIndex(const TString& inputActiveVolume) const {
    if (!IsActiveVolume(inputActiveVolume)) {
        cerr << "TRestGeant4Geometry::GetActiveVolumeIndex - " << inputActiveVolume
             << " is not a valid active volume.";
        exit(1);
    }
    for (int i = 0; i < fActiveVolumes.size(); i++) {
        if (inputActiveVolume.EqualTo(fActiveVolumes[i])) {
            return i;
        }
    }
    return -1;
}

Double_t TRestGeant4Geometry::GetStorageChance(const TString& inputPhysicalVolume) const {
    return fPhysicalToStorageChanceMap.at(inputPhysicalVolume);
}

Double_t TRestGeant4Geometry::GetMaxStepSize(const TString& inputPhysicalVolume) const {
    return fPhysicalToMaxStepSizeMap.at(inputPhysicalVolume);
}

TString TRestGeant4Geometry::GetActiveVolumeFromIndex(size_t index) const { return fActiveVolumes[index]; }

void TRestGeant4Geometry::InsertActiveVolume(const TString& name, Double_t chance, Double_t maxStepSize) {
    if (!IsActiveVolume(name)) {
        fActiveVolumes.emplace_back(name);
    }
    fPhysicalToStorageChanceMap[name] = chance;
    fPhysicalToMaxStepSizeMap[name] = maxStepSize;
}

void TRestGeant4Geometry::InsertSensitiveVolume(const TString& name) { fSensitiveVolumes.emplace_back(name); }

TRestGeant4Geometry::TRestGeant4Geometry() : fInitialized(false) {}

void TRestGeant4Geometry::LoadGdml(const TString& gdml) {
    fGdmlAbsolutePath = gdml;
    ifstream f(fGdmlAbsolutePath);
    if (f) {
        ostringstream ss;
        ss << f.rdbuf();  // reading data
        fGdmlContents = ss.str();
    }
}

void TRestGeant4Geometry::PrintGeometryInfo() const {
    metadata << "TRestGeant4Geometry::PrintGeometryInfo ---> Total number of physical volumes: "
             << fPhysicalVolumes.size() << endl;
    for (const auto& volume : fPhysicalVolumes) {
        auto logicalVolumeName = fPhysicalToLogicalVolumeMap.at(volume);
        auto materialName = fLogicalToMaterialMap.at(logicalVolumeName);
        metadata << "---> ---> physical volume: " << volume << " - logical: " << logicalVolumeName
                 << " - material: " << materialName << endl;
    }
}
