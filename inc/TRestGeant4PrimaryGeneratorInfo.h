
#ifndef REST_TRESTGEANT4PRIMARYGENERATORINFO_H
#define REST_TRESTGEANT4PRIMARYGENERATORINFO_H

#include <TString.h>
#include <TVector3.h>

#include <string>

namespace TRestGeant4PrimaryGeneratorTypes {

enum class SpatialGeneratorTypes {
    CUSTOM,
    VOLUME,
    SURFACE,
    POINT,
};

std::string SpatialGeneratorTypesToString(const SpatialGeneratorTypes&);
SpatialGeneratorTypes StringToSpatialGeneratorTypes(const std::string&);

enum class SpatialGeneratorShapes {
    GDML,
    WALL,
    CIRCLE,
    BOX,
    SPHERE,
    CYLINDER,
};

std::string SpatialGeneratorShapesToString(const SpatialGeneratorShapes&);
SpatialGeneratorShapes StringToSpatialGeneratorShapes(const std::string&);

enum class EnergyDistributionTypes {
    TH1D,
    FORMULA,
    MONO,
    FLAT,
    LOG,
};

std::string EnergyDistributionTypesToString(const EnergyDistributionTypes&);
EnergyDistributionTypes StringToEnergyDistributionTypes(const std::string&);

enum class AngularDistributionTypes {
    TH1D,
    FORMULA,
    ISOTROPIC,
    FLUX,
    BACK_TO_BACK,
};

std::string AngularDistributionTypesToString(const AngularDistributionTypes&);
AngularDistributionTypes StringToAngularDistributionTypes(const std::string&);

}  // namespace TRestGeant4PrimaryGeneratorTypes

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
