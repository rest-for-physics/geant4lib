//
// Created by lobis on 7/14/2022.
//

#include "TRestGeant4PrimaryGeneratorInfo.h"

#include <TAxis.h>
#include <TF1.h>
#include <TF2.h>
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
    cout << "TRestGeant4PrimaryGeneratorTypes::SpatialGeneratorTypesToString - Error - Unknown "
            "SpatialGeneratorTypes"
         << endl;
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
        cout << "TRestGeant4PrimaryGeneratorTypes::StringToSpatialGeneratorTypes - Error - Unknown "
                "SpatialGeneratorTypes: "
             << type << endl;
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
    cout << "TRestGeant4PrimaryGeneratorTypes::SpatialGeneratorShapesToString - Error - Unknown "
            "SpatialGeneratorShapes"
         << endl;
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
        cout << "TRestGeant4PrimaryGeneratorTypes::StringToSpatialGeneratorShapes - Error - Unknown "
                "SpatialGeneratorShapes: "
             << type << endl;
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
        case EnergyDistributionTypes::FORMULA2:
            return "Formula2";
        case EnergyDistributionTypes::MONO:
            return "Mono";
        case EnergyDistributionTypes::FLAT:
            return "Flat";
        case EnergyDistributionTypes::LOG:
            return "Log";
    }
    cout << "TRestGeant4PrimaryGeneratorTypes::EnergyDistributionTypesToString - Error - Unknown energy "
            "distribution type"
         << endl;
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
    } else if (TString(type).EqualTo(EnergyDistributionTypesToString(EnergyDistributionTypes::FORMULA2),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return EnergyDistributionTypes::FORMULA2;
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
        cout << "TRestGeant4PrimaryGeneratorTypes::StringToEnergyDistributionTypes - Error - Unknown "
                "EnergyDistributionTypes: "
             << type << endl;
        exit(1);
    }
}

string TRestGeant4PrimaryGeneratorTypes::EnergyDistributionFormulasToString(
    const EnergyDistributionFormulas& type) {
    switch (type) {
        case EnergyDistributionFormulas::COSMIC_NEUTRONS:
            return "CosmicNeutrons";
        case EnergyDistributionFormulas::COSMIC_GAMMAS:
            return "CosmicGammas";
    }
    cout << "TRestGeant4PrimaryGeneratorTypes::EnergyDistributionFormulasToString - Error - Unknown energy "
            "distribution formula"
         << endl;
    exit(1);
}

EnergyDistributionFormulas TRestGeant4PrimaryGeneratorTypes::StringToEnergyDistributionFormulas(
    const string& type) {
    if (TString(type).EqualTo(EnergyDistributionFormulasToString(EnergyDistributionFormulas::COSMIC_NEUTRONS),
                              TString::ECaseCompare::kIgnoreCase)) {
        return EnergyDistributionFormulas::COSMIC_NEUTRONS;
    } else if (TString(type).EqualTo(
                   EnergyDistributionFormulasToString(EnergyDistributionFormulas::COSMIC_GAMMAS),
                   TString::ECaseCompare::kIgnoreCase)) {
        return EnergyDistributionFormulas::COSMIC_GAMMAS;
    } else {
        cout << "TRestGeant4PrimaryGeneratorTypes::StringToEnergyDistributionFormulas - Error - Unknown "
                "energyDistributionFormulas: "
             << type << endl;
        exit(1);
    }
}

TF1 TRestGeant4PrimaryGeneratorTypes::EnergyDistributionFormulasToRootFormula(
    const EnergyDistributionFormulas& type) {
    switch (type) {
        case EnergyDistributionFormulas::COSMIC_NEUTRONS: {
            // Formula from https://ieeexplore.ieee.org/document/1369506
            const char* title = "Cosmic Neutrons at Sea Level";
            auto distribution =
                TF1(title,
                    "1.006E-6 * TMath::Exp(-0.3500 * TMath::Power(TMath::Log(x * 1E-3), 2) + 2.1451 * "
                    "TMath::Log(x * 1E-3)) + "
                    "1.011E-3 * TMath::Exp(-0.4106 * TMath::Power(TMath::Log(x * 1E-3), 2) - 0.6670 * "
                    "TMath::Log(x * 1E-3))",
                    1E2, 1E7);
            distribution.SetNormalized(true);  // Normalized, not really necessary
            distribution.SetTitle(title);
            distribution.GetXaxis()->SetTitle("Energy (keV)");
            return distribution;
        }
        case EnergyDistributionFormulas::COSMIC_GAMMAS:
            exit(1);
    }
    cout << "TRestGeant4PrimaryGeneratorTypes::EnergyDistributionFormulasToRootFormula - Error - Unknown "
            "energy distribution formula"
         << endl;
    exit(1);
}

string TRestGeant4PrimaryGeneratorTypes::AngularDistributionTypesToString(
    const AngularDistributionTypes& type) {
    switch (type) {
        case AngularDistributionTypes::TH1D:
            return "TH1D";
        case AngularDistributionTypes::FORMULA:
            return "Formula";
        case AngularDistributionTypes::FORMULA2:
            return "Formula2";
        case AngularDistributionTypes::ISOTROPIC:
            return "Isotropic";
        case AngularDistributionTypes::FLUX:
            return "Flux";
        case AngularDistributionTypes::BACK_TO_BACK:
            return "Back to back";
    }
    cout << "TRestGeant4PrimaryGeneratorTypes::AngularDistributionTypesToString - Error - Unknown angular "
            "distribution type"
         << endl;
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
    } else if (TString(type).EqualTo(AngularDistributionTypesToString(AngularDistributionTypes::FORMULA2),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return AngularDistributionTypes::FORMULA2;
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
        cout << "TRestGeant4PrimaryGeneratorTypes::StringToAngularDistributionTypes - Error - Unknown "
                "AngularDistributionTypes: "
             << type << endl;
        exit(1);
    }
}

string TRestGeant4PrimaryGeneratorTypes::AngularDistributionFormulasToString(
    const AngularDistributionFormulas& type) {
    switch (type) {
        case AngularDistributionFormulas::COS2:
            return "Cos2";
        case AngularDistributionFormulas::COS3:
            return "Cos3";
    }
    cout << "TRestGeant4PrimaryGeneratorTypes::AngularDistributionFormulasToString - Error - Unknown angular "
            "distribution formula"
         << endl;
    exit(1);
}

AngularDistributionFormulas TRestGeant4PrimaryGeneratorTypes::StringToAngularDistributionFormulas(
    const string& type) {
    if (TString(type).EqualTo(AngularDistributionFormulasToString(AngularDistributionFormulas::COS2),
                              TString::ECaseCompare::kIgnoreCase)) {
        return AngularDistributionFormulas::COS2;
    } else if (TString(type).EqualTo(AngularDistributionFormulasToString(AngularDistributionFormulas::COS3),
                                     TString::ECaseCompare::kIgnoreCase)) {
        return AngularDistributionFormulas::COS3;
    } else {
        cout << "TRestGeant4PrimaryGeneratorTypes::StringToAngularDistributionFormulas - Error - Unknown "
                "AngularDistributionFormulas: "
             << type << endl;
        exit(1);
    }
}

TF1 TRestGeant4PrimaryGeneratorTypes::AngularDistributionFormulasToRootFormula(
    const AngularDistributionFormulas& type) {
    switch (type) {
        case AngularDistributionFormulas::COS2: {
            auto cos2 = [](double* xs, double* ps) {
                if (xs[0] >= 0 && xs[0] <= TMath::Pi() / 2) {
                    return TMath::Power(TMath::Cos(xs[0]), 2);
                }
                return 0.0;
            };
            const char* title = "AngularDistribution: Cos2";
            auto f = TF1(title, cos2, 0.0, TMath::Pi());
            f.SetTitle(title);
            return f;
        }
        case AngularDistributionFormulas::COS3: {
            auto cos3 = [](double* xs, double* ps) {
                if (xs[0] >= 0 && xs[0] <= TMath::Pi() / 2) {
                    return TMath::Power(TMath::Cos(xs[0]), 3);
                }
                return 0.0;
            };
            const char* title = "AngularDistribution: Cos3";
            auto f = TF1(title, cos3, 0.0, TMath::Pi());
            f.SetTitle(title);
            return f;
        }
    }
    cout << "TRestGeant4PrimaryGeneratorTypes::AngularDistributionFormulasToRootFormula - Error - Unknown "
            "angular distribution formula"
         << endl;
    exit(1);
}

string TRestGeant4PrimaryGeneratorTypes::EnergyAndAngularDistributionFormulasToString(
    const EnergyAndAngularDistributionFormulas& type) {
    switch (type) {
        case EnergyAndAngularDistributionFormulas::COSMIC_MUONS:
            return "CosmicMuons";
    }
    cout << "TRestGeant4PrimaryGeneratorTypes::EnergyAndAngularDistributionFormulasToString - Error - "
            "Unknown energy/angular distribution formula"
         << endl;
    exit(1);
}

EnergyAndAngularDistributionFormulas
TRestGeant4PrimaryGeneratorTypes::StringToEnergyAndAngularDistributionFormulas(const string& type) {
    if (TString(type).EqualTo(
            EnergyAndAngularDistributionFormulasToString(EnergyAndAngularDistributionFormulas::COSMIC_MUONS),
            TString::ECaseCompare::kIgnoreCase)) {
        return EnergyAndAngularDistributionFormulas::COSMIC_MUONS;
    } else {
        cout << "TRestGeant4PrimaryGeneratorTypes::StringToEnergyAndAngularDistributionFormulas - Error - "
                "Unknown AngularDistributionFormulas: "
             << type << endl;
        exit(1);
    }
}

TF2 TRestGeant4PrimaryGeneratorTypes::EnergyAndAngularDistributionFormulasToRootFormula(
    const EnergyAndAngularDistributionFormulas& type) {
    switch (type) {
        case EnergyAndAngularDistributionFormulas::COSMIC_MUONS: {
            // GUAN formula from https://arxiv.org/pdf/1509.06176.pdf
            // muon rest mass is 105.7 MeV
            // formula returns energy in keV
            const char* title = "Cosmic Muons Energy and Angular";
            auto f =
                TF2(title,
                    "0.14*TMath::Power(x*1E-6*(1.+3.64/"
                    "(x*1E-6*TMath::Power(TMath::Power((TMath::Power(TMath::Cos(y),2)+0.0105212-0.068287*"
                    "TMath::Power(TMath::Cos(y),0.958633)+0.0407253*TMath::Power(TMath::Cos(y),0.817285)"
                    ")/(0.982960),0.5),1.29))),-2.7)*(1./"
                    "(1.+(1.1*x*1E-6*TMath::Power((TMath::Power(TMath::Cos(y),2)+0.0105212-0.068287*TMath::"
                    "Power(TMath::Cos(y),0.958633)+0.0407253*TMath::Power(TMath::Cos(y),0.817285))/"
                    "(0.982960),0.5))/115.)+0.054/"
                    "(1.+(1.1*x*1E-6*TMath::Power((TMath::Power(TMath::Cos(y),2)+0.0105212-0.068287*TMath::"
                    "Power(TMath::Cos(y),0.958633)+0.0407253*TMath::Power(TMath::Cos(y),0.817285))/"
                    "(0.982960),0.5))/850.))*(2.*TMath::Sin(y)*TMath::Pi())",
                    0.0, 1.0E10, 0, TMath::Pi() / 2.);
            f.SetTitle(title);
            return f;
        }
    }
    cout << "TRestGeant4PrimaryGeneratorTypes::EnergyAndAngularDistributionFormulasToRootFormula - Error - "
            "Unknown energy and angular distribution formula"
         << endl;
    exit(1);
}

void TRestGeant4PrimaryGeneratorInfo::Print() const {
    const auto typeEnum = StringToSpatialGeneratorTypes(fSpatialGeneratorType.Data());
    RESTMetadata << "Generator type: " << SpatialGeneratorTypesToString(typeEnum) << RESTendl;

    if (typeEnum != SpatialGeneratorTypes::POINT) {
        const auto shapeEnum = StringToSpatialGeneratorShapes(fSpatialGeneratorShape.Data());
        RESTMetadata << "Generator shape: " << SpatialGeneratorShapesToString(shapeEnum);
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

            RESTMetadata << fSpatialGeneratorSize.X() << ", " << fSpatialGeneratorSize.Y() << ", "
                         << fSpatialGeneratorSize.Z() << RESTendl;
        }
    }
    RESTMetadata << "Generator center : (" << fSpatialGeneratorPosition.X() << ","
                 << fSpatialGeneratorPosition.Y() << "," << fSpatialGeneratorPosition.Z() << ") mm"
                 << RESTendl;
    RESTMetadata << "Generator rotation : (" << fSpatialGeneratorRotationAxis.X() << ","
                 << fSpatialGeneratorRotationAxis.Y() << "," << fSpatialGeneratorRotationAxis.Z()
                 << "), angle: " << fSpatialGeneratorRotationValue * TMath::RadToDeg() << "º" << RESTendl;
}
