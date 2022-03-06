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
void AddVolumesRecursively(vector<TString>* container, const vector<TString>& children,
                           map<TString, TString>& nameTable, map<TString, vector<TString>>& childrenTable,
                           const TString& name = "") {
    // G4cout << "called AddVolumesRecursively with name: " << name << endl;
    for (const auto& child : children) {
        // G4cout << "\t" << child << endl;
    }
    if (children.empty()) {
        container->push_back(name);
        // G4cout << "ADDED: " << name << endl;
        return;
    }
    for (const auto& childName : children) {
        AddVolumesRecursively(container, childrenTable[nameTable[childName]], nameTable, childrenTable,
                              name + "_" + childName);
    }
}
}  // namespace myXml

void TRestGeant4GeometryInfo::PopulateFromGdml(const TString& gdmlFilename) {
    cout << "TRestGeant4GeometryInfo::PopulateFromGdml" << endl;
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
                volumeRefNode = xml.GetNext(volumeRefNode);
            }
            physicalVolumeNode = xml.GetNext(physicalVolumeNode);
        }
        child = xml.GetNext(child);
    }

    fGdmlNewPhysicalNames.clear();
    for (const auto& topName : childrenTable["world"]) {
        auto children = childrenTable[nameTable[topName]];
        myXml::AddVolumesRecursively(&fGdmlNewPhysicalNames, children, nameTable, childrenTable, topName);
    }

    xml.FreeDoc(xmldoc);

    // Find duplicates
    size_t n = fGdmlNewPhysicalNames.size();
    set<TString> s;
    for (const auto& name : fGdmlNewPhysicalNames) {
        s.insert(name);
    }
    if (s.size() != n) {
        cout
            << "TRestGeant4GeometryInfo::PopulateFromGdml - Generated a duplicate name, please check assembly"
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

Int_t TRestGeant4GeometryInfo::GetIDFromVolumeName(const TString& volumeName) const {
    for (int i = 0; i < fGdmlNewPhysicalNames.size(); i++) {
        if (volumeName.EqualTo(fGdmlNewPhysicalNames[i])) {
            return i;
        }
    }

    int i = 0;
    for (const auto& physical : GetAllPhysicalVolumes()) {
        if (volumeName.EqualTo(physical)) {
            return i;
        }
        i++;
    }

    cout << "TRestGeant4GeometryInfo::GetIDFromPhysicalName - ID not found for " << volumeName << endl;
    exit(1);
}

void TRestGeant4GeometryInfo::Print() const {
    cout << "TRestGeant4GeometryInfo::Print" << endl;

    cout << "Assembly Geometry: " << (fIsAssembly ? "yes" : "no") << endl;

    cout << "Physical volumes:" << endl;
    for (const auto& physical : GetAllPhysicalVolumes()) {
        auto newName = GetAlternativeNameFromGeant4PhysicalName(physical);
        cout << "\t- " << (newName == physical ? physical : newName + " (" + physical + ")") << endl;
    }

    cout << "Logical volumes:" << endl;
    for (const auto& logical : GetAllLogicalVolumes()) {
        cout << "\t- " << logical << endl;
    }
}

std::vector<TString> TRestGeant4GeometryInfo::GetAllPhysicalVolumes() const {
    auto volumes = std::vector<TString>();

    for (const auto& kv : fPhysicalToLogicalVolumeMap) {
        volumes.emplace_back(kv.first);
    }

    return volumes;
}

std::vector<TString> TRestGeant4GeometryInfo::GetAllLogicalVolumes() const {
    auto volumes = std::vector<TString>();

    for (const auto& kv : fLogicalToPhysicalMap) {
        volumes.emplace_back(kv.first);
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
