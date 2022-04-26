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
/// The TRestGeant4NeutronTaggingProcess generates observables based on veto volumes energy
/// depositions. It was first developed as a process for the IAXO experiment but can be used in any analysis.
/// It uses `keywords` to identify different relevant volumes (such as vetoes). The default veto keyword for
/// IAXO is `veto` and it will tag each volume containing the keyword as a veto volume, so avoid using the
/// keyword on volumes that do not act as vetoes. There are also keywords for shielding and capture volumes
/// (Cd layers).
///
/// ### Parameters:
///
/// * **vetoKeyword**: keyword to identify a volume of the geometry as a veto. The condition is that the
/// keyword
/// is contained inside the name of the volume. Only volumes serving as veto (i.e. scintillators) should
/// contain this vetoKeyword in their names.
///
/// * **captureKeyword**: keyword to identify a volume of the geometry as a capture volumes. These volumes
/// correspond to the volumes used to capture neutrons and produce easily detectable secondaries, such as Cd
/// layers. This parameter is optional and is useful to benchmark the effect of the capture volume and
/// material, for example, if a very low % of neutron captures occur on the `capture` volumes, they are not
/// very optimized.
///
/// * **shieldingKeyword**: keyword to identify the shielding volume. It is used to study the secondaries
/// coming
/// out of the shielding, as in IAXO most of the secondaries come from the shielding. If there are multiple
/// shielding volumes this may not work as expected.
///
/// * **vetoGroupKeywords**: comma separated keywords used to identify different groups of vetoes. This is an
/// optional parameter that when specified will make the process return additional observables on a per group
/// basis. The most common use case is using group names to identify the location of the vetoes (top, bottom,
/// ...). The volumes detected as vetoes (via vetoKeyword) will also be  assigned to a group if they contain a
/// keyword contained in this list.
///
/// * **vetoQuenchingFactors**: comma separated values for the quenching factors used in the analysis. The
/// observables will be calculated for each of the quenching factors contained in this list. Values between 0
/// and 1 only. This is useful in case the user doesn't know the exact value of the quenching factor. Also it
/// is useful to insert `0` or `1` to study the effects of electromagnetic processes only, or no quenching at
/// all.
///
/// ### Example usage
///
/// \code
///         <addProcess type="TRestGeant4NeutronTaggingProcess" name="g4Neutrons" value="ON"
///         observable="all">
///             <parameter name="vetoKeyword" value="veto"/>
///             <parameter name="captureKeyword" value="sheet"/>
///             <parameter name="shieldingKeyword" value="shielding"/>
///             <parameter name="vetoGroupKeywords" value="top, bottom, east, west, front, back"/>
///             <parameter name="vetoQuenchingFactors" value="0, 0.15, 1"/>
///         </addProcess>
/// \endcode
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2021-February: Implementation.
///
/// \class      TRestGeant4NeutronTaggingProcess
/// \author     Luis Obis
///
/// <hr>
///

#include "TRestGeant4NeutronTaggingProcess.h"

using namespace std;

ClassImp(TRestGeant4NeutronTaggingProcess);

TRestGeant4NeutronTaggingProcess::TRestGeant4NeutronTaggingProcess() { Initialize(); }

TRestGeant4NeutronTaggingProcess::TRestGeant4NeutronTaggingProcess(char* configFilename) {
    Initialize();
    if (LoadConfigFromFile(configFilename)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestGeant4NeutronTaggingProcess::~TRestGeant4NeutronTaggingProcess() { delete fOutputG4Event; }

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestGeant4NeutronTaggingProcess::LoadDefaultConfig() { SetTitle("Default config"); }

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestGeant4NeutronTaggingProcess::Initialize() {
    fG4Metadata = nullptr;

    SetSectionName(this->ClassName());
    SetLibraryVersion(LIBRARY_VERSION);

    fInputG4Event = nullptr;
    fOutputG4Event = new TRestGeant4Event();
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
/// correspondig TRestGeant4NeutronTaggingProcess section inside the RML.
///
void TRestGeant4NeutronTaggingProcess::LoadConfig(std::string configFilename, std::string name) {
    if (LoadConfigFromFile(configFilename, name)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Process initialization.
///
void TRestGeant4NeutronTaggingProcess::InitProcess() {
    fG4Metadata = GetMetadata<TRestGeant4Metadata>();

    // CAREFUL THIS METHOD IS CALLED TWICE!
    // we need to reset these variables to zero
    Reset();
    // get "veto" volumes
    if (fVetoVolumeIds.empty()) {
        for (unsigned int i = 0; i < fG4Metadata->GetNumberOfActiveVolumes(); i++) {
            string volume_name = (string)fG4Metadata->GetActiveVolumeName(i);
            volume_name = TrimAndLower(volume_name);
            if (volume_name.find(TrimAndLower(fVetoKeyword)) != string::npos) {
                fVetoVolumeIds.push_back(i);
            } else if (volume_name.find(TrimAndLower(fCaptureKeyword)) != string::npos) {
                fCaptureVolumeIds.push_back(i);
            } else if (volume_name.find(TrimAndLower(fShieldingKeyword)) != string::npos) {
                fShieldingVolumeIds.push_back(i);
            }
        }

        // veto groups (fill fVetoGroupVolumeNames)
        for (unsigned int i = 0; i < fVetoGroupKeywords.size(); i++) {
            string veto_group_keyword = TrimAndLower(fVetoGroupKeywords[i]);
            fVetoGroupVolumeNames[veto_group_keyword] = std::vector<string>{};
            for (int& id : fVetoVolumeIds) {
                string volume_name = (string)fG4Metadata->GetActiveVolumeName(id);
                volume_name = TrimAndLower(volume_name);
                if (volume_name.find(veto_group_keyword) != string::npos) {
                    fVetoGroupVolumeNames[veto_group_keyword].push_back(
                        (string)fG4Metadata->GetActiveVolumeName(id));
                }
            }
        }
    }

    PrintMetadata();
}

void TRestGeant4NeutronTaggingProcess::Reset() {
    /*
    fVetoVolumeIds.clear();
    fVetoGroupVolumeNames.clear();
    fCaptureVolumeIds.clear();
    */
    fNeutronsCapturedNumber = 0;
    fNeutronsCapturedPosX.clear();
    fNeutronsCapturedPosY.clear();
    fNeutronsCapturedPosZ.clear();
    fNeutronsCapturedIsCaptureVolume.clear();
    fNeutronsCapturedProductionE.clear();
    fNeutronsCapturedEDepByNeutron.clear();
    fNeutronsCapturedEDepByNeutronAndChildren.clear();
    fNeutronsCapturedEDepByNeutronInVeto.clear();
    fNeutronsCapturedEDepByNeutronAndChildrenInVeto.clear();
    fNeutronsCapturedEDepByNeutronAndChildrenInVetoMax.clear();
    fNeutronsCapturedEDepByNeutronAndChildrenInVetoMin.clear();

    fGammasNeutronCaptureNumber = 0;
    fGammasNeutronCapturePosX.clear();
    fGammasNeutronCapturePosY.clear();
    fGammasNeutronCapturePosZ.clear();
    fGammasNeutronCaptureIsCaptureVolume.clear();
    fGammasNeutronCaptureProductionE.clear();

    fSecondaryNeutronsShieldingNumber = 0;
    fSecondaryNeutronsShieldingExitPosX.clear();
    fSecondaryNeutronsShieldingExitPosY.clear();
    fSecondaryNeutronsShieldingExitPosZ.clear();
    fSecondaryNeutronsShieldingIsCaptured.clear();
    fSecondaryNeutronsShieldingIsCapturedInCaptureVolume.clear();
    fSecondaryNeutronsShieldingProductionE.clear();
    fSecondaryNeutronsShieldingExitE.clear();
}
///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestGeant4NeutronTaggingProcess::ProcessEvent(TRestEvent* evInput) {
    fInputG4Event = (TRestGeant4Event*)evInput;
    *fOutputG4Event = *((TRestGeant4Event*)evInput);

    Reset();
    std::map<string, Double_t> volume_energy_map;

    for (unsigned int i = 0; i < fVetoVolumeIds.size(); i++) {
        int id = fVetoVolumeIds[i];
        string volume_name = (string)fG4Metadata->GetActiveVolumeName(id);

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
            const auto& track = fOutputG4Event->GetTrack(i);
            string particle_name = (string)track.GetParticleName();
            for (const auto& id : fVetoVolumeIds) {
                string volume_name = (string)fG4Metadata->GetActiveVolumeName(id);

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

    std::set<int> neutronsCaptured = {};
    for (int i = 0; i < fOutputG4Event->GetNumberOfTracks(); i++) {
        auto track = fOutputG4Event->GetTrack(i);
        string particle_name = (string)track.GetParticleName();
        if (particle_name == "neutron") {
            auto hits = track.GetHits();
            for (int j = 0; j < hits.GetNumberOfHits(); j++) {
                string process_name = (string)track.GetProcessName(hits.GetProcess(j));
                if (process_name == "nCapture") {
                    // << "Neutron capture!!!!!! " << particle_name << "trackId " << track.GetTrackID()
                    //    << " hit " << j << endl;
                    // track.PrintTrack();
                    // hits.PrintHits(j + 1);

                    neutronsCaptured.insert(track.GetTrackID());

                    fNeutronsCapturedNumber += 1;
                    fNeutronsCapturedPosX.push_back(hits.GetX(j));
                    fNeutronsCapturedPosY.push_back(hits.GetY(j));
                    fNeutronsCapturedPosZ.push_back(hits.GetZ(j));

                    Int_t volumeId = hits.GetVolumeId(j);
                    Int_t isCaptureVolume = 0;
                    for (const auto& id : fCaptureVolumeIds) {
                        if (volumeId == id) {
                            isCaptureVolume = 1;
                            continue;
                        }
                    }
                    fNeutronsCapturedIsCaptureVolume.push_back(isCaptureVolume);
                    fNeutronsCapturedProductionE.push_back(track.GetKineticEnergy());

                    // get energy deposited by neutron that undergoes capture and children
                    double neutronsCapturedEDepByNeutron = 0;
                    double neutronsCapturedEDepByNeutronAndChildren = 0;
                    double neutronsCapturedEDepByNeutronInVeto = 0;
                    double neutronsCapturedEDepByNeutronAndChildrenInVeto = 0;

                    std::set<int> parents = {track.GetTrackID()};
                    std::map<int, double> energy_in_veto;
                    for (int child = 0; child < fOutputG4Event->GetNumberOfTracks(); child++) {
                        const auto& track_child = fOutputG4Event->GetTrack(child);
                        if ((parents.count(track_child.GetParentID()) > 0) ||
                            parents.count(track_child.GetTrackID()) > 0) {
                            // track or parent is in list of tracks, we add to list and add energy
                            parents.insert(track_child.GetTrackID());
                            neutronsCapturedEDepByNeutronAndChildren += track_child.GetEnergy();
                            if (track_child.GetTrackID() == track.GetTrackID()) {
                                neutronsCapturedEDepByNeutron += track_child.GetEnergy();
                            }
                            for (const auto& vetoId : fVetoVolumeIds) {
                                neutronsCapturedEDepByNeutronAndChildrenInVeto +=
                                    track_child.GetEnergyInVolume(vetoId);
                                energy_in_veto[vetoId] += track_child.GetEnergyInVolume(vetoId);
                                if (track_child.GetTrackID() == track.GetTrackID()) {
                                    neutronsCapturedEDepByNeutronInVeto +=
                                        track_child.GetEnergyInVolume(vetoId);
                                }
                            }
                        }
                    }

                    fNeutronsCapturedEDepByNeutron.push_back(neutronsCapturedEDepByNeutron);
                    fNeutronsCapturedEDepByNeutronAndChildren.push_back(
                        neutronsCapturedEDepByNeutronAndChildren);
                    fNeutronsCapturedEDepByNeutronInVeto.push_back(neutronsCapturedEDepByNeutronInVeto);
                    fNeutronsCapturedEDepByNeutronAndChildrenInVeto.push_back(
                        neutronsCapturedEDepByNeutronAndChildrenInVeto);

                    // get max and min energy in each veto (to compare with energy in ALL vetoes)
                    double energyMaxVeto = 0;
                    double energyMinVeto = -1;
                    for (const auto& pair : energy_in_veto) {
                        auto E = pair.second;
                        if (E > energyMaxVeto) energyMaxVeto = E;
                        if (E < energyMaxVeto || energyMinVeto == -1) energyMinVeto = E;
                    }

                    fNeutronsCapturedEDepByNeutronAndChildrenInVetoMax.push_back(energyMaxVeto);
                    fNeutronsCapturedEDepByNeutronAndChildrenInVetoMin.push_back(energyMinVeto);
                }
            }
        }
    }

    SetObservableValue("neutronsCapturedNumber", fNeutronsCapturedNumber);
    SetObservableValue("neutronsCapturedPosX", fNeutronsCapturedPosX);
    SetObservableValue("neutronsCapturedPosY", fNeutronsCapturedPosY);
    SetObservableValue("neutronsCapturedPosZ", fNeutronsCapturedPosZ);
    SetObservableValue("neutronsCapturedIsCaptureVolume", fNeutronsCapturedIsCaptureVolume);
    SetObservableValue("neutronsCapturedProductionE", fNeutronsCapturedProductionE);
    SetObservableValue("neutronsCapturedEDepByNeutron", fNeutronsCapturedEDepByNeutron);
    SetObservableValue("neutronsCapturedEDepByNeutronAndChildren", fNeutronsCapturedEDepByNeutronAndChildren);
    SetObservableValue("neutronsCapturedEDepByNeutronInVeto", fNeutronsCapturedEDepByNeutronInVeto);
    SetObservableValue("neutronsCapturedEDepByNeutronAndChildrenInVeto",
                       fNeutronsCapturedEDepByNeutronAndChildrenInVeto);
    SetObservableValue("neutronsCapturedEDepByNeutronAndChildrenInVetoMax",
                       fNeutronsCapturedEDepByNeutronAndChildrenInVetoMax);
    SetObservableValue("neutronsCapturedEDepByNeutronAndChildrenInVetoMin",
                       fNeutronsCapturedEDepByNeutronAndChildrenInVetoMin);
    for (int i = 0; i < fOutputG4Event->GetNumberOfTracks(); i++) {
        auto track = fOutputG4Event->GetTrack(i);
        string particle_name = (string)track.GetParticleName();
        if (particle_name == "gamma") {
            // check if gamma is child of captured neutron
            Int_t parent = track.GetParentID();
            if (neutronsCaptured.count(parent) > 0) {
                const auto& hits = track.GetHits();

                fGammasNeutronCaptureNumber += 1;
                fGammasNeutronCapturePosX.push_back(hits.GetX(0));
                fGammasNeutronCapturePosY.push_back(hits.GetY(0));
                fGammasNeutronCapturePosZ.push_back(hits.GetZ(0));

                Int_t volumeId = hits.GetVolumeId(0);
                Int_t isCaptureVolume = 0;
                for (const auto& id : fCaptureVolumeIds) {
                    if (volumeId == id) {
                        isCaptureVolume = 1;
                        continue;
                    }
                }
                fGammasNeutronCaptureIsCaptureVolume.push_back(isCaptureVolume);
                fGammasNeutronCaptureProductionE.push_back(track.GetKineticEnergy());

                // cout << "gamma capture" << endl;

                // hits.PrintHits(1);
            }
        }
    }

    SetObservableValue("gammasNeutronCaptureNumber", fGammasNeutronCaptureNumber);
    SetObservableValue("gammasNeutronCapturePosX", fGammasNeutronCapturePosX);
    SetObservableValue("gammasNeutronCapturePosY", fGammasNeutronCapturePosY);
    SetObservableValue("gammasNeutronCapturePosZ", fGammasNeutronCapturePosZ);
    SetObservableValue("gammasNeutronCaptureIsCaptureVolume", fGammasNeutronCaptureIsCaptureVolume);
    SetObservableValue("gammasNeutronCaptureProductionE", fGammasNeutronCaptureProductionE);

    std::set<int> secondaryNeutrons = {};  // avoid counting twice
    for (int i = 0; i < fOutputG4Event->GetNumberOfTracks(); i++) {
        auto track = fOutputG4Event->GetTrack(i);
        string particle_name = (string)track.GetParticleName();
        if (particle_name == "neutron" && track.GetParentID() != 0) {  // not consider primary
            // check if neutron exits shielding
            auto hits = track.GetHits();
            for (int j = 0; j < hits.GetNumberOfHits(); j++) {
                string process_name = (string)track.GetProcessName(hits.GetProcess(j));
                if (process_name == "Transportation") {
                    for (const auto& id : fShieldingVolumeIds) {
                        if (hits.GetVolumeId(j) == id) {
                            // transportation and shielding == exits shielding
                            if (secondaryNeutrons.count(track.GetTrackID()) == 0) {
                                // first time adding this secondary neutron
                                secondaryNeutrons.insert(track.GetTrackID());
                            } else {
                                continue;
                            }
                            fSecondaryNeutronsShieldingNumber += 1;
                            fSecondaryNeutronsShieldingExitPosX.push_back(hits.GetX(j));
                            fSecondaryNeutronsShieldingExitPosY.push_back(hits.GetY(j));
                            fSecondaryNeutronsShieldingExitPosZ.push_back(hits.GetZ(j));

                            Int_t volumeId = hits.GetVolumeId(j);
                            Int_t isCaptureVolume = 0;
                            for (const auto& id : fCaptureVolumeIds) {
                                if (volumeId == id) {
                                    isCaptureVolume = 1;
                                    continue;
                                }
                            }
                            Int_t isCaptured = 0;
                            if (neutronsCaptured.count(track.GetTrackID()) > 0) {
                                isCaptured = 1;
                            }
                            fSecondaryNeutronsShieldingIsCaptured.push_back(isCaptured);
                            if (isCaptured)
                                fSecondaryNeutronsShieldingIsCapturedInCaptureVolume.push_back(
                                    isCaptureVolume);
                            else {
                                fSecondaryNeutronsShieldingIsCapturedInCaptureVolume.push_back(0);
                            }

                            fSecondaryNeutronsShieldingProductionE.push_back(track.GetKineticEnergy());
                            fSecondaryNeutronsShieldingExitE.push_back(hits.GetKineticEnergy(j));
                        }
                    }
                }
            }
        }
    }

    SetObservableValue("secondaryNeutronsShieldingNumber", fSecondaryNeutronsShieldingNumber);
    SetObservableValue("secondaryNeutronsShieldingExitPosX", fSecondaryNeutronsShieldingExitPosX);
    SetObservableValue("secondaryNeutronsShieldingExitPosY", fSecondaryNeutronsShieldingExitPosY);
    SetObservableValue("secondaryNeutronsShieldingExitPosZ", fSecondaryNeutronsShieldingExitPosZ);
    SetObservableValue("secondaryNeutronsShieldingIsCaptured", fSecondaryNeutronsShieldingIsCaptured);
    SetObservableValue("secondaryNeutronsShieldingIsCapturedInCaptureVolume",
                       fSecondaryNeutronsShieldingIsCapturedInCaptureVolume);
    SetObservableValue("secondaryNeutronsShieldingProductionE", fSecondaryNeutronsShieldingProductionE);
    SetObservableValue("secondaryNeutronsShieldingExitE", fSecondaryNeutronsShieldingExitE);

    return fOutputG4Event;
}

///////////////////////////////////////////////
/// \brief Function to include required actions after all events have been
/// processed.
///
void TRestGeant4NeutronTaggingProcess::EndProcess() {
    // Function to be executed once at the end of the process
    // (after all events have been processed)

    // Start by calling the EndProcess function of the abstract class.
    // Comment this if you don't want it.
    // TRestEventProcess::EndProcess();
}

///////////////////////////////////////////////
/// \brief Function to read input parameters from the RML
/// TRestGeant4NeutronTaggingProcess metadata section
///
void TRestGeant4NeutronTaggingProcess::InitFromConfigFile() {
    // word to identify active volume as veto (default = "veto" e.g. "vetoTop")
    string veto_keyword = GetParameter("vetoKeyword", "veto");
    fVetoKeyword = TrimAndLower(veto_keyword);
    // comma separated tags: "top, bottom, ..."
    string veto_group_keywords = GetParameter("vetoGroupKeywords", "");
    stringstream ss(veto_group_keywords);
    while (ss.good()) {
        string substr;
        getline(ss, substr, ',');
        fVetoGroupKeywords.push_back(TrimAndLower(substr));
    }

    // word to identify active volume as capture sheet (cadmium, default = "sheet" e.g.
    // "scintillatorSheetTop1of4")
    string capture_keyword = GetParameter("captureKeyword", "sheet");
    fCaptureKeyword = TrimAndLower(capture_keyword);

    // word to identify active volume as shielding

    string shielding_keyword = GetParameter("shieldingKeyword", "shielding");
    fShieldingKeyword = TrimAndLower(shielding_keyword);

    // comma separated quenching factors: "0.15, 1.00, ..."
    string quenching_factors = GetParameter("vetoQuenchingFactors", "-1");
    stringstream ss_qf(quenching_factors);
    while (ss_qf.good()) {
        string substr;
        getline(ss_qf, substr, ',');
        substr = TrimAndLower(substr);
        Float_t quenching_factor = (Float_t)std::atof(substr.c_str());
        if (quenching_factor > 1 || quenching_factor < 0) {
            cout << "ERROR: quenching factor must be between 0 and 1" << endl;
            continue;
        }
        fQuenchingFactors.push_back(quenching_factor);
    }
}
