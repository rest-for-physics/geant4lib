//
// Created by lobis on 3/5/2022.
//

#include "TRestGeant4GeometryInfo.h"

#include <TPRegexp.h>
#include <TXMLEngine.h>

#include <iostream>

#include "TRestStringHelper.h"

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
XMLNodePointer_t GetChildByAttributeValue(TXMLEngine xml, XMLNodePointer_t node,
                                         const TString& attributeName, const TString& attributeValue) {
    XMLNodePointer_t child = xml.GetChild(node);
    while (child) {
        TString childAttributeValue = GetNodeAttribute(xml, child, attributeName);
        if (childAttributeValue.EqualTo(attributeValue)) {
            return child;
        }
        child = xml.GetNext(child);
    }
    return nullptr;
}
void AddVolumesRecursively(vector<TString>* physicalNames, vector<TString>* logicalNames,
                           const vector<TString>& children, map<TString, TString>& nameTable,
                           map<TString, vector<TString>>& childrenTable, const TString& name = "",
                           const TString& separator = "_") {
    if (children.empty()) {
        physicalNames->push_back(name);
        const auto logicalVolumeName = nameTable[name];
        logicalNames->push_back(logicalVolumeName);
        return;
    }
    for (const auto& childName : children) {
        const auto newName = name + separator + childName;
        nameTable[newName] = nameTable[childName];  // needed to retrieve logical volume name
        AddVolumesRecursively(physicalNames, logicalNames, childrenTable[nameTable[childName]], nameTable,
                              childrenTable, newName, separator);
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

    // auxiliar maps
    map<TString, TString> gdmlToGeant4AssemblyNameMap;  // to record the ordering of the assemblies
                                                        // definition, e.g. "assemblyName" -> "av_1"
    size_t assemblyCounter = 0;
    map<TString, size_t> gdmlAssemblyNameToImprintCounterMap;  // to track the number of imprints per assembly
    /* When a PV is placed from an assembly, its daughter physical volumes are imprinted into the mother
    volume where you are placing the assembly. This daughter PV are named following the format:
    "av_WWW_impr_XXX_YYY_ZZZ". But, if one of those daughter PV is itself an assembly, its own daughter PV are
    named (wrongly in my opinion) "av_WWW_impr_XXX+1_yyy_zzz". To follow this logic, we need to add this
    "av_WWW_impr_XXX+1"-> nestedAssemblyPVname to the fGeant4AssemblyImprintToGdmlNameMap too. To do this we
    use the auxiliar map 'fixNestedAssembliesMap' to store which nested assemblies where found inside each
    assembly and their order.

    Also, the daughter physical volumes defined after this nested assembly
    gets the imprint number XXX+1 instead of the XXX of its mother assembly imprint (which I think is a bug in
    Geant4). To avoid having several "av_WWW_impr_XXX+n" pointing to different GDML names, you must define all
    the daughter PV from normal LV before all the nested assembly PV.
    */
    map<TString, std::vector<TString>>
        fixNestedAssembliesMap;  // parent assembly -> child PV of assemblies. Used to add later the bad
                                 // imprints "av_WWW_impr_XXX+1"

    map<TString, TString> nameTable;
    map<TString, vector<TString>> childrenTable;
    XMLNodePointer_t mainNode = xml.DocGetRootElement(xmldoc);
    XMLNodePointer_t structure = myXml::FindChildByName(xml, mainNode, "structure");
    XMLNodePointer_t child = xml.GetChild(structure);
    RESTDebug << "Searching for assemblies..." << RESTendl;
    while (child) {  // loop over the direct children of structure (logical volumes and assemblies)
        TString name = xml.GetNodeName(child);  // name of the type of children: "volume" or "assembly"
        if (!name.EqualTo("assembly")) {
            child = xml.GetNext(child);
            continue;
        }
        TString assemblyName = myXml::GetNodeAttribute(xml, child, "name");
        gdmlToGeant4AssemblyNameMap[assemblyName] =
            "av_" + to_string(++assemblyCounter);  // first assembly is av_1
        gdmlAssemblyNameToImprintCounterMap[assemblyName] = 0;
        RESTDebug << "Assembly found: " << assemblyName << " → " << gdmlToGeant4AssemblyNameMap[assemblyName]
                  << RESTendl;

        // blablabla
        bool hasNestedAssemblies = false;
        std::map<TString, TString> childrenGeant4toGdmlMap;
        size_t childrenPVCounter = 0;
        auto physicalVolumeNode =
            xml.GetChild(child);  // children of a volume or assembly are physical volumes
        while (physicalVolumeNode) {
            auto physicalVolumeName = myXml::GetNodeAttribute(xml, physicalVolumeNode, "name");
            auto volumeRefNode =
                xml.GetChild(physicalVolumeNode);  // this are volumeref, position and rotation
            while (volumeRefNode) {
                TString volumeRefNodeName =
                    xml.GetNodeName(volumeRefNode);  // "volumeref", "position" or "rotation"
                if (volumeRefNodeName.EqualTo("volumeref")) {
                    TString refName =
                        myXml::GetNodeAttribute(xml, volumeRefNode, "ref");  // the logical volume name
                    TString geant4Name =
                        refName + "_pv_" +
                        to_string(childrenPVCounter++);  // first pv is logicalVolumeName_pv_0
                    childrenGeant4toGdmlMap[geant4Name] = physicalVolumeName;
                    if (gdmlToGeant4AssemblyNameMap.count(refName) > 0) {
                        // it's an assembly
                        hasNestedAssemblies =
                            true;  // to warn later that pv from assemblies must be defined last
                        fixNestedAssembliesMap[assemblyName].push_back(physicalVolumeName);
                    } else {
                        if (hasNestedAssemblies) {
                            RESTError << "TRestGeant4GeometryInfo::PopulateFromGdml - Assembly '"
                                      << assemblyName << "' contains physical volumes from normal "
                                      << "(i.e. non-assembly) logical volumes defined after physical volumes "
                                      << "from assemblies. Due to a Geant4 bug you cannot do this. "
                                      << "Please define the physical volumes from the assemblies last."
                                      << RESTendl;
                        }
                    }
                }
                volumeRefNode = xml.GetNext(volumeRefNode);
            }
            physicalVolumeNode = xml.GetNext(physicalVolumeNode);
        }
        fGdmlAssemblyTChildrenGeant4ToGdmlPhysicalNameMap[assemblyName] = childrenGeant4toGdmlMap;
        child = xml.GetNext(child);
    }

    child = xml.GetChild(structure);
    while (child) {  // loop over the direct children of structure (logical volumes and assemblies)
        TString name = xml.GetNodeName(child);  // name of the type of children: "volume" or "assembly"
        TString volumeName = myXml::GetNodeAttribute(xml, child, "name");
        auto physicalVolumeNode =
            xml.GetChild(child);  // children of a volume or assembly are physical volumes
        childrenTable[volumeName] = {};
        while (physicalVolumeNode) {
            auto physicalVolumeName = myXml::GetNodeAttribute(xml, physicalVolumeNode, "name");
            auto volumeRefNode =
                xml.GetChild(physicalVolumeNode);  // this are volumeref, position and rotation
            while (volumeRefNode) {
                TString volumeRefNodeName =
                    xml.GetNodeName(volumeRefNode);  // "volumeref", "position" or "rotation"
                if (volumeRefNodeName.EqualTo("volumeref")) {
                    TString refName =
                        myXml::GetNodeAttribute(xml, volumeRefNode, "ref");  // the logical volume name
                    if (gdmlToGeant4AssemblyNameMap.count(refName) > 0) {
                        // it's an assembly
                        fGeant4AssemblyImprintToAssemblyLogicalNameMap[physicalVolumeName] = refName;
                        size_t imprintCounter =
                            ++gdmlAssemblyNameToImprintCounterMap[refName];  // first imprint is impr_1
                        TString imprint =
                            gdmlToGeant4AssemblyNameMap[refName] + "_impr_" + to_string(imprintCounter);
                        fGeant4AssemblyImprintToGdmlNameMap[imprint] = physicalVolumeName;

                        if (fixNestedAssembliesMap.count(refName) > 0) {
                            // fix nested assemblies
                            for (const auto& childPV : fixNestedAssembliesMap[refName]) {
                                size_t nestedImprintCounter = ++gdmlAssemblyNameToImprintCounterMap[refName];
                                TString nestedImprint = gdmlToGeant4AssemblyNameMap[refName] + "_impr_" +
                                                        to_string(nestedImprintCounter);
                                fGeant4AssemblyImprintToGdmlNameMap[nestedImprint] = childPV;
                            }
                        }
                    }
                    nameTable[physicalVolumeName] = refName;
                    childrenTable[volumeName].push_back(physicalVolumeName);
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
                                     childrenTable, topName, fPathSeparator);
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

    /*
    //print fGeant4AssemblyImprintToGdmlNameMap
    cout << "GDML Assembly Imprint to GDML Name Map:" << endl;
    for (const auto& kv : fGeant4AssemblyImprintToGdmlNameMap) {
        cout << "\t" << kv.first << " → " << kv.second << endl;
    }

    cout << "GDML assembly children Geant4 to GDML Physical Name Map:" << endl;
    for (const auto& kv : fGdmlAssemblyTChildrenGeant4ToGdmlPhysicalNameMap) {
        cout << "\tAssembly: " << kv.first << endl;
        for (const auto& childKv : kv.second) {
            cout << "\t\t" << childKv.first << " → " << childKv.second << endl;
        }
    }

    cout << "Assembly imprint to Assembly Logical Name Map:" << endl;
    for (const auto& kv : fGeant4AssemblyImprintToAssemblyLogicalNameMap) {
        cout << "\t" << kv.first << " → " << kv.second << endl;
    }
    */
}

//////////////////////////////////////////////////////////////////////////
/// \brief Converts a Geant4 volume path to a GDML volume path. The path is
/// the concatenation of the names of the nested volumes from the world to the desired volume.
/// This method is meant mainly meant to handle the conversion of assembly imprints names to the
/// GDML assembly names used in the GDML file.
/// \param geant4Path The Geant4 volume path.
/// \return The corresponding GDML volume path using the PV names defined in the GDML file.
TString TRestGeant4GeometryInfo::GetAlternativePathFromGeant4Path(const TString& geant4Path) const {
    std::string convertedPath = geant4Path.Data();
    // convert the Geant4 assembly imprint convention to GDML name
    // e.g. av_1_impr_2_childLV_pv_1 → assemblyName/childLV_pv_1
    for (const auto& [gvName, gdmlName] : fGeant4AssemblyImprintToGdmlNameMap) {
        convertedPath = Replace(convertedPath, (gvName + "_").Data(), (gdmlName + fPathSeparator).Data());
    }
    // convert the children names inside assemblies to physical volume names in GDML
    // e.g. assemblyName/childLV_pv_1 → assemblyName/childPVname
    for (const auto& [gvImprint, assemblyLogicalName] : fGeant4AssemblyImprintToAssemblyLogicalNameMap) {
        auto gvImprintPosition = convertedPath.find(gvImprint.Data());
        if (gvImprintPosition != std::string::npos) {
            for (const auto& [childGeant4Name, childGdmlName] :
                 fGdmlAssemblyTChildrenGeant4ToGdmlPhysicalNameMap.at(assemblyLogicalName)) {
                TString toReplace = gvImprint + fPathSeparator + childGeant4Name;
                TString replaceBy = gvImprint + fPathSeparator + childGdmlName;
                convertedPath = Replace(convertedPath, toReplace.Data(), replaceBy.Data());
            }
        }
    }
    return TString(convertedPath);
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
