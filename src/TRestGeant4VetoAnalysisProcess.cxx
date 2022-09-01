/*************************************************************************
 * This file is part of the REST software framework.                     *
 *                                                                       *
 * Copyright (C) 2020 GIFNA/TREX (University of Zaragoza)                *
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
/// The TRestGeant4VetoAnalysisProcess generates observables based on veto volumes energy depositions. It was
/// first developed as a process for the IAXO experiment but can be used in any analysis. It uses `keywords`
/// to identify different relevant volumes (such as vetoes). The default veto keyword for IAXO is `veto` and
/// it will tag each volume containing the keyword as a veto volume, so avoid using the keyword on volumes
/// that do not act as vetoes.
///
/// Parameters:
/// `vetoKeyword`: keyword to identify a volume of the geometry as a veto. The condition is that the keyword
/// is contained inside the name of the volume. Only volumes serving as veto (i.e. scintillators) should
/// contain this vetoKeyword in their names.
/// `vetoGroupKeywords`: comma separated keywords used to identify different groups of vetoes. This is an
/// optional parameter that when specified will make the process return additional observables on a per group
/// basis. The most common use case is using group names to identify the location of the vetoes (top, bottom,
/// ...). The volumes detected as vetoes (via vetoKeyword) will also be  assigned to a group if they contain a
/// keyword contained in this list.
/// `vetoQuenchingFactors`: comma separated values for the quenching factors used in the analysis. The
/// observables will be calculated for each of the quenching factors contained in this list. Values between 0
/// and 1 only. This is useful in case the user doesn't know the exact value of the quenching factor. Also it
/// is useful to insert `0` or `1` to study the effects of electromagnetic processes only, or no quenching at
/// all.
///
/// Example usage:
///         <addProcess type="TRestGeant4VetoAnalysisProcess" name="g4Veto" value="ON"
///         observable="all">
///             <parameter name="vetoKeyword" value="veto"/>
///             <parameter name="vetoGroupKeywords" value="top, bottom, east, west, front, back"/>
///             <parameter name="vetoQuenchingFactors" value="0, 0.15, 1"/>
///         </addProcess>
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2020-September: Implementation.
///
/// \class      TRestGeant4VetoAnalysisProcess
/// \author     Luis Obis
///
/// <hr>
///

#include "TRestGeant4VetoAnalysisProcess.h"

#include <fmt/color.h>
#include <fmt/core.h>

using namespace fmt;
using namespace std;

ClassImp(TRestGeant4VetoAnalysisProcess);

TRestGeant4VetoAnalysisProcess::TRestGeant4VetoAnalysisProcess() { Initialize(); }

TRestGeant4VetoAnalysisProcess::TRestGeant4VetoAnalysisProcess(const char* configFilename) {
    TRestGeant4VetoAnalysisProcess();
    if (LoadConfigFromFile(configFilename)) {
        LoadDefaultConfig();
    }
}

TRestGeant4VetoAnalysisProcess::~TRestGeant4VetoAnalysisProcess() { delete fOutputEvent; }

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestGeant4VetoAnalysisProcess::LoadDefaultConfig() { SetTitle("Default config"); }

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestGeant4VetoAnalysisProcess::Initialize() {
    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fOutputEvent = new TRestGeant4Event();
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
/// corresponding TRestGeant4VetoAnalysisProcess section inside the RML.
///
void TRestGeant4VetoAnalysisProcess::LoadConfig(const string& configFilename, const string& name) {
    if (LoadConfigFromFile(configFilename, name)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Process initialization.
///
void TRestGeant4VetoAnalysisProcess::InitProcess() {
    // CAREFUL THIS METHOD IS CALLED TWICE!
    fVetoVolumes.clear();
    fVetoDetectorVolumes.clear();
    fVetoDetectorBoundaryDirection.clear();
    fVetoDetectorBoundaryPosition.clear();

    if (fGeant4Metadata == nullptr) {
        // maybe it was manually initialized
        fGeant4Metadata = GetMetadata<TRestGeant4Metadata>();
    }
    if (fGeant4Metadata == nullptr) {
        cerr << "TRestGeant4VetoAnalysisProcess::InitProcess: Geant4 metadata not found" << endl;
        exit(1);
    }

    const auto& geometryInfo = fGeant4Metadata->GetGeant4GeometryInfo();

    fVetoVolumes = geometryInfo.GetAllPhysicalVolumesMatchingExpression(fVetoVolumesExpression);
    if (fVetoVolumes.empty()) {
        const auto logicalVolumes =
            geometryInfo.GetAllLogicalVolumesMatchingExpression(fVetoVolumesExpression);
        for (const auto& logicalVolume : logicalVolumes) {
            for (const auto& physicalVolume : geometryInfo.GetAllPhysicalVolumesFromLogical(logicalVolume)) {
                fVetoVolumes.push_back(geometryInfo.GetAlternativeNameFromGeant4PhysicalName(physicalVolume));
            }
        }
    }
    if (fVetoVolumes.empty()) {
        cerr << "TRestGeant4VetoAnalysisProcess::InitProcess: No veto volumes found" << endl;
        exit(1);
    }

    // get detector volumes if requested
    if (!fVetoDetectorsExpression.IsNull()) {
        fVetoDetectorVolumes = geometryInfo.GetAllPhysicalVolumesMatchingExpression(fVetoDetectorsExpression);
        if (fVetoDetectorVolumes.empty()) {
            const auto logicalVolumes =
                geometryInfo.GetAllLogicalVolumesMatchingExpression(fVetoDetectorsExpression);
            for (const auto& logicalVolume : logicalVolumes) {
                for (const auto& physicalVolume :
                     geometryInfo.GetAllPhysicalVolumesFromLogical(logicalVolume)) {
                    fVetoDetectorVolumes.push_back(
                        geometryInfo.GetAlternativeNameFromGeant4PhysicalName(physicalVolume));
                }
            }
        }
        if (fVetoDetectorVolumes.empty()) {
            cerr << "TRestGeant4VetoAnalysisProcess::InitProcess: No detector volumes found" << endl;
            exit(1);
        }
        if (fVetoDetectorVolumes.size() != fVetoVolumes.size()) {
            cerr << "TRestGeant4VetoAnalysisProcess::InitProcess: Number of detector volumes "
                 << "does not match number of veto volumes" << endl;
            exit(1);
        }
    }

    for (int i = 0; i < fVetoDetectorVolumes.size(); i++) {
        const auto& vetoName = fVetoVolumes[i];
        const auto& vetoPosition = geometryInfo.GetPosition(vetoName);

        const auto& vetoDetectorName = fVetoDetectorVolumes[i];
        const auto& vetoDetectorPosition = geometryInfo.GetPosition(vetoDetectorName);

        const auto distance = vetoDetectorPosition - vetoPosition;
        const auto direction = distance.Unit();

        fVetoDetectorBoundaryDirection[vetoName] = direction;
        fVetoDetectorBoundaryPosition[vetoName] = vetoDetectorPosition - direction * fVetoDetectorOffsetSize;
    }

    PrintMetadata();
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestGeant4VetoAnalysisProcess::ProcessEvent(TRestEvent* inputEvent) {
    /*
    fInputEvent = (TRestGeant4Event*)inputEvent;
    *fOutputEvent = *((TRestGeant4Event*)inputEvent);

    std::map<string, Double_t> volume_energy_map;

    for (unsigned int i = 0; i < fVetoVolumeIds.size(); i++) {
        int id = fVetoVolumeIds[i];
        string volume_name = (string)fGeant4Metadata->GetActiveVolumeName(id);

        Double_t energy = fOutputG4Event->GetEnergyDepositedInVolume(id);
        volume_energy_map[volume_name] = energy;
    }

    Double_t energy_veto_max = 0;
    for (const auto& pair : volume_energy_map) {
        Double_t veto_energy = pair.second;
        SetObservableValue(pair.first + "VolumeEDep", veto_energy);
        if (veto_energy > energy_veto_max) {
            energy_veto_max = veto_energy;
        };
    }
    SetObservableValue("vetoAllEVetoMax", energy_veto_max);

    // veto groups
    for (const auto& pair : fVetoGroupVolumeNames) {
        Double_t energy_veto_max_group = 0;
        for (unsigned int i = 0; i < pair.second.size(); i++) {
            string volume_name = pair.second[i];
            Double_t veto_energy = volume_energy_map[volume_name];
            if (veto_energy > energy_veto_max_group) {
                energy_veto_max_group = veto_energy;
            };
        }
        // convert to Upper + lower case (VetoGroupTopEVetoMax, ...)
        string group_name;
        for (auto it = pair.first.cbegin(); it != pair.first.cend(); ++it) {
            if (it == pair.first.cbegin()) {
                group_name += std::toupper(*it);
            } else {
                group_name += std::tolower(*it);
            }
        }
        SetObservableValue("vetoGroup" + group_name + "EVetoMax", energy_veto_max_group);
    }

    //

    for (const auto& quenching_factor : fQuenchingFactors) {
        string quenching_factor_string = std::to_string(quenching_factor);
        // replace "." in string by "_" because its gives very strange problems
        quenching_factor_string =
            quenching_factor_string.replace(quenching_factor_string.find("."), sizeof(".") - 1, "_");
        volume_energy_map.clear();
        for (int i = 0; i < fOutputG4Event->GetNumberOfTracks(); i++) {
            auto track = fOutputG4Event->GetTrack(i);
            string particle_name = (string)track.GetParticleName();
            for (const auto& id : fVetoVolumeIds) {
                string volume_name = (string)fGeant4Metadata->GetActiveVolumeName(id);

                if (particle_name == "e-" || particle_name == "e+" || particle_name == "gamma") {
                    // no quenching factor
                    volume_energy_map[volume_name] += track.GetEnergyInVolume(id);
                } else {
                    // apply quenching factor
                    volume_energy_map[volume_name] += quenching_factor * track.GetEnergyInVolume(id);
                }
            }
        }

        Double_t energy_veto_max = 0;
        for (const auto& pair : volume_energy_map) {
            Double_t veto_energy = pair.second;
            SetObservableValue(pair.first + "VolumeEDep" + "Qf" + quenching_factor_string, veto_energy);
            if (veto_energy > energy_veto_max) {
                energy_veto_max = veto_energy;
            };
        }
        SetObservableValue(string("vetoAllEVetoMax") + "Qf" + quenching_factor_string, energy_veto_max);

        // veto groups
        for (const auto& pair : fVetoGroupVolumeNames) {
            Double_t energy_veto_max_group = 0;
            for (unsigned int i = 0; i < pair.second.size(); i++) {
                string volume_name = pair.second[i];
                Double_t veto_energy = volume_energy_map[volume_name];
                if (veto_energy > energy_veto_max_group) {
                    energy_veto_max_group = veto_energy;
                };
            }
            // convert to Upper + lower case (VetoGroupTopEVetoMax, ...)
            string group_name;
            for (auto it = pair.first.cbegin(); it != pair.first.cend(); ++it) {
                if (it == pair.first.cbegin()) {
                    group_name += std::toupper(*it);
                } else {
                    group_name += std::tolower(*it);
                }
            }
            SetObservableValue("vetoGroup" + group_name + "EVetoMax" + "Qf" + quenching_factor_string,
                               energy_veto_max_group);
        }
    }

    return fOutputG4Event;
     */
}

///////////////////////////////////////////////
/// \brief Function to include required actions after all events have been
/// processed.
///
void TRestGeant4VetoAnalysisProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

///////////////////////////////////////////////
/// \brief Function to read input parameters from the RML
/// TRestGeant4VetoAnalysisProcess metadata section
///
void TRestGeant4VetoAnalysisProcess::InitFromConfigFile() {
    // word to identify active volume as veto (default = "veto" e.g. "vetoTop")
    fVetoVolumesExpression = GetParameter("vetoVolumesExpression", fVetoVolumesExpression);
    fVetoDetectorsExpression = GetParameter("vetoDetectorsExpression", fVetoDetectorsExpression);

    fVetoDetectorOffsetSize = GetDblParameterWithUnits("vetoDetectorOffset", fVetoDetectorOffsetSize);
    fVetoLightAttenuation = GetDblParameterWithUnits("vetoLightAttenuation", fVetoLightAttenuation);
    fVetoQuenchingFactor = GetDblParameterWithUnits("quenchingFactor", fVetoQuenchingFactor);
}

// TODO: Find how to place this so that we don't need to copy it in every source file
template <>
struct fmt::formatter<TVector3> : formatter<string> {
    auto format(const TVector3& c, format_context& ctx) {
        string s = fmt::format("({:0.3f}, {:0.3f}, {:0.3f})", c.X(), c.Y(), c.Z());
        return formatter<string>::format(s, ctx);
    }
};

void TRestGeant4VetoAnalysisProcess::PrintMetadata() {
    BeginPrintProcess();

    cout << "Veto volume expression: " << fVetoVolumesExpression << endl;
    if (!fVetoDetectorsExpression.IsNull()) {
        cout << "Veto detector expression: " << fVetoDetectorsExpression << endl;
        cout << "Veto detector offset: " << fVetoDetectorOffsetSize << endl;
        cout << "Veto light attenuation: " << fVetoLightAttenuation << endl;
    } else {
        cout << "Veto detector expression: not set" << endl;
    }
    cout << "Veto quenching factor: " << fVetoQuenchingFactor << endl;

    RESTDebug << RESTendl;

    if (fVetoVolumes.empty()) {
        cout << "Process not initialized yet" << endl;
        return;
    }

    cout << "Number of veto volumes: " << fVetoVolumes.size() << endl;
    cout << "Number of veto detector volumes: " << fVetoDetectorVolumes.size() << endl;

    const auto& geometryInfo = fGeant4Metadata->GetGeant4GeometryInfo();
    for (int i = 0; i < fVetoVolumes.size(); i++) {
        const auto& vetoName = fVetoVolumes[i];
        const auto& vetoPosition = geometryInfo.GetPosition(vetoName);

        print(" - Veto volume: {} - name: '{}' - position: {} mm\n", i, vetoName, vetoPosition);

        if (fVetoDetectorVolumes.empty()) {
            continue;
        }

        const auto& vetoDetectorName = fVetoDetectorVolumes[i];
        const auto& vetoDetectorPosition = geometryInfo.GetPosition(vetoDetectorName);

        print("   Veto detector name: '{}' - position: {} mm\n", vetoDetectorName, vetoDetectorPosition);

        print("   Boundary position: {} mm - direction: {}\n", fVetoDetectorBoundaryPosition.at(vetoName),
              fVetoDetectorBoundaryDirection.at(vetoName));
    }
}
