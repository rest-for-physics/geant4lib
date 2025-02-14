//
// Created by lobis on 3/5/2022.
//

#include "TRestGeant4GeometryInfo.h"

#include <TPRegexp.h>
#include <TXMLEngine.h>

#include <iostream>

using namespace std;

namespace myXml {
XMLNodePointer_t FindChildByName(TXMLEngine xml, XMLNodePointer_t node, const TString& name) {
    XMLNodePointer_t child = xml.GetChild(node);
    while (child) {
        TString childName = xml.GetNodeName(child);
        if (childName.EqualTo(name)) {
            return child;
        }
        child = xml.GetNext(child);
    }
    return nullptr;
}
TString GetNodeAttribute(TXMLEngine xml, XMLNodePointer_t node, const TString& attributeName) {
    XMLAttrPointer_t attr = xml.GetFirstAttr(node);
    while (attr) {
        if (TString(xml.GetAttrName(attr)).EqualTo(attributeName)) {
            TString refName = xml.GetAttrValue(attr);
            return refName;
        }
        attr = xml.GetNextAttr(attr);
    }
    return {};
}
void AddVolumesRecursively(vector<TString>* physicalNames, vector<TString>* logicalNames,
                           const vector<TString>& children, map<TString, TString>& nameTable,
                           map<TString, vector<TString>>& childrenTable, const TString& name = "") {
    if (children.empty()) {
        physicalNames->push_back(name);
        const auto logicalVolumeName = nameTable[name];
        logicalNames->push_back(logicalVolumeName);
        return;
    }
    for (const auto& childName : children) {
        const auto newName = name + "_" + childName;
        nameTable[newName] = nameTable[childName];  // needed to retrieve logical volume name
        AddVolumesRecursively(physicalNames, logicalNames, childrenTable[nameTable[childName]], nameTable,
                              childrenTable, newName);
    }
}
}  // namespace myXml

void TRestGeant4GeometryInfo::PopulateFromGdml(const TString& gdmlFilename) {
    /*
     * Fills 'fGdmlNewPhysicalNames' with physical volume names generated from GDML
     */
    cout << "TRestGeant4GeometryInfo::PopulateFromGdml - " << gdmlFilename << endl;
    // Geometry must be in GDML
    TXMLEngine xml;
    XMLDocPointer_t xmldoc = xml.ParseFile(gdmlFilename.Data());
    if (!xmldoc) {
        cout << "TRestGeant4GeometryInfo::PopulateFromGdml - GDML file " << gdmlFilename.Data()
             << " not found" << endl;
        exit(1);
    }
    map<TString, TString> nameTable;
    map<TString, vector<TString>> childrenTable;
    XMLNodePointer_t mainNode = xml.DocGetRootElement(xmldoc);
    XMLNodePointer_t structure = myXml::FindChildByName(xml, mainNode, "structure");
    XMLNodePointer_t child = xml.GetChild(structure);
    while (child) {
        TString name = xml.GetNodeName(child);
        TString volumeName = myXml::GetNodeAttribute(xml, child, "name");
        auto physicalVolumeNode = xml.GetChild(child);
        childrenTable[volumeName] = {};
        while (physicalVolumeNode) {
            auto physicalVolumeName = myXml::GetNodeAttribute(xml, physicalVolumeNode, "name");
            auto volumeRefNode = xml.GetChild(physicalVolumeNode);
            while (volumeRefNode) {
                TString volumeRefNodeName = xml.GetNodeName(volumeRefNode);
                if (volumeRefNodeName.EqualTo("volumeref")) {
                    TString refName = myXml::GetNodeAttribute(xml, volumeRefNode, "ref");
                    nameTable[physicalVolumeName] = refName;
                    childrenTable[volumeName].push_back(physicalVolumeName);
                }
                if (volumeRefNodeName.EqualTo("file")){
                    TString fullFileName = myXml::GetNodeAttribute(xml, volumeRefNode, "name");
                    TString fileName = TRestTools::GetFileNameRoot(fullFileName.Data());
                    TString refName = myXml::GetNodeAttribute(xml, volumeRefNode, "ref");
                    if (physicalVolumeName.IsNull()){
                        physicalVolumeName = fileName + "_PV";
                    }
                    if (refName.IsNull()){
                        refName = fileName;
                    }
                    nameTable[physicalVolumeName] = refName; // ?????????
                    childrenTable[volumeName].push_back(physicalVolumeName); // ????????????
                }
                volumeRefNode = xml.GetNext(volumeRefNode);
            }
            physicalVolumeNode = xml.GetNext(physicalVolumeNode);
        }
        child = xml.GetNext(child);
    }

    string worldVolumeName = "world";
    if (childrenTable.count(worldVolumeName) == 0) {
        worldVolumeName = "World";
        if (childrenTable.count(worldVolumeName) == 0) {
            cout << "Could not find world volume in GDML, please name it either 'World' or 'world'";
            exit(1);
        }
    }

    fGdmlNewPhysicalNames.clear();
    fGdmlLogicalNames.clear();
    for (const auto& topName : childrenTable[worldVolumeName]) {
        auto children = childrenTable[nameTable[topName]];
        myXml::AddVolumesRecursively(&fGdmlNewPhysicalNames, &fGdmlLogicalNames, children, nameTable,
                                     childrenTable, topName);
    }

    xml.FreeDoc(xmldoc);

    // Checks
    if (fGdmlNewPhysicalNames.empty()) {
        cout << "TRestGeant4GeometryInfo::PopulateFromGdml - ERROR - No physical volumes have been added!"
             << endl;
        exit(1);
    }
    // Find duplicates
    set<TString> s;
    for (const auto& name : fGdmlNewPhysicalNames) {
        s.insert(name);
    }
    if (s.size() != fGdmlNewPhysicalNames.size()) {
        cout << "TRestGeant4GeometryInfo::PopulateFromGdml - ERROR - Generated a duplicate name, please "
                "check assembly"
             << endl;
        exit(1);
    }
}

TString TRestGeant4GeometryInfo::GetAlternativeNameFromGeant4PhysicalName(
    const TString& geant4PhysicalName) const {
    if (fGeant4PhysicalNameToNewPhysicalNameMap.count(geant4PhysicalName) > 0) {
        return fGeant4PhysicalNameToNewPhysicalNameMap.at(geant4PhysicalName);
    }
    return geant4PhysicalName;
}

TString TRestGeant4GeometryInfo::GetGeant4PhysicalNameFromAlternativeName(
    const TString& alternativeName) const {
    for (const auto& kv : fGeant4PhysicalNameToNewPhysicalNameMap) {
        if (kv.second == alternativeName) {
            return kv.first;
        }
    }
    return "";
}

template <typename T, typename U>
U GetOrDefaultMapValueFromKey(const map<T, U>* pMap, const T& key) {
    if (pMap->count(key) > 0) {
        return pMap->at(key);
    }
    return {};
}

TString TRestGeant4GeometryInfo::GetVolumeFromID(Int_t id) const {
    return GetOrDefaultMapValueFromKey<Int_t, TString>(&fVolumeNameMap, id);
}

Int_t TRestGeant4GeometryInfo::GetIDFromVolume(const TString& volumeName) const {
    if (fVolumeNameReverseMap.count(volumeName) == 0) {
        // if we do not find the volume we return -1 instead of default (which is 0 and may be confusing)
        cout << "TRestGeant4GeometryInfo::GetIDFromVolume - volume '" << volumeName << "' not found in store!"
             << endl;
        return -1;
    }
    return GetOrDefaultMapValueFromKey<TString, Int_t>(&fVolumeNameReverseMap, volumeName);
}

void TRestGeant4GeometryInfo::InsertVolumeName(Int_t id, const TString& volumeName) {
    fVolumeNameMap[id] = volumeName;
    fVolumeNameReverseMap[volumeName] = id;
}

void TRestGeant4GeometryInfo::Print() const {
    cout << "Assembly Geometry: " << (fIsAssembly ? "yes" : "no") << endl;

    const auto physicalVolumes = GetAllPhysicalVolumes();
    cout << "Physical volumes (" << physicalVolumes.size() << "):" << endl;
    for (const auto& physical : GetAllPhysicalVolumes()) {
        auto geant4Name = GetGeant4PhysicalNameFromAlternativeName(physical);
        const auto& logical = fPhysicalToLogicalVolumeMap.at(physical);
        const auto& position = GetPosition(physical);
        cout << "\t- " << (geant4Name == physical ? physical : physical + " (" + geant4Name + ")")
             << " - ID: " << GetIDFromVolume(physical)
             << " - Logical: " << fPhysicalToLogicalVolumeMap.at(physical)
             << " - Material: " << fLogicalToMaterialMap.at(logical)                                        //
             << " - Position: (" << position.X() << ", " << position.Y() << ", " << position.Z() << ") mm"  //
             << endl;
    }

    const auto logicalVolumes = GetAllLogicalVolumes();
    cout << "Logical volumes (" << logicalVolumes.size() << "):" << endl;
    for (const auto& logical : logicalVolumes) {
        cout << "\t- " << logical << endl;
    }
}

std::vector<TString> TRestGeant4GeometryInfo::GetAllLogicalVolumes() const {
    auto volumes = std::vector<TString>();

    for (const auto& kv : fLogicalToPhysicalMap) {
        volumes.emplace_back(kv.first);
    }

    return volumes;
}

std::vector<TString> TRestGeant4GeometryInfo::GetAllPhysicalVolumes() const {
    auto volumes = std::vector<TString>();

    for (const auto& kv : fPhysicalToLogicalVolumeMap) {
        volumes.emplace_back(kv.first);
    }

    return volumes;
}

std::vector<TString> TRestGeant4GeometryInfo::GetAllAlternativePhysicalVolumes() const {
    auto volumes = std::vector<TString>();

    for (const auto& kv : fPhysicalToLogicalVolumeMap) {
        volumes.emplace_back(GetAlternativeNameFromGeant4PhysicalName(kv.first));
    }

    return volumes;
}

std::vector<TString> TRestGeant4GeometryInfo::GetAllPhysicalVolumesMatchingExpression(
    const TString& regularExpression) const {
    auto volumes = std::vector<TString>();

    TPRegexp regex(regularExpression);

    for (const auto& volume : GetAllPhysicalVolumes()) {
        if (regex.Match(volume)) {
            volumes.emplace_back(volume);
        }
    }

    return volumes;
}

std::vector<TString> TRestGeant4GeometryInfo::GetAllLogicalVolumesMatchingExpression(
    const TString& regularExpression) const {
    auto volumes = std::vector<TString>();

    TPRegexp regex(regularExpression);

    for (const auto& volume : GetAllLogicalVolumes()) {
        if (regex.Match(volume)) {
            volumes.emplace_back(volume);
        }
    }

    return volumes;
}
