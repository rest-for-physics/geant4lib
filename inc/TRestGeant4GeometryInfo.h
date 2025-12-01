
#ifndef REST_TRESTGEANT4GEOMETRYINFO_H
#define REST_TRESTGEANT4GEOMETRYINFO_H

#include <TRestStringOutput.h>
#include <TString.h>
#include <TVector3.h>

#include <map>
#include <set>
#include <vector>

class G4VPhysicalVolume;

class TRestGeant4GeometryInfo {
    ClassDef(TRestGeant4GeometryInfo, 4);

   private:
    bool fIsAssembly = false;
    TString fPathSeparator = "_";

    std::map<Int_t, TString> fVolumeNameMap = {};
    std::map<TString, Int_t> fVolumeNameReverseMap = {};

    void PopulateFromGeant4World(const G4VPhysicalVolume*);

    inline void InitializeOnDetectorConstruction(const TString& gdmlFilename,
                                                 const G4VPhysicalVolume* world) {
        PopulateFromGdml(gdmlFilename);
        PopulateFromGeant4World(world);
    }

   public:
    /// Physical volume names as in the GDML file
    std::vector<TString> fGdmlNewPhysicalNames;

    /// Logical volume names as in the GDML file
    std::vector<TString> fGdmlLogicalNames;

    /// Map from new physical volume names (unique) to Geant4 physical volume names (not always unique)
    std::map<TString, TString>
        fNewPhysicalToGeant4PhysicalNameMap;  // reverse map of old fGeant4PhysicalNameToNewPhysicalNameMap

    /// Map of Geant4 prefix for the assembly imprint to the GDML physical volume name of that assembly
    /// imprint. For example: {"av_1_impr_1" -> "micromegasRight" , "av_1_impr_2" -> "micromegasleft"}
    std::map<TString, TString> fGeant4AssemblyImprintToGdmlNameMap;

    /// Map of each assembly name to a map of Geant4 naming of the assembly daughters to their GDML physical
    /// volume names. For example: {"micromegas_assembly" -> { "mMBaseLV_pv_0" -> "mMBase",
    /// "mMBaseClosingBracketLV_pv_1" -> "mMBaseClosingBracket1"}}
    std::map<TString, std::map<TString, TString>> fGdmlAssemblyToChildrenGeant4ToGdmlPhysicalNameMap;

    /// Map of GDML physical volume name of an assembly to its assembly name.
    /// For example: {"micromegasRight" -> "micromegas_assembly"}
    std::map<TString, TString> fGeant4AssemblyImprintToAssemblyLogicalNameMap;

    /// Map of GDML physical volume name to its logical volume name
    std::map<TString, TString> fPhysicalToLogicalVolumeMap;

    /// Map of logical volume name to their Geant4 physical volume names.
    /// Note that many physical volumes can share the same logical volume.
    std::map<TString, std::vector<TString>> fLogicalToPhysicalMap;

    /// Map of logical volume name to its material name
    std::map<TString, TString> fLogicalToMaterialMap;

    /// Map of GDML physical volume name to its position in world coordinates
    std::map<TString, TVector3> fPhysicalToPositionInWorldMap;

   public:
    inline TRestGeant4GeometryInfo() = default;

    void PopulateFromGdml(const TString&);

    TString GetAlternativePathFromGeant4Path(const TString&) const;
    TString GetAlternativeNameFromGeant4PhysicalName(const TString&) const;
    std::set<TString> GetAlternativeNamesFromGeant4PhysicalName(const TString&) const;
    TString GetGeant4PhysicalNameFromAlternativeName(const TString&) const;

    std::vector<TString> GetAllLogicalVolumes() const;
    std::vector<TString> GetAllPhysicalVolumes() const;
    std::vector<TString> GetAllAlternativePhysicalVolumes() const;

    std::vector<TString> GetAllLogicalVolumesMatchingExpression(const TString&) const;
    std::vector<TString> GetAllPhysicalVolumesMatchingExpression(const TString&) const;

    inline bool IsValidGdmlName(const TString& volume) const {
        for (const auto& name : fGdmlNewPhysicalNames) {
            if (name == volume) {
                return true;
            }
        }
        return false;
    }

    /// \brief Checks if a (Geant4) physical volume name exists in the geometry.
    inline bool IsValidGeant4PhysicalVolume(const TString& volume) const {
        return fPhysicalToLogicalVolumeMap.count(volume) > 0;
    }

    /// \brief Checks if a (GDML) physical volume name exists in the geometry.
    inline bool IsValidPhysicalVolume(const TString& volume) const {
        return fNewPhysicalToGeant4PhysicalNameMap.count(volume) > 0;
    }

    /// \brief Checks if a logical volume name exists in the geometry.
    inline bool IsValidLogicalVolume(const TString& volume) const {
        return fLogicalToPhysicalMap.count(volume) > 0;
    }

    /// \brief Gets all the (Geant4) physical volume names corresponding to a given logical volume name.
    inline std::vector<TString> GetAllPhysicalVolumesFromLogical(const TString& logicalVolume) const {
        if (IsValidLogicalVolume(logicalVolume)) {
            return fLogicalToPhysicalMap.at(logicalVolume);
        }
        return {};
    }

    /// \brief Gets the position in world coordinates of a given physical volume.
    inline TVector3 GetPosition(const TString& volume) const {
        return fPhysicalToPositionInWorldMap.at(volume);
    }

    inline bool IsAssembly() const { return fIsAssembly; }
    inline TString GetPathSeparator() const { return fPathSeparator; }
    void SetPathSeparator(const TString& separator) { fPathSeparator = separator; }
    void InsertVolumeName(Int_t id, const TString& volumeName);

    TString GetVolumeFromID(Int_t id) const;
    Int_t GetIDFromVolume(const TString& volumeName) const;

    void Print(bool multiLine = false) const;

    friend class DetectorConstruction;
};

#endif  // REST_TRESTGEANT4GEOMETRYINFO_H
