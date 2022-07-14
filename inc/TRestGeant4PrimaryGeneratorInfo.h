
#ifndef REST_TRESTGEANT4PRIMARYGENERATORINFO_H
#define REST_TRESTGEANT4PRIMARYGENERATORINFO_H

namespace TRestGeant4PrimaryGeneratorInfo {

enum class generator_types {
    CUSTOM,
    VOLUME,
    SURFACE,
    POINT,
};
extern std::map<std::string, generator_types> generator_types_map;

enum class generator_shapes {
    GDML,
    WALL,
    CIRCLE,
    BOX,
    SPHERE,
    CYLINDER,
};
extern std::map<std::string, generator_shapes> generator_shapes_map;

enum class energy_dist_types {
    TH1D,
    FORMULA,
    MONO,
    FLAT,
    LOG,
};
extern std::map<std::string, energy_dist_types> energy_dist_types_map;

enum class angular_dist_types {
    TH1D,
    FORMULA,
    ISOTROPIC,
    FLUX,
    BACK_TO_BACK,
};

class TRestGeant4PrimaryGeneratorInfo {
    ClassDef(TRestGeant4PrimaryGeneratorInfo, 1);

   private:
    // Related to spatial generator
    TString fSpatialGeneratorType;
    TString fSpatialGeneratorShape;
    TString fSpatialGeneratorGdmlFrom;
    TString fSpatialGeneratorVolumeFrom;
    TVector3 fSpatialGeneratorPosition;
    TVector3 fSpatialGeneratorRotationAxis;
    Double_t fSpatialGeneratorRotationValue;
    TVector3 fSpatialGeneratorSize;
    TString fSpatialGeneratorSpatialDensity;
};

#endif  // REST_TRESTGEANT4PRIMARYGENERATORINFO_H
