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

#include <TObjArray.h>
#include <TObjString.h>
#include <TString.h>

#include <iostream>
#include <unordered_map>

using namespace std;

ClassImp(TRestGeant4VetoAnalysisProcess);

int findMostFrequent(const std::vector<int>& nums) {
    if (nums.empty()) {
        return 0;
    }

    std::unordered_map<int, int> freqMap;

    // Count the frequency of each element
    for (int num : nums) {
        freqMap[num]++;
    }

    int mostFrequent = nums[0];
    int maxFrequency = 0;

    // Find the most frequent element
    for (const auto& entry : freqMap) {
        if (entry.second > maxFrequency) {
            mostFrequent = entry.first;
            maxFrequency = entry.second;
        }
    }

    return mostFrequent;
}

Veto ExtractVetoFromString(const TString& input) {
    // example input:
    // VetoSystem_vetoSystemEast_vetoLayerEast2_assembly-13.veto2_scintillatorVolume-1500.0mm-f1a5df68

    Veto veto;
    veto.name = input;

    TObjArray* parts = input.Tokenize("_");

    if (parts->GetEntries() >= 5) {
        TString group = ((TObjString*)parts->At(1))->GetString();
        // remove leading "vetoSystem" from group if present
        if (group.Index("vetoSystem") == 0) {
            group = group(10, group.Length() - 10);
        }

        TString layer = ((TObjString*)parts->At(2))->GetString();
        // layer is last character of layer string
        layer = layer(layer.Length() - 1, 1);

        TString numberAndRest = ((TObjString*)parts->At(parts->GetEntries() - 2))->GetString();
        Ssiz_t vetoPos = numberAndRest.Index("veto");
        if (vetoPos != kNPOS) {
            TString number = numberAndRest(vetoPos + 4, numberAndRest.Length() - vetoPos - 4);

            TString lengthAndRest = ((TObjString*)parts->At(parts->GetEntries() - 1))->GetString();
            Ssiz_t mmPos = lengthAndRest.Index("mm");
            if (mmPos != kNPOS) {
                TString length = lengthAndRest(0, mmPos);
                // if "scintillatorVolume" is present, remove it
                if (length.Index("scintillatorVolume") == 0) {
                    length = length(19, length.Length() - 19);
                }

                veto.group = group.Data();
                veto.layer = layer.Atoi();
                veto.number = number.Atoi();
                veto.length = length.Atof();

            } else {
                std::cout << "No match found." << std::endl;
            }
        } else {
            std::cout << "No match found." << std::endl;
        }
    } else {
        std::cout << "No match found." << std::endl;
    }

    delete parts;

    veto.alias = veto.group + "_L" + veto.layer + "_N" + veto.number;
    return veto;
}

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

    if (fGeant4Metadata == nullptr) {
        // maybe it was manually initialized
        fGeant4Metadata = GetMetadata<TRestGeant4Metadata>();
    }
    if (fGeant4Metadata == nullptr) {
        cerr << "TRestGeant4VetoAnalysisProcess::InitProcess: Geant4 metadata not found" << endl;
        exit(1);
    }

    cout << "Expression: " << fVetoVolumesExpression << endl;
    const auto& geometryInfo = fGeant4Metadata->GetGeant4GeometryInfo();

    auto vetoVolumes = geometryInfo.GetAllPhysicalVolumesMatchingExpression(fVetoVolumesExpression);
    if (vetoVolumes.empty()) {
        const auto logicalVolumes =
            geometryInfo.GetAllLogicalVolumesMatchingExpression(fVetoVolumesExpression);
        for (const auto& logicalVolume : logicalVolumes) {
            for (const auto& physicalVolume : geometryInfo.GetAllPhysicalVolumesFromLogical(logicalVolume)) {
                const string name =
                    geometryInfo.GetAlternativeNameFromGeant4PhysicalName(physicalVolume).Data();
                Veto veto = ExtractVetoFromString(name);
                veto.position = geometryInfo.GetPosition(name);
                fVetoVolumes.push_back(veto);
            }
        }
    }

    PrintMetadata();
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestGeant4VetoAnalysisProcess::ProcessEvent(TRestEvent* inputEvent) {
    fInputEvent = (TRestGeant4Event*)inputEvent;
    *fOutputEvent = *((TRestGeant4Event*)inputEvent);

    std::map<string, double> vetoEnergyMap;
    std::map<string, double> vetoGroupEnergyMap;
    std::map<std::pair<string, int>, double> vetoGroupLayerEnergyMap;
    double totalVetoEnergy = 0;

    for (const auto& veto : fVetoVolumes) {
        const double energy = fInputEvent->GetEnergyInVolume(veto.name);

        totalVetoEnergy += energy;
        vetoEnergyMap[veto.name] = energy;
        vetoGroupEnergyMap[veto.group] += energy;
        vetoGroupLayerEnergyMap[std::make_pair(veto.group, veto.layer)] += energy;

        SetObservableValue("EnergyVeto_" + veto.alias, energy);
    }

    for (const auto& [group, energy] : vetoGroupEnergyMap) {
        SetObservableValue("EnergyGroup_" + group, energy);
    }

    for (const auto& [groupLayer, energy] : vetoGroupLayerEnergyMap) {
        SetObservableValue("EnergyGroupLayer_" + groupLayer.first + "_" + std::to_string(groupLayer.second),
                           energy);
    }

    SetObservableValue("EnergyTotal", totalVetoEnergy);

    // compute max energy
    double maxEnergy = 0;
    for (const auto& [name, energy] : vetoEnergyMap) {
        if (energy > maxEnergy) {
            maxEnergy = energy;
        }
    }
    SetObservableValue("EnergyMax", maxEnergy);

    // compute neutron capture observables
    int nCaptures = 0;
    int nCapturesInCaptureVolumes = 0;
    int nCapturesInVetoVolumes = 0;

    std::vector<float> nCapturesInCaptureVolumesTimes;
    std::vector<std::vector<float>> nCapturesInCaptureVolumesChildGammaEnergies;
    std::vector<float> nCapturesInCaptureVolumesChildGammasEnergyInVetos;
    std::vector<int> nCapturesInCaptureVolumesNumberOfVetoesHitByCapture;

    for (const auto& track : fInputEvent->GetTracks()) {
        if (track.GetParticleName() == "neutron") {
            const auto hits = track.GetHits();
            for (size_t hitIndex = 0; hitIndex < hits.GetNumberOfHits(); hitIndex++) {
                const auto processName = hits.GetProcessName(hitIndex);
                if (processName != "nCapture") {
                    continue;
                }
                nCaptures++;
                const string volumeName = hits.GetVolumeName(hitIndex).Data();
                const double time = hits.GetTime(hitIndex);

                if (volumeName.find("captureLayerVolume") != std::string::npos) {
                    nCapturesInCaptureVolumes++;
                    nCapturesInCaptureVolumesTimes.push_back(time);
                    std::vector<float> childrenEnergy;
                    const auto children = track.GetChildrenTracks();
                    for (const auto& child : children) {
                        if (child->GetParticleName() != "gamma") {
                            continue;
                        }
                        if (child->GetCreatorProcess() != "nCapture") {
                            continue;
                        }
                        childrenEnergy.push_back(child->GetInitialKineticEnergy());
                        double energyInVeto = 0;
                        for (const auto& veto : fVetoVolumes) {
                            energyInVeto += child->GetEnergyInVolume(veto.name, true);
                        }
                        nCapturesInCaptureVolumesChildGammasEnergyInVetos.push_back(energyInVeto);
                    }
                    nCapturesInCaptureVolumesChildGammaEnergies.push_back(childrenEnergy);

                    int numberOfVetoesHitByCapture = 0;
                    for (const auto& veto : fVetoVolumes) {
                        const double energyInVeto = track.GetEnergyInVolume(veto.name, true);
                        if (energyInVeto > 100) {  // soft limit of 100 keV
                            numberOfVetoesHitByCapture++;
                        }
                    }
                    nCapturesInCaptureVolumesNumberOfVetoesHitByCapture.push_back(numberOfVetoesHitByCapture);
                }
                if (volumeName.find("scintillatorVolume") != std::string::npos) {
                    nCapturesInVetoVolumes++;
                }
            }
        }
    }

    // Set capture observables
    SetObservableValue("nCaptures", nCaptures);
    SetObservableValue("nCapturesInCaptureVolumes", nCapturesInCaptureVolumes);
    SetObservableValue("nCapturesInVetoVolumes", nCapturesInVetoVolumes);

    // Set capture time observables
    SetObservableValue("nCapturesInCaptureVolumesTimes", nCapturesInCaptureVolumesTimes);
    // cannot insert a vector of vectors, need to flatten it
    std::vector<float> nCapturesInCaptureVolumesChildGammaEnergiesFlat;
    for (const auto& v : nCapturesInCaptureVolumesChildGammaEnergies) {
        nCapturesInCaptureVolumesChildGammaEnergiesFlat.insert(
            nCapturesInCaptureVolumesChildGammaEnergiesFlat.end(), v.begin(), v.end());
    }
    SetObservableValue("nCapturesInCaptureVolumesChildGammaEnergies",
                       nCapturesInCaptureVolumesChildGammaEnergiesFlat);
    SetObservableValue("nCapturesInCaptureVolumesChildGammaCount",
                       nCapturesInCaptureVolumesChildGammaEnergiesFlat.size());

    SetObservableValue("nCapturesInCaptureVolumesChildGammasEnergyInVetos",
                       nCapturesInCaptureVolumesChildGammasEnergyInVetos);

    auto nCapturesInCaptureVolumesChildGammasEnergyInVetosTotal = 0;
    for (const auto& energy : nCapturesInCaptureVolumesChildGammasEnergyInVetos) {
        nCapturesInCaptureVolumesChildGammasEnergyInVetosTotal += energy;
    }

    SetObservableValue("nCapturesInCaptureVolumesChildGammasEnergyInVetosTotal",
                       nCapturesInCaptureVolumesChildGammasEnergyInVetosTotal);

    SetObservableValue("nCapturesInCaptureVolumesNumberOfVetoesHitByCapture",
                       nCapturesInCaptureVolumesNumberOfVetoesHitByCapture);

    int nCapturesInCaptureVolumesNumberOfVetoesHitByCaptureTotal = 0;
    float nCapturesInCaptureVolumesNumberOfVetoesHitByCaptureAverage = 0;
    for (const auto& n : nCapturesInCaptureVolumesNumberOfVetoesHitByCapture) {
        nCapturesInCaptureVolumesNumberOfVetoesHitByCaptureTotal += n;
        nCapturesInCaptureVolumesNumberOfVetoesHitByCaptureAverage +=
            float(n) / nCapturesInCaptureVolumesNumberOfVetoesHitByCapture.size();
    }
    SetObservableValue("nCapturesInCaptureVolumesNumberOfVetoesHitByCaptureTotal",
                       nCapturesInCaptureVolumesNumberOfVetoesHitByCaptureTotal);

    SetObservableValue("nCapturesInCaptureVolumesNumberOfVetoesHitByCaptureAverage",
                       nCapturesInCaptureVolumesNumberOfVetoesHitByCaptureAverage);

    SetObservableValue("nCapturesInCaptureVolumesNumberOfVetoesHitByCaptureMostFrequent",
                       findMostFrequent(nCapturesInCaptureVolumesNumberOfVetoesHitByCapture));

    return fOutputEvent;
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
    fVetoVolumesExpression = GetParameter("vetoVolumesExpression", fVetoVolumesExpression);
}

void TRestGeant4VetoAnalysisProcess::PrintMetadata() {
    BeginPrintProcess();

    cout << "Number of veto volumes: " << fVetoVolumes.size() << endl;
    for (const auto& veto : fVetoVolumes) {
        cout << "Veto: " << veto.name << endl;
        cout << "   - Alias: " << veto.alias << endl;
        cout << "   - Group: " << veto.group << endl;
        cout << "   - Layer: " << veto.layer << endl;
        cout << "   - Number: " << veto.number << endl;
        cout << "   - Position: (" << veto.position.x() << ", " << veto.position.y() << ", "
             << veto.position.z() << ")" << endl;
        cout << "   - Length: " << veto.length << endl;
    }
}
