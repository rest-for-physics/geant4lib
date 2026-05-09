/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2016 GIFNA/TREX (University of Zaragoza)                *
 * For more information see http://gifna.unizar.es/trex                  *
 *                                                                       *
 * REST is free software: you can redistribute it and/or modify          *
 * it under the terms of the GNU General Public License as published by  *
 * the Free Software Foundation, either version 3 of the License, or     *
 * (at your option) any later version.                                   *
 *                                                                       *
 * REST is distributed in the hope that it will be useful,               *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the          *
 * GNU General Public License for more details.                          *
 *                                                                       *
 * You should have a copy of the GNU General Public License along with   *
 * REST in $REST_PATH/LICENSE.                                           *
 * If not, see http://www.gnu.org/licenses/.                             *
 * For the list of contributors see $REST_PATH/CREDITS.                  *
 *************************************************************************/

//////////////////////////////////////////////////////////////////////////
/// The TRestGeant4QuenchingProcess converts Geant4 deposited energies into a simple visible-energy estimate.
/// It supports a Lindhard-style nuclear recoil model and a Birks-law scintillator model. By default the model
/// is inferred from the volume name: scintillator/VETO-like volumes use Birks and the rest use Lindhard.
/// Physical volumes, logical volumes, or expressions (to be matched to either physical or logical) can be
/// used. Example usage: \code
///< TRestGeant4QuenchingProcess>
///
///    <parameter name="applyToHitEnergies" value="true"/>
///    <parameter name="birksConstant" value="0.126mm/MeV"/>
///    <parameter name="birksFallbackStepLength" value="0.5mm"/>
///
///    <volume name="^scintillatorVolume" model="birks"/>
///    <volume name="gasVolume" model="lindhard"/>
///
///    </TRestGeant4QuenchingProcess>
/// \endcode
///

#include "TRestGeant4QuenchingProcess.h"

#include <TMath.h>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <limits>

using namespace std;

ClassImp(TRestGeant4QuenchingProcess);

namespace {
string ToLower(string value) {
    transform(value.begin(), value.end(), value.begin(),
              [](unsigned char c) { return static_cast<char>(tolower(c)); });
    return value;
}

string NormalizeQuenchingModel(const string& model) {
    const auto normalized = ToLower(model);
    if (normalized == "auto" || normalized == "lindhard" || normalized == "birks") {
        return normalized;
    }

    RESTWarning << "TRestGeant4QuenchingProcess: Unknown quenching model '" << model
                << "'. Falling back to auto." << RESTendl;
    return "auto";
}

string InferQuenchingModel(const string& requestedModel, const string& userVolumeExpression,
                           const string& volumeName) {
    if (requestedModel != "auto") {
        return requestedModel;
    }

    const auto haystack = ToLower(userVolumeExpression + " " + volumeName);
    if (haystack.find("scintillator") != string::npos || haystack.find("veto") != string::npos) {
        return "birks";
    }
    return "lindhard";
}

double Clamp01(double value) {
    if (value < 0) {
        return 0;
    }
    if (value > 1) {
        return 1;
    }
    return value;
}

double LindhardQuenchingFactor(double recoilEnergy, int A, int Z) {
    if (recoilEnergy <= 0 || A <= 0 || Z <= 0) {
        return 1.0;
    }

    const double gamma = 11.5 * recoilEnergy * TMath::Power(Z, -7.0 / 3.0);
    const double g = 3 * TMath::Power(gamma, 0.15) + 0.7 * TMath::Power(gamma, 0.6) + gamma;
    const double k = 0.133 * TMath::Power(Z, 2.0 / 3.0) * TMath::Power(A, -1.0 / 2.0);
    return Clamp01(k * g / (1 + k * g));
}

bool IsNeutralParticleWithoutBirksQuenching(const string& particleName) {
    const auto name = ToLower(particleName);
    return name == "gamma" || name == "opticalphoton" || name == "geantino" || name == "chargedgeantino" ||
           name.find("neutrino") != string::npos;
}

double EstimateStepLength(const TRestGeant4Hits& hits, int hitIndex, double fallbackStepLength) {
    const auto nHits = static_cast<int>(hits.GetNumberOfHits());
    if (nHits <= 1) {
        return fallbackStepLength;
    }

    const auto volumeId = hits.GetVolumeId(hitIndex);
    const auto position = hits.GetPosition(hitIndex);
    double stepLength = numeric_limits<double>::max();

    if (hitIndex > 0 && hits.GetVolumeId(hitIndex - 1) == volumeId) {
        stepLength = min(stepLength, (position - hits.GetPosition(hitIndex - 1)).Mag());
    }
    if (hitIndex + 1 < nHits && hits.GetVolumeId(hitIndex + 1) == volumeId) {
        stepLength = min(stepLength, (hits.GetPosition(hitIndex + 1) - position).Mag());
    }

    if (!std::isfinite(stepLength) || stepLength <= 0 || stepLength == numeric_limits<double>::max()) {
        return fallbackStepLength;
    }
    return stepLength;
}

double BirksQuenchingFactor(double energy, double stepLength, double birksConstant) {
    if (energy <= 0 || stepLength <= 0 || birksConstant <= 0) {
        return 1.0;
    }

    const double dEdx = energy / stepLength;
    return Clamp01(1.0 / (1.0 + birksConstant * dEdx));
}
}  // namespace

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestGeant4QuenchingProcess::TRestGeant4QuenchingProcess() { Initialize(); }

///////////////////////////////////////////////
/// \brief Constructor loading data from a config file
///
/// If no configuration path is defined using TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// The default behaviour is that the config file must be specified with
/// full path, absolute or relative.
///
/// \param configFilename A const char* giving the path to an RML file.
///
TRestGeant4QuenchingProcess::TRestGeant4QuenchingProcess(const char* configFilename) {
    Initialize();

    if (LoadConfigFromFile(configFilename)) {
        LoadDefaultConfig();
    }
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestGeant4QuenchingProcess::~TRestGeant4QuenchingProcess() { delete fOutputG4Event; }

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestGeant4QuenchingProcess::LoadDefaultConfig() { SetTitle("Default config"); }

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestGeant4QuenchingProcess::Initialize() {
    fGeant4Metadata = nullptr;
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fInputG4Event = nullptr;
    fOutputG4Event = new TRestGeant4Event();

    fBirksConstant = 0.000126;       // 0.126 mm/MeV in REST units, mm/keV
    fBirksFallbackStepLength = 0.5;  // mm
    fApplyToHitEnergies = true;
}

///////////////////////////////////////////////
/// \brief Function to load the configuration from an external configuration
/// file.
///
/// If no configuration path is defined in TRestMetadata::SetConfigFilePath
/// the path to the config file must be specified using full path, absolute or
/// relative.
///
/// \param configFilename A const char* giving the path to an RML file.
/// \param name The name of the specific metadata. It will be used to find the
/// corresponding TRestGeant4QuenchingProcess section inside the RML.
///
void TRestGeant4QuenchingProcess::LoadConfig(const string& configFilename, const string& name) {
    if (LoadConfigFromFile(configFilename, name)) {
        LoadDefaultConfig();
    }
}

void TRestGeant4QuenchingProcess::InitFromConfigFile() {
    fUserVolumeExpressions.clear();
    fUserVolumeModels.clear();
    fUserVolumeBirksConstants.clear();
    fUserVolumeBirksFallbackStepLengths.clear();

    fApplyToHitEnergies = StringToBool(GetParameter("applyToHitEnergies", fApplyToHitEnergies));
    fBirksConstant = GetDblParameterWithUnits("birksConstant", fBirksConstant);
    fBirksFallbackStepLength = GetDblParameterWithUnits("birksFallbackStepLength", fBirksFallbackStepLength);

    TiXmlElement* volumeElement = GetElement("volume");
    while (volumeElement) {
        const string volumeName = GetParameter("name", volumeElement, "");
        if (volumeName.empty()) {
            cerr << "TRestGeant4QuenchingProcess: No volume expression specified" << endl;
            exit(1);
        }
        fUserVolumeExpressions.insert(volumeName);
        fUserVolumeModels[volumeName] =
            NormalizeQuenchingModel(GetParameter("model", volumeElement, string("auto")));
        fUserVolumeBirksConstants[volumeName] =
            GetDblParameterWithUnits("birksConstant", volumeElement, fBirksConstant);
        fUserVolumeBirksFallbackStepLengths[volumeName] =
            GetDblParameterWithUnits("birksFallbackStepLength", volumeElement, fBirksFallbackStepLength);
        volumeElement = GetNextElement(volumeElement);
    }
}

///////////////////////////////////////////////
/// \brief Process initialization. User volume expressions are matched to physical volumes
void TRestGeant4QuenchingProcess::InitProcess() {
    fGeant4Metadata = GetMetadata<TRestGeant4Metadata>();
    if (fGeant4Metadata == nullptr) {
        cerr << "TRestGeant4QuenchingProcess: No TRestGeant4Metadata found" << endl;
        exit(1);
    }

    fVolumes.clear();
    fVolumeModels.clear();
    fVolumeBirksConstants.clear();
    fVolumeBirksFallbackStepLengths.clear();

    const auto geometryInfo = fGeant4Metadata->GetGeant4GeometryInfo();
    // check all the user volume expressions are valid and correspond to at least a volume
    for (const auto& userVolume : fUserVolumeExpressions) {
        set<string> physicalVolumes = {};
        for (const auto& volume : geometryInfo.GetAllPhysicalVolumesMatchingExpression(userVolume)) {
            physicalVolumes.insert(volume.Data());
        }
        if (physicalVolumes.empty()) {
            // maybe it refers to a logical volume
            for (const auto& logicalVolume :
                 geometryInfo.GetAllLogicalVolumesMatchingExpression(userVolume)) {
                for (const auto& volume :
                     geometryInfo.GetAllPhysicalVolumesFromLogical(logicalVolume.Data())) {
                    physicalVolumes.insert(volume.Data());
                }
            }
        }

        if (physicalVolumes.empty()) {
            RESTWarning << "TRestGeant4QuenchingProcess: No volume found matching expression: " << userVolume
                        << RESTendl;
            continue;
        }

        for (const auto& physicalVolume : physicalVolumes) {
            const auto volumeName = geometryInfo.GetAlternativeNameFromGeant4PhysicalName(physicalVolume);
            const string volumeNameString = volumeName.Data();
            fVolumes.insert(volumeNameString);
            fVolumeModels[volumeNameString] =
                InferQuenchingModel(fUserVolumeModels[userVolume], userVolume, volumeNameString);
            fVolumeBirksConstants[volumeNameString] = fUserVolumeBirksConstants[userVolume];
            fVolumeBirksFallbackStepLengths[volumeNameString] =
                fUserVolumeBirksFallbackStepLengths[userVolume];
        }
    }

    RESTDebug << "TRestGeant4QuenchingProcess initialized with volumes" << RESTendl;
    for (const auto& volume : fVolumes) {
        RESTDebug << " " << volume << " (" << fVolumeModels[volume] << ")" << RESTendl;
    }
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestGeant4QuenchingProcess::ProcessEvent(TRestEvent* inputEvent) {
    fInputG4Event = (TRestGeant4Event*)inputEvent;
    *fOutputG4Event = *((TRestGeant4Event*)inputEvent);

    const string sensitiveVolumeName = fGeant4Metadata->GetSensitiveVolume().Data();
    const double sensitiveVolumeEnergyBefore = fOutputG4Event->GetEnergyInVolume(sensitiveVolumeName);

    fOutputG4Event->InitializeReferences(GetRunInfo());
    fOutputG4Event->fEnergyInVolumePerParticlePerProcess.clear();
    fOutputG4Event->fTotalDepositedEnergy = 0;
    fOutputG4Event->SetSensitiveVolumeEnergy(0);
    fill(fOutputG4Event->fVolumeDepositedEnergy.begin(), fOutputG4Event->fVolumeDepositedEnergy.end(), 0.0);

    bool missingHadronicInfoWarningPrinted = false;

    // loop over all tracks
    for (int trackIndex = 0; trackIndex < int(fOutputG4Event->GetNumberOfTracks()); trackIndex++) {
        // get the track
        TRestGeant4Track* track = fOutputG4Event->GetTrackPointer(trackIndex);
        const string particleName = track->GetParticleName().Data();

        auto hits = track->GetHitsPointer();
        auto& energy = hits->GetEnergyRef();
        for (int hitIndex = 0; hitIndex < int(hits->GetNumberOfHits()); hitIndex++) {
            const string volumeName = hits->GetVolumeName(hitIndex).Data();
            const double depositedEnergy = hits->GetEnergy(hitIndex);
            double quenchingFactor = 1.0;

            if (fVolumes.count(volumeName) && depositedEnergy > 0) {
                const auto model =
                    fVolumeModels.count(volumeName) ? fVolumeModels[volumeName] : string("lindhard");
                if (model == "birks") {
                    if (!IsNeutralParticleWithoutBirksQuenching(particleName)) {
                        const double stepLength =
                            EstimateStepLength(*hits, hitIndex, fVolumeBirksFallbackStepLengths[volumeName]);
                        quenchingFactor = BirksQuenchingFactor(depositedEnergy, stepLength,
                                                               fVolumeBirksConstants[volumeName]);
                    }
                } else if (model == "lindhard") {
                    if (hits->GetHadronicOk()) {
                        const string isotopeName = hits->GetHadronicTargetIsotopeName(hitIndex);
                        const int isotopeA = hits->GetHadronicTargetIsotopeA(hitIndex);
                        const int isotopeZ = hits->GetHadronicTargetIsotopeZ(hitIndex);
                        if (!isotopeName.empty()) {
                            quenchingFactor = LindhardQuenchingFactor(depositedEnergy, isotopeA, isotopeZ);
                        }
                    } else if (!missingHadronicInfoWarningPrinted) {
                        RESTWarning
                            << "TRestGeant4QuenchingProcess: Lindhard quenching requested but "
                               "hadronic target information is not available. Unquenched hit energies "
                               "will be used for those hits. Enable storeHadronicTargetInfo in restG4 "
                               "to apply this model."
                            << RESTendl;
                        missingHadronicInfoWarningPrinted = true;
                    }
                }
            }

            const auto visibleEnergy = depositedEnergy * quenchingFactor;
            if (fApplyToHitEnergies) {
                energy[hitIndex] = visibleEnergy;
            }

            const auto processName = hits->GetProcessName(hitIndex);

            if (visibleEnergy > 0) {
                fOutputG4Event->AddEnergyInVolumeForParticleForProcess(visibleEnergy, volumeName,
                                                                       particleName, processName.Data());
                const auto volumeId = hits->GetVolumeId(hitIndex);
                if (volumeId >= 0 && volumeId < int(fOutputG4Event->fVolumeDepositedEnergy.size())) {
                    fOutputG4Event->fVolumeDepositedEnergy[volumeId] += visibleEnergy;
                }
            }
        }
    }

    const double sensitiveVolumeEnergyAfter = fOutputG4Event->GetEnergyInVolume(sensitiveVolumeName);
    fOutputG4Event->SetSensitiveVolumeEnergy(sensitiveVolumeEnergyAfter);

    bool sensitiveQuenched = TMath::Abs(sensitiveVolumeEnergyAfter - sensitiveVolumeEnergyBefore) > 1e-2;
    SetObservableValue("sensitiveQuenched", sensitiveQuenched);
    SetObservableValue("sensitiveVolumeEnergyBefore", sensitiveVolumeEnergyBefore);
    SetObservableValue("sensitiveVolumeEnergyAfter", sensitiveVolumeEnergyAfter);

    return fOutputG4Event;
}

void TRestGeant4QuenchingProcess::EndProcess() {}

void TRestGeant4QuenchingProcess::PrintMetadata() {
    BeginPrintProcess();
    RESTMetadata << "Apply to hit energies: " << (fApplyToHitEnergies ? "true" : "false") << RESTendl;
    RESTMetadata << "Default Birks constant: " << fBirksConstant << " mm/keV" << RESTendl;
    RESTMetadata << "Default Birks fallback step length: " << fBirksFallbackStepLength << " mm" << RESTendl;
    for (auto const& volume : fVolumes) {
        RESTMetadata << "Volume: " << volume << " model=" << fVolumeModels[volume] << RESTendl;
    }
    EndPrintProcess();
}

std::set<std::string> TRestGeant4QuenchingProcess::GetVolumes() const { return fVolumes; }

std::set<std::string> TRestGeant4QuenchingProcess::GetUserVolumeExpressions() const {
    return fUserVolumeExpressions;
}
