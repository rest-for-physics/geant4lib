
#ifndef REST_TRESTGEANT4GEOMETRYINFO_H
#define REST_TRESTGEANT4GEOMETRYINFO_H

#include <TString.h>
#include <TVector3.h>

#include <map>
#include <set>
#include <vector>

class G4VPhysicalVolume;

class TRestGeant4GeometryInfo {
ClassDef(TRestGeant4GeometryInfo, 3);

private:
    bool fIsAssembly = false;

    std::map<Int_t, TString> fVolumeNameMap = {};
    std::map<TString, Int_t> fVolumeNameReverseMap = {};

    void PopulateFromGeant4World(const G4VPhysicalVolume *);

    inline void InitializeOnDetectorConstruction(const TString &gdmlFilename,
                                                 const G4VPhysicalVolume *world) {
        PopulateFromGdml(gdmlFilename);
        PopulateFromGeant4World(world);
    }

public:
    // Insertion order is important for GDML containers. These containers are filled from GDML only not Geant4
    std::vector<TString> fGdmlNewPhysicalNames;
    std::vector<TString> fGdmlLogicalNames;

    std::map<TString, TString>
            fGeant4PhysicalNameToNewPhysicalNameMap; /*
                                                  * only makes sense when using assembly
                                                  */

    std::map<TString, TString> fPhysicalToLogicalVolumeMap;
    std::map<TString, std::vector<TString> > fLogicalToPhysicalMap;
    // many physical volumes can point to one single logical
    std::map<TString, TString> fLogicalToMaterialMap;
    std::map<TString, TVector3> fPhysicalToPositionInWorldMap;

public:
    inline TRestGeant4GeometryInfo() = default;

    void PopulateFromGdml(const TString &);

    TString GetAlternativeNameFromGeant4PhysicalName(const TString &) const;

    TString GetGeant4PhysicalNameFromAlternativeName(const TString &) const;

    std::vector<TString> GetAllLogicalVolumes() const;

    std::vector<TString> GetAllPhysicalVolumes() const;

    std::vector<TString> GetAllAlternativePhysicalVolumes() const;

    std::vector<TString> GetAllLogicalVolumesMatchingExpression(const TString &) const;

    std::vector<TString> GetAllPhysicalVolumesMatchingExpression(const TString &) const;

    inline bool IsValidGdmlName(const TString &volume) const {
        for (const auto &name: fGdmlNewPhysicalNames) {
            if (name == volume) {
                return true;
            }
        }
        return false;
    }

    inline bool IsValidPhysicalVolume(const TString &volume) const {
        return fPhysicalToLogicalVolumeMap.count(volume) > 0;
    }

    inline bool IsValidLogicalVolume(const TString &volume) const {
        return fLogicalToPhysicalMap.count(volume) > 0;
    }

    inline std::vector<TString> GetAllPhysicalVolumesFromLogical(const TString &logicalVolume) const {
        if (IsValidLogicalVolume(logicalVolume)) {
            return fLogicalToPhysicalMap.at(logicalVolume);
        }
        return {};
    }

    inline TString GetLogicalVolumeFromPhysical(const TString &physicalVolume) const {
        if (IsValidPhysicalVolume(physicalVolume)) {
            return fPhysicalToLogicalVolumeMap.at(physicalVolume);
        }
        return {};
    }

    inline TVector3 GetPosition(const TString &volume) const {
        return fPhysicalToPositionInWorldMap.at(volume);
    }

    inline bool IsAssembly() const { return fIsAssembly; }

    void InsertVolumeName(Int_t id, const TString &volumeName);

    TString GetVolumeFromID(Int_t id) const;

    Int_t GetIDFromVolume(const TString &volumeName) const;

    void Print() const;

    friend class DetectorConstruction;
};

#endif  // REST_TRESTGEANT4GEOMETRYINFO_H
