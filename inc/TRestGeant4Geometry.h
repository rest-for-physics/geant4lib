/*
 * Created by @lobis on 15/11/2021
 *
 * Class used to store geometry information. A single instance of this class will be in TRestGeant4Metadata.
 * Having a separate class is due to the current size of TRestGeant4Metadata, this will help readability.
 */

#ifndef REST_TRESTGEANT4GEOMETRY_H
#define REST_TRESTGEANT4GEOMETRY_H

#include <TRestMetadata.h>

class G4VPhysicalVolume;

class TRestGeant4Geometry : public TRestMetadata {
   private:
    Bool_t fInitialized;  // flag to see if class has been initialized from Geant4
    Bool_t fAllVolumesActive = false;

    TString fGdmlAbsolutePath;
    TString fGdmlContents;

    std::vector<TString> fPhysicalVolumes; /*
                                            * Sorted list of physical volumes as they appear in the GDML!
                                            * It is important to be sorted, to solve the naming problem
                                            * between TGeoManager and Geant4 with assembly volumes
                                            */

    std::vector<TString> fActiveVolumes;
    std::vector<TString> fSensitiveVolumes;
    std::vector<TString> fLogicalVolumes;
    std::vector<TString> fMaterials;

    std::map<TString, TString>
        fGeant4PhysicalToPhysicalMap; /*
                                       * maps TGeoManager volume name to Geant4 physical volume name,
                                       * only makes sense when using assembly
                                       */

    std::map<TString, TString> fPhysicalToLogicalVolumeMap;
    std::map<TString, std::vector<TString> > fLogicalToPhysicalMap;
    // many physical volumes can point to one single logical

    std::map<TString, TString> fLogicalToMaterialMap;

    std::map<TString, TVector3> fPhysicalToPositionInWorldMap;

    std::map<TString, Double_t> fPhysicalToStorageChanceMap;
    constexpr static const Double_t fDefaultStorageChance = 1.00;

    std::map<TString, Double_t> fPhysicalToMaxStepSizeMap;  // units in mm
    constexpr static const Double_t fDefaultMaxStepSize = 0.05;

   public:
    Bool_t IsSensitiveVolume(const TString&) const;
    Bool_t IsActiveVolume(const TString&) const;
    Bool_t IsLogicalVolume(const TString&) const;
    Bool_t IsPhysicalVolume(const TString&) const;

    inline std::vector<TString> GetSensitiveVolumes() const { return fPhysicalVolumes; }
    inline std::vector<TString> GetActiveVolumes() const { return fPhysicalVolumes; }
    inline std::vector<TString> GetPhysicalVolumes() const { return fPhysicalVolumes; }
    inline std::vector<TString> GetLogicalVolumes() const { return fLogicalVolumes; }
    inline std::vector<TString> GetMaterials() const { return fMaterials; }

    inline size_t GetNumberOfSensitiveVolumes() const { return fSensitiveVolumes.size(); }
    inline size_t GetNumberOfActiveVolumes() const { return fActiveVolumes.size(); }
    inline size_t GetNumberOfPhysicalVolumes() const { return fPhysicalVolumes.size(); }
    inline size_t GetNumberOfLogicalVolumes() const { return fLogicalVolumes.size(); }

    TVector3 GetPhysicalVolumePosition(const TString&) const;

    std::vector<TString> GetAllPhysicalFromLogical(const TString&) const;
    TString GetUniquePhysicalFromLogical(const TString&) const;

    TString GetLogicalFromPhysical(const TString&) const;

    TString GetMaterialFromLogical(const TString&) const;
    TString GetMaterialFromPhysical(const TString&) const;

    TString GetPhysicalFromGeant4Physical(const TString&) const;

    size_t GetActiveVolumeIndex(const TString&) const;
    TString GetActiveVolumeFromIndex(size_t) const;

    Double_t GetStorageChance(const TString&) const;
    Double_t GetMaxStepSize(const TString&) const;

    void PrintGeometryInfo() const;
    inline void Print() const { PrintGeometryInfo(); }

   public:
    void InsertActiveVolume(const TString& name, Double_t chance, Double_t maxStepSize);

    void LoadGdml(const TString& gdml);

   public:
    inline void InitFromConfigFile() override {}  // this class should not be initialized from rml

    TRestGeant4Geometry();

    void InitializeFromGeant4World(const G4VPhysicalVolume*);  // Implemented in package that links to Geant4

    void BuildAssemblyLookupTable(const G4VPhysicalVolume*);

   public:
    ClassDef(TRestGeant4Geometry, 1);
};

#endif  // REST_TRESTGEANT4GEOMETRY_H
