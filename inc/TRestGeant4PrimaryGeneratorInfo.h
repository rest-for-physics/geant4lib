
#ifndef REST_TRESTGEANT4PRIMARYGENERATORINFO_H
#define REST_TRESTGEANT4PRIMARYGENERATORINFO_H

#include <TF1.h>
#include <TF2.h>
#include <TString.h>
#include <TVector3.h>

#include <string>

namespace TRestGeant4PrimaryGeneratorTypes {

enum class SpatialGeneratorTypes {
    CUSTOM,
    VOLUME,
    SURFACE,
    POINT,
    COSMIC,
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
    FORMULA2,
    MONO,
    FLAT,
    LOG,
};

std::string EnergyDistributionTypesToString(const EnergyDistributionTypes&);
EnergyDistributionTypes StringToEnergyDistributionTypes(const std::string&);

enum class EnergyDistributionFormulas {
    COSMIC_NEUTRONS,
    COSMIC_GAMMAS,
};

std::string EnergyDistributionFormulasToString(const EnergyDistributionFormulas&);
EnergyDistributionFormulas StringToEnergyDistributionFormulas(const std::string&);
TF1 EnergyDistributionFormulasToRootFormula(const EnergyDistributionFormulas&);

enum class AngularDistributionTypes {
    TH1D,
    FORMULA,
    FORMULA2,
    ISOTROPIC,
    FLUX,
    BACK_TO_BACK,
};

std::string AngularDistributionTypesToString(const AngularDistributionTypes&);
AngularDistributionTypes StringToAngularDistributionTypes(const std::string&);

enum class AngularDistributionFormulas {
    COS2,
    COS3,
};

std::string AngularDistributionFormulasToString(const AngularDistributionFormulas&);
AngularDistributionFormulas StringToAngularDistributionFormulas(const std::string&);
TF1 AngularDistributionFormulasToRootFormula(const AngularDistributionFormulas&);

enum class EnergyAndAngularDistributionFormulas {
    COSMIC_MUONS,
};

std::string EnergyAndAngularDistributionFormulasToString(const EnergyAndAngularDistributionFormulas&);
EnergyAndAngularDistributionFormulas StringToEnergyAndAngularDistributionFormulas(const std::string&);

TF2 EnergyAndAngularDistributionFormulasToRootFormula(const EnergyAndAngularDistributionFormulas&);

}  // namespace TRestGeant4PrimaryGeneratorTypes

class TiXmlElement;

class TRestGeant4PrimaryGeneratorInfo {
    ClassDef(TRestGeant4PrimaryGeneratorInfo, 1);

   public:
    void Print() const;

   private:
    /* Members related to spatial generator */
    /// Type of spatial generator (point, surface, volume, custom)
    TString fSpatialGeneratorType = "point";

    /// Shape of spatial generator (wall, GDML, sphere, etc)
    TString fSpatialGeneratorShape;

    /// The volume name where the events are generated, in case of volume or surface generator types
    TString fSpatialGeneratorFrom;

    /// The position of the generator for virtual, and point, generator types
    TVector3 fSpatialGeneratorPosition;

    /// \brief A 3d-std::vector with the angles, measured in degrees, of a XYZ rotation
    /// applied to the virtual generator. This rotation is used by virtualWall,
    /// virtualCircleWall and virtualCylinder generators.
    TVector3 fSpatialGeneratorRotationAxis;

    /// \brief degrees of rotation for generator virtual shape along the axis
    Double_t fSpatialGeneratorRotationValue = 0;

    /// \brief The size of the generator. Stores up to three dimensions according to the shape
    /// box: length, width, height
    /// sphere: radius
    /// wall: length, width
    /// circle: radius
    /// cylinder: radius, length
    TVector3 fSpatialGeneratorSize;

    /// \brief Defines density distribution of the generator shape. rho=F(x,y,z), in range 0~1
    TString fSpatialGeneratorSpatialDensityFunction;

   public:
    /// \brief Returns a std::string specifying the generator type (volume, surface, point,
    /// virtualWall, etc )
    inline TString GetSpatialGeneratorType() const { return fSpatialGeneratorType; }

    /// \brief Returns a std::string specifying the generator shape (point, wall, box, etc )
    inline TString GetSpatialGeneratorShape() const { return fSpatialGeneratorShape; }

    /// \brief Returns the name of the GDML volume where primary events are
    /// produced. This value has meaning only when using volume or surface
    /// generator types.
    inline TString GetSpatialGeneratorFrom() const { return fSpatialGeneratorFrom; }

    /// \brief Returns a 3d-std::vector with the position of the primary event
    /// generator. This value has meaning only when using point and virtual
    /// generator types.
    inline TVector3 GetSpatialGeneratorPosition() const { return fSpatialGeneratorPosition; }

    /// \brief Returns a 3d-std::vector, fGenRotation, with the XYZ rotation angle
    /// values in degrees. This value is used by virtualWall, virtualCircleWall
    /// and virtualCylinder generator types.
    inline TVector3 GetSpatialGeneratorRotationAxis() const { return fSpatialGeneratorRotationAxis; }

    /// \brief Returns the degree of rotation
    inline Double_t GetSpatialGeneratorRotationValue() const { return fSpatialGeneratorRotationValue; }

    /// \brief Returns the main spatial dimension of virtual generator.
    /// It is the size of a  virtualBox.
    inline TVector3 GetSpatialGeneratorSize() const { return fSpatialGeneratorSize; }

    /// \brief Returns the density function of the generator
    inline TString GetSpatialGeneratorSpatialDensityFunction() const {
        return fSpatialGeneratorSpatialDensityFunction;
    }

    friend class TRestGeant4Metadata;
    friend class DetectorConstruction;
};

#endif  // REST_TRESTGEANT4PRIMARYGENERATORINFO_H
