//
// Created by lobis on 7/14/2022.
//

#include "TRestGeant4PrimaryGeneratorInfo.h"

#include <TMath.h>
#include <TRestStringOutput.h>
#include <tinyxml.h>

#include <iostream>

using namespace std;
using namespace TRestGeant4PrimaryGeneratorTypes;

string TRestGeant4PrimaryGeneratorTypes::SpatialGeneratorTypesToString(const SpatialGeneratorTypes& type) {
    switch (type) {
        case SpatialGeneratorTypes::CUSTOM:
            return "Custom";
        case SpatialGeneratorTypes::VOLUME:
            return "Volume";
        case SpatialGeneratorTypes::SURFACE:
            return "Surface";
        case SpatialGeneratorTypes::POINT:
            return "Point";
    }
    cout << "Error: Unknown SpatialGeneratorTypes" << endl;
    exit(1);
}

SpatialGeneratorTypes TRestGeant4PrimaryGeneratorTypes::StringToSpatialGeneratorTypes(const string& type) {
    if (TString(type).EqualTo(SpatialGeneratorTypesToString(SpatialGeneratorTypes::CUSTOM),
                              TString::ECaseCompare::kIgnoreCase)) {
        return SpatialGeneratorTypes::CUSTOM;
    } else if (TString(type).EqualTo(SpatialGeneratorTypesToString(SpatialGeneratorTypes::VOLUME),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return SpatialGeneratorTypes::VOLUME;
    } else if (TString(type).EqualTo(SpatialGeneratorTypesToString(SpatialGeneratorTypes::SURFACE),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return SpatialGeneratorTypes::SURFACE;
    } else if (TString(type).EqualTo(SpatialGeneratorTypesToString(SpatialGeneratorTypes::POINT),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return SpatialGeneratorTypes::POINT;
    } else {
        cout << "Error: Unknown SpatialGeneratorTypes: " << type << endl;
        exit(1);
    }
}

string TRestGeant4PrimaryGeneratorTypes::SpatialGeneratorShapesToString(const SpatialGeneratorShapes& type) {
    switch (type) {
        case SpatialGeneratorShapes::GDML:
            return "GDML";
        case SpatialGeneratorShapes::WALL:
            return "Wall";
        case SpatialGeneratorShapes::CIRCLE:
            return "Circle";
        case SpatialGeneratorShapes::BOX:
            return "Box";
        case SpatialGeneratorShapes::SPHERE:
            return "Sphere";
        case SpatialGeneratorShapes::CYLINDER:
            return "Cylinder";
    }
    cout << "Error: Unknown SpatialGeneratorShapes" << endl;
    exit(1);
}

SpatialGeneratorShapes TRestGeant4PrimaryGeneratorTypes::StringToSpatialGeneratorShapes(const string& type) {
    if (TString(type).EqualTo(SpatialGeneratorShapesToString(SpatialGeneratorShapes::GDML),
                              TString::ECaseCompare::kIgnoreCase)) {
        return SpatialGeneratorShapes::GDML;
    } else if (TString(type).EqualTo(SpatialGeneratorShapesToString(SpatialGeneratorShapes::WALL),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return SpatialGeneratorShapes::WALL;
    } else if (TString(type).EqualTo(SpatialGeneratorShapesToString(SpatialGeneratorShapes::CIRCLE),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return SpatialGeneratorShapes::CIRCLE;
    } else if (TString(type).EqualTo(SpatialGeneratorShapesToString(SpatialGeneratorShapes::BOX),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return SpatialGeneratorShapes::BOX;
    } else if (TString(type).EqualTo(SpatialGeneratorShapesToString(SpatialGeneratorShapes::SPHERE),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return SpatialGeneratorShapes::SPHERE;
    } else if (TString(type).EqualTo(SpatialGeneratorShapesToString(SpatialGeneratorShapes::CYLINDER),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return SpatialGeneratorShapes::CYLINDER;

    } else {
        cout << "Error: Unknown SpatialGeneratorShapes: " << type << endl;
        exit(1);
    }
}

string TRestGeant4PrimaryGeneratorTypes::EnergyDistributionTypesToString(
    const EnergyDistributionTypes& type) {
    switch (type) {
        case EnergyDistributionTypes::TH1D:
            return "TH1D";
        case EnergyDistributionTypes::FORMULA:
            return "Formula";
        case EnergyDistributionTypes::MONO:
            return "Mono";
        case EnergyDistributionTypes::FLAT:
            return "Flat";
        case EnergyDistributionTypes::LOG:
            return "Log";
    }
    cout << "Error: Unknown EnergyDistributionTypes" << endl;
    exit(1);
}

EnergyDistributionTypes TRestGeant4PrimaryGeneratorTypes::StringToEnergyDistributionTypes(
    const string& type) {
    if (TString(type).EqualTo(EnergyDistributionTypesToString(EnergyDistributionTypes::TH1D),
                              TString::ECaseCompare::kIgnoreCase)) {
        return EnergyDistributionTypes::TH1D;
    } else if (TString(type).EqualTo(EnergyDistributionTypesToString(EnergyDistributionTypes::FORMULA),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return EnergyDistributionTypes::FORMULA;
    } else if (TString(type).EqualTo(EnergyDistributionTypesToString(EnergyDistributionTypes::MONO),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return EnergyDistributionTypes::MONO;
    } else if (TString(type).EqualTo(EnergyDistributionTypesToString(EnergyDistributionTypes::FLAT),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return EnergyDistributionTypes::FLAT;
    } else if (TString(type).EqualTo(EnergyDistributionTypesToString(EnergyDistributionTypes::LOG),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return EnergyDistributionTypes::LOG;
    } else {
        cout << "Error: Unknown EnergyDistributionTypes: " << type << endl;
        exit(1);
    }
}

string TRestGeant4PrimaryGeneratorTypes::AngularDistributionTypesToString(
    const AngularDistributionTypes& type) {
    switch (type) {
        case AngularDistributionTypes::TH1D:
            return "TH1D";
        case AngularDistributionTypes::FORMULA:
            return "Formula";
        case AngularDistributionTypes::ISOTROPIC:
            return "Isotropic";
        case AngularDistributionTypes::FLUX:
            return "Flux";
        case AngularDistributionTypes::BACK_TO_BACK:
            return "Back to back";
    }
    cout << "Error: Unknown AngularDistributionTypes" << endl;
    exit(1);
}

AngularDistributionTypes TRestGeant4PrimaryGeneratorTypes::StringToAngularDistributionTypes(
    const string& type) {
    if (TString(type).EqualTo(AngularDistributionTypesToString(AngularDistributionTypes::TH1D),
                              TString::ECaseCompare::kIgnoreCase)) {
        return AngularDistributionTypes::TH1D;
    } else if (TString(type).EqualTo(AngularDistributionTypesToString(AngularDistributionTypes::FORMULA),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return AngularDistributionTypes::FORMULA;
    } else if (TString(type).EqualTo(AngularDistributionTypesToString(AngularDistributionTypes::ISOTROPIC),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return AngularDistributionTypes::ISOTROPIC;
    } else if (TString(type).EqualTo(AngularDistributionTypesToString(AngularDistributionTypes::FLUX),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return AngularDistributionTypes::FLUX;
    } else if (TString(type).EqualTo(AngularDistributionTypesToString(AngularDistributionTypes::BACK_TO_BACK),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return AngularDistributionTypes::BACK_TO_BACK;
    } else {
        cout << "Error: Unknown AngularDistributionTypes: " << type << endl;
        exit(1);
    }
}

void TRestGeant4PrimaryGeneratorInfo::Print() const {
    const auto typeEnum = StringToSpatialGeneratorTypes(fSpatialGeneratorType.Data());
    const auto shapeEnum = StringToSpatialGeneratorShapes(fSpatialGeneratorShape.Data());

    RESTMetadata << "Generator type: " << fSpatialGeneratorType << RESTendl;
    RESTMetadata << "Generator shape: " << fSpatialGeneratorShape;

    if (shapeEnum == SpatialGeneratorShapes::GDML) {
        RESTMetadata << "::" << fSpatialGeneratorFrom << RESTendl;
    } else {
        if (shapeEnum == SpatialGeneratorShapes::BOX) {
            RESTMetadata << ", (length, width, height): ";
        } else if (shapeEnum == SpatialGeneratorShapes::SPHERE) {
            RESTMetadata << ", (radius, , ): ";
        } else if (shapeEnum == SpatialGeneratorShapes::WALL) {
            RESTMetadata << ", (length, width, ): ";
        } else if (shapeEnum == SpatialGeneratorShapes::CIRCLE) {
            RESTMetadata << ", (radius, , ): ";
        } else if (shapeEnum == SpatialGeneratorShapes::CYLINDER) {
            RESTMetadata << ", (radius, length, ): ";
        }

        if (typeEnum != SpatialGeneratorTypes::POINT) {
            RESTMetadata << fSpatialGeneratorSize.X() << ", " << fSpatialGeneratorSize.Y() << ", "
                         << fSpatialGeneratorSize.Z() << RESTendl;
        } else {
            RESTMetadata << RESTendl;
        }
    }
    RESTMetadata << "Generator center : (" << fSpatialGeneratorPosition.X() << ","
                 << fSpatialGeneratorPosition.Y() << "," << fSpatialGeneratorPosition.Z() << ") mm"
                 << RESTendl;
    RESTMetadata << "Generator rotation : (" << fSpatialGeneratorRotationAxis.X() << ","
                 << fSpatialGeneratorRotationAxis.Y() << "," << fSpatialGeneratorRotationAxis.Z()
                 << "), angle: " << fSpatialGeneratorRotationValue * TMath::RadToDeg() << "º" << RESTendl;
}
