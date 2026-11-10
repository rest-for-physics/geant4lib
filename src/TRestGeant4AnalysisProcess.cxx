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
/// The TRestGeant4AnalysisProcess allows to extract valuable information
/// from a TRestGeant4Event which contains the information of energy deposits
/// from a Geant4 MonteCarlo simulation. TRestGeant4Event stores hits which
/// include the physical process and geometry volume id where the
/// interaction took place. This process accesses the Geant4 event data
/// and metadata information and it will add the observables defined by
/// the user to the analysisTree.
///
/// All energies are given in keV, positions in mm and angles in radians
/// unless stated otherwise.
///
/// ### Volume names and aliases
///
/// Observables related to a volume require the name of a volume as it is
/// registered in TRestGeant4Metadata, i.e. the full path of the physical
/// volume (e.g. `shielding/vessel/gas`). Since these names may be long or
/// contain characters not allowed in a branch name, the volume can be given
/// through the `volume` attribute, and the observable name will then be just
/// an alias, which must still end in the corresponding keyword.
///
/// \code
///    <observable name="gasVolumeEDep" value="ON" volume="shielding/vessel/gas"
///            description="Energy deposited in the gas volume in keV" />
/// \endcode
///
/// \warning The volume must be defined as an *active* volume in the
/// TRestGeant4Metadata `<storage>` section of the simulation, otherwise the
/// observable is not registered and a warning with the list of active
/// volumes is printed.
///
/// ### Generic observables
///
/// This process includes generic observables by using a common pattern
/// inside the observable name. These observables require to be completed
/// with the name of a volume, a process or a particle following Geant4
/// conventions.
///
/// * **<volume>VolumeEDep**: The total energy deposited in a particular
/// volume of the geometry.
/// \code
///    <observable name="gasVolumeEDep" value="ON" volume="shielding/vessel/gas"
///            description="Energy deposited in the gas volume in keV" />
///
///    <observable name="vesselVolumeEDep" value="ON" volume="shielding/vessel"
///            description="Energy deposited in the vessel volume in keV" />
/// \endcode
///
/// * **<volume>MeanPos<X|Y|Z>**: The mean position of the hits that have
/// been registered in a particular volume of the geometry. The keyword
/// `MeanPos` must be followed by the axis (`X`, `Y` or `Z`).
/// \code
///    <observable name="gasMeanPosX" value="ON" volume="shielding/vessel/gas"
///            description="Mean hits position in the gas volume (X-axis)" />
///
///    <observable name="gasMeanPosY" value="ON" volume="shielding/vessel/gas"
///            description="Mean hits position in the gas volume (Y-axis)" />
/// \endcode
///
/// * **<volume><Process>Process**: The energy deposited (in keV) in a
/// particular volume by a given physics process. `<Process>` is either a
/// REST process name from the table below, or a Geant4 process name with its
/// first letter capitalized (e.g. `Compt`, `HadElastic`, `NCapture`).
/// \code
///    <observable name="gasComptonProcess" value="ON" volume="shielding/vessel/gas"
///            description="Energy deposited in the gas by Compton scattering in keV" />
///
///    <observable name="gasHadElasticProcess" value="ON" volume="shielding/vessel/gas"
///            description="Energy deposited in the gas by hadron elastic scattering in keV" />
/// \endcode
///
/// * **<particle>TracksCounter**: The number of tracks of a given Geant4
/// particle found at each event.
/// \code
///    <observable name="neutronTracksCounter" value="ON"
///            description="Number of neutron tracks found in the event" />
///
///    <observable name="gammaTracksCounter" value="ON"
///            description="Number of gamma tracks found in the event" />
/// \endcode
///
/// * **<particle>TracksEDep**: The total energy deposited by the tracks of
/// a given Geant4 particle (in all volumes).
/// \code
///    <observable name="neutronTracksEDep" value="ON"
///            description="Energy deposited by neutron tracks in keV" />
///
///    <observable name="gammaTracksEDep" value="ON"
///            description="Energy deposited by gamma tracks in keV" />
/// \endcode
///
/// \warning It is important to notice that the keyword used for these
/// observables is case sensitive!
///
/// ### Physics process observables
///
/// * **containsProcess<Process>**: It is 1 if the event contains at least one
/// hit produced by the given Geant4 process, in any volume, and 0 otherwise.
/// `<Process>` follows the same naming rules as in `<volume><Process>Process`,
/// e.g. `containsProcessCompt`, `containsProcessHadElastic` or
/// `containsProcessNCapture`. `containsProcessPhot` and `containsProcessCompt`
/// are always evaluated, any other is evaluated only if it is defined in the
/// RML. If the process does not exist in the physics information stored in
/// TRestGeant4Metadata, a warning is printed and the value will be 0.
///
/// \note REST identifies processes by Geant4 process type and subtype, so
/// processes sharing them (e.g. `eIoni`, `hIoni`, `ionIoni` and `muIoni`) cannot
/// be distinguished by `containsProcess<Process>`. Energy based observables
/// (`<volume><Process>Process` and `PerProcess<Name>`) use the process name and
/// are not affected.
///
/// The following table lists the REST process names and their Geant4
/// equivalent.
///
/// REST name           | Geant4 name        | Description
/// --------------------|--------------------|-------------------------------------
/// Photoelectric       | phot               | Photoelectric absorption
/// Compton             | compt              | Compton scattering
/// Rayleigh            | Rayl               | Rayleigh scattering
/// Conversion          | conv               | Gamma conversion (pair production)
/// PhotonNuclear       | photonNuclear      | Photonuclear interaction
/// Bremsstrahlung      | eBrem              | Electron/positron bremsstrahlung
/// Annihilation        | annihil            | Positron annihilation
/// EIoni               | eIoni              | Electron/positron ionisation
/// HIoni               | hIoni              | Hadron ionisation
/// IonIoni             | ionIoni            | Ion ionisation
/// MuIoni              | muIoni             | Muon ionisation
/// Msc                 | msc                | Multiple scattering
/// CoulombScat         | CoulombScat        | Single Coulomb scattering
/// HadElastic          | hadElastic         | Hadron elastic scattering
/// NeutronInelastic    | neutronInelastic   | Neutron inelastic scattering
/// ProtonInelastic     | protonInelastic    | Proton inelastic scattering
/// NCapture            | nCapture           | Neutron capture
/// RadioactiveDecay    | RadioactiveDecay   | Radioactive decay
/// Decay               | Decay              | Particle decay
///
/// The legacy names `Bremstralung`, `NInelastic` and `RadiactiveDecay` are
/// also accepted. Any other Geant4 process registered in the simulation can be
/// used with its first letter capitalized. An unknown process name produces
/// a warning and the observable is not filled.
///
/// * **PerProcess<Name>**: If the parameter `perProcessSensitiveEnergy` is
/// set to `true`, the following observables with the energy deposited in the
/// sensitive volume by a given process are added automatically:
/// `PerProcessPhotoelectric` (phot), `PerProcessCompton` (compt),
/// `PerProcessElectronicIoni` (eIoni), `PerProcessAlphaIoni` (ionIoni and
/// alphaIoni of alpha particles), `PerProcessIonIoni` (ionIoni),
/// `PerProcessHadronicIoni` (hIoni), `PerProcessProtonIoni` (hIoni of protons),
/// `PerProcessMsc` (msc), `PerProcessHadronElastic` (hadElastic) and
/// `PerProcessNeutronElastic` (hadElastic of neutrons). These categories may
/// overlap. If `perProcessSensitiveEnergyNorm` is `true` they are normalized
/// to the total energy deposited in the sensitive volume.
///
/// ### Sensitive volume observables
///
/// * **sensitiveVolumeEnergy**: The energy deposited in the sensitive volume.
/// * **sensitiveVolumeFirstHitTime**: The time of the first hit with an energy
/// deposit in the sensitive volume (infinity if there is none).
///
/// The following observables describe the first track outside the sensitive
/// volume which is the ancestor of the tracks depositing energy in the
/// sensitive volume. They are only filled when such a track exists.
///
/// * **firstTrackInSensitiveOk**: 1 if there is exactly one such ancestor
/// track, 0 otherwise.
/// * **firstTrackInSensitiveParticle**: Particle name of the track (string).
/// * **firstTrackInSensitiveParentParticle**: Particle name of its parent
/// track (string, empty if it is a primary).
/// * **firstTrackInSensitiveCreatorProcess**: Process which created the
/// track (string).
/// * **firstTrackInSensitiveEnergy**: Initial kinetic energy of the track.
/// * **firstTrackInSensitivePositionX/Y/Z**: Initial position of the track.
/// * **firstTrackInSensitiveVolumeName**: Volume of the first hit of the
/// track (string).
///
/// String observables must be declared with `type="string"`.
///
/// ### Primary and global observables
///
/// The following list provides observables related to the primary event, as
/// the position, direction or energy of the primary generated.
///
/// * **xOriginPrimary**, **yOriginPrimary**, **zOriginPrimary**: coordinates
/// where the primary event was generated.
/// * **xDirectionPrimary**, **yDirectionPrimary**, **zDirectionPrimary**:
/// components of the momentum direction of the primary event generated.
/// * **thetaPrimary**: polar angle of the primary generated particle.
/// * **phiPrimary**: azimuth angle of the primary generated particle.
/// * **zenithYDegrees**: angle, in degrees, between the primary direction and
/// the -Y axis (zenith angle when Y is the vertical axis).
/// * **zenithSourceDegrees**: angle, in degrees, between the primary direction
/// and the direction of the first particle source defined in TRestGeant4Metadata.
/// * **energyPrimary**: energy of the primary event generated.
/// * **eventPrimaryParticleName**: name of the primary particle (string).
/// * **subEventPrimaryParticleName**: name of the particle which originated
/// the sub-event, or `generator` for the main event (sub-event id 0) (string).
///
/// * **totalEdep**: the total energy deposited in all volumes.
/// * **boundingSize**: It stores a value with the event size calculated
/// as the diagonal distance of a bounding box defined to contain all the
/// hits that produced an energy deposit.
///
/// ### RML example
///
/// \code
/// <TRestGeant4AnalysisProcess name="g4Ana" value="ON" verboseLevel="warning">
///    <parameter name="perProcessSensitiveEnergy" value="false" />
///
///    <observable name="totalEdep" value="ON" description="Total energy deposited in keV" />
///    <observable name="energyPrimary" value="ON" description="Primary energy in keV" />
///    <observable name="eventPrimaryParticleName" type="string" value="ON"
///            description="Name of the primary particle" />
///
///    <observable name="gasVolumeEDep" value="ON" volume="shielding/vessel/gas"
///            description="Energy deposited in the gas in keV" />
///    <observable name="gasComptonProcess" value="ON" volume="shielding/vessel/gas"
///            description="Energy deposited in the gas by Compton scattering in keV" />
///    <observable name="gasHadElasticProcess" value="ON" volume="shielding/vessel/gas"
///            description="Energy deposited in the gas by hadron elastic scattering in keV" />
///
///    <observable name="gammaTracksCounter" value="ON" description="Number of gamma tracks" />
///    <observable name="neutronTracksCounter" value="ON" description="Number of neutron tracks" />
///    <observable name="neutronTracksEDep" value="ON"
///            description="Energy deposited by neutron tracks in keV" />
///
///    <observable name="containsProcessCompt" type="int" value="ON"
///            description="1 if the event contains a Compton scattering" />
///    <observable name="containsProcessHadElastic" type="int" value="ON"
///            description="1 if the event contains a hadron elastic scattering" />
///    <observable name="containsProcessNCapture" type="int" value="ON"
///            description="1 if the event contains a neutron capture" />
/// </TRestGeant4AnalysisProcess>
/// \endcode
///
///--------------------------------------------------------------------------
///
/// RESTsoft - Software for Rare Event Searches with TPCs
///
/// History of developments:
///
/// 2016-March: First implementation of Geant4 analysis process into REST_v2.
///             Javier Galan
///
/// 2017-October: Generic upgrades to add particle and volume observables.
///               Gloria Luzon and Javier Galan
///
/// 2026-October: Implemented `<volume><Process>Process`, generic
///               `containsProcess<Process>` and `PerProcess<Name>` observables,
///               and updated documentation.
///               Alvaro Ezquerro
///
/// \class      TRestGeant4AnalysisProcess
/// \author     Javier Galan
/// \author     Gloria Luzon
///
/// <hr>
///

#include "TRestGeant4AnalysisProcess.h"

#include <algorithm>
#include <tuple>

using namespace std;

ClassImp(TRestGeant4AnalysisProcess);

///////////////////////////////////////////////
/// \brief Default constructor
///
TRestGeant4AnalysisProcess::TRestGeant4AnalysisProcess() { Initialize(); }

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
TRestGeant4AnalysisProcess::TRestGeant4AnalysisProcess(const char* configFilename) {
    Initialize();

    if (LoadConfigFromFile(configFilename)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief Default destructor
///
TRestGeant4AnalysisProcess::~TRestGeant4AnalysisProcess() { delete fOutputG4Event; }

///////////////////////////////////////////////
/// \brief Function to load the default config in absence of RML input
///
void TRestGeant4AnalysisProcess::LoadDefaultConfig() { SetTitle("Default config"); }

///////////////////////////////////////////////
/// \brief Function to initialize input/output event members and define the
/// section name
///
void TRestGeant4AnalysisProcess::Initialize() {
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
/// corresponding TRestGeant4AnalysisProcess section inside the RML.
///
void TRestGeant4AnalysisProcess::LoadConfig(const string& configFilename, const string& name) {
    if (LoadConfigFromFile(configFilename, name)) LoadDefaultConfig();
}

namespace {
string CapitalizeFirst(string name) {
    if (!name.empty()) {
        name[0] = toupper(name[0]);
    }
    return name;
}

bool EndsWith(const string& text, const string& suffix) {
    return text.size() >= suffix.size() &&
           text.compare(text.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool StartsWith(const string& text, const string& prefix) { return text.rfind(prefix, 0) == 0; }
}  // namespace

///////////////////////////////////////////////
/// \brief It returns the map between the process names used in REST observables
/// (`<volume><Process>Process`) and the process names used by Geant4.
///
const map<string, string>& TRestGeant4AnalysisProcess::GetRestToGeant4ProcessNameMap() {
    static const map<string, string> processNameMap = {
        // electromagnetic, photons
        {"Photoelectric", "phot"},
        {"Compton", "compt"},
        {"Rayleigh", "Rayl"},
        {"Conversion", "conv"},
        {"PhotonNuclear", "photonNuclear"},
        // electromagnetic, charged particles
        {"Bremsstrahlung", "eBrem"},
        {"Bremstralung", "eBrem"},  // legacy misspelling
        {"Annihilation", "annihil"},
        {"EIoni", "eIoni"},
        {"HIoni", "hIoni"},
        {"IonIoni", "ionIoni"},
        {"MuIoni", "muIoni"},
        {"Msc", "msc"},
        {"CoulombScat", "CoulombScat"},
        // hadronic
        {"HadElastic", "hadElastic"},
        {"NeutronInelastic", "neutronInelastic"},
        {"NInelastic", "neutronInelastic"},  // legacy name
        {"ProtonInelastic", "protonInelastic"},
        {"NCapture", "nCapture"},
        // decay
        {"RadioactiveDecay", "RadioactiveDecay"},
        {"RadiactiveDecay", "RadioactiveDecay"},  // legacy misspelling
        {"Decay", "Decay"},
    };
    return processNameMap;
}

///////////////////////////////////////////////
/// \brief It returns the Geant4 process name corresponding to `name`.
///
/// `name` can be a REST process name (see GetRestToGeant4ProcessNameMap) or a Geant4
/// process name with the first letter capitalized (e.g. `Compt`, `HadElastic` or `NCapture`).
/// It returns an empty string if no match is found.
///
string TRestGeant4AnalysisProcess::GetGeant4ProcessName(const string& name) const {
    if (name.empty()) {
        return "";
    }

    const auto& processNameMap = GetRestToGeant4ProcessNameMap();
    const auto it = processNameMap.find(name);
    if (it != processNameMap.end()) {
        return it->second;
    }

    for (const auto& [restName, geant4Name] : processNameMap) {
        if (CapitalizeFirst(geant4Name) == name) {
            return geant4Name;
        }
    }

    if (fG4Metadata != nullptr) {
        for (const auto& geant4Name : fG4Metadata->GetGeant4PhysicsInfo().GetAllProcesses()) {
            if (CapitalizeFirst(geant4Name.Data()) == name) {
                return geant4Name.Data();
            }
        }
    }

    return "";
}

///////////////////////////////////////////////
/// \brief It prints a warning when the volume used by an observable is not an active volume.
///
void TRestGeant4AnalysisProcess::PrintNotActiveVolumeWarning(const TString& volumeName) {
    cout << endl;
    cout << "??????????????????????????????????????????????????" << endl;
    cout << "REST warning : TRestGeant4AnalysisProcess." << endl;
    cout << "------------------------------------------" << endl;
    cout << endl;
    cout << " Volume " << volumeName << " is not an active volume" << endl;
    cout << endl;
    cout << "List of active volumes : " << endl;
    cout << "------------------------ " << endl;

    for (unsigned int n = 0; n < fG4Metadata->GetNumberOfActiveVolumes(); n++)
        cout << "Volume " << n << " : " << fG4Metadata->GetActiveVolumeName(n) << endl;
    cout << "??????????????????????????????????????????????????" << endl;
    cout << endl;
}

///////////////////////////////////////////////
/// \brief Process initialization. Observable names are interpreted and auxiliar
/// observable members, related to VolumeEdep, MeanPos, TracksCounter, TrackEDep,
/// `<volume><Process>Process`, containsProcess and PerProcess observables defined
/// in TRestGeant4AnalysisProcess are filled at this stage.
///
void TRestGeant4AnalysisProcess::InitProcess() {
    fG4Metadata = GetMetadata<TRestGeant4Metadata>();

    // InitProcess may be called more than once
    fEnergyInObservables.clear();
    fVolumeID.clear();
    fVolumeName.clear();
    fMeanPosObservables.clear();
    fVolumeID2.clear();
    fDirID.clear();
    fProcessObservables.clear();
    fVolumeID3.clear();
    fProcessVolumeName.clear();
    fProcessName.clear();
    fTrackCounterObservables.clear();
    fParticleTrackCounter.clear();
    fTracksEDepObservables.clear();
    fParticleTrackEdep.clear();
    fContainsProcessObservables.clear();
    fContainsProcessG4Name.clear();
    fPerProcessObservables.clear();
    fPerProcessG4Names.clear();
    fPerProcessParticle.clear();

    std::vector<string> fObservables;
    fObservables = TRestEventProcess::ReadObservables();
    std::map<std::string, std::string> aliasObsToVol = GetAliasObservableNameToVolume();

    const auto& physicsInfo = fG4Metadata->GetGeant4PhysicsInfo();
    const auto availableProcesses = physicsInfo.GetAllProcesses();

    // containsProcessPhot and containsProcessCompt are always evaluated (historical behaviour)
    fContainsProcessObservables = {"containsProcessPhot", "containsProcessCompt"};
    fContainsProcessG4Name = {"phot", "compt"};

    if (fPerProcessSensitiveEnergy) {
        // {observable name, particle (empty for any), Geant4 processes}
        const vector<tuple<string, string, vector<string>>> perProcessDefinitions = {
            {"PerProcessPhotoelectric", "", {"phot"}},
            {"PerProcessCompton", "", {"compt"}},
            {"PerProcessElectronicIoni", "", {"eIoni"}},
            {"PerProcessAlphaIoni", "alpha", {"ionIoni", "alphaIoni"}},
            {"PerProcessIonIoni", "", {"ionIoni"}},
            {"PerProcessHadronicIoni", "", {"hIoni"}},
            {"PerProcessProtonIoni", "proton", {"hIoni"}},
            {"PerProcessMsc", "", {"msc"}},
            {"PerProcessHadronElastic", "", {"hadElastic"}},
            {"PerProcessNeutronElastic", "neutron", {"hadElastic"}},
        };
        for (const auto& [obsName, particle, processes] : perProcessDefinitions) {
            fPerProcessObservables.push_back(obsName);
            fPerProcessParticle.push_back(particle);
            fPerProcessG4Names.push_back(processes);
            if (fAnalysisTree != nullptr) {
                fAnalysisTree->AddObservable((string)GetName() + "_" + obsName, "double",
                                             "Energy deposited in the sensitive volume by a given process");
            }
        }
    }

    for (unsigned int i = 0; i < fObservables.size(); i++) {
        if (fObservables[i].find("VolumeEDep") != string::npos) {
            TString volName = fObservables[i].substr(0, fObservables[i].length() - 10).c_str();
            if (aliasObsToVol.find(fObservables[i]) != aliasObsToVol.end()) {
                volName = aliasObsToVol[fObservables[i]].c_str();
            }

            Int_t volId = fG4Metadata->GetActiveVolumeID(volName);
            if (volId >= 0) {
                fEnergyInObservables.push_back(fObservables[i]);
                fVolumeID.push_back(volId);
                fVolumeName.push_back(volName.Data());
            }

            if (volId == -1) {
                PrintNotActiveVolumeWarning(volName);
            }
        }

        if (fObservables[i].find("MeanPos") != string::npos) {
            TString volName2 = fObservables[i].substr(0, fObservables[i].length() - 8).c_str();
            if (aliasObsToVol.find(fObservables[i]) != aliasObsToVol.end()) {
                volName2 = aliasObsToVol[fObservables[i]].c_str();
            }
            std::string dirId = fObservables[i].substr(fObservables[i].length() - 1, 1);

            Int_t volId2 = fG4Metadata->GetActiveVolumeID(volName2);
            if (volId2 >= 0) {
                fMeanPosObservables.push_back(fObservables[i]);
                fVolumeID2.push_back(volId2);
                fDirID.push_back(dirId);
            }

            if (volId2 == -1) {
                PrintNotActiveVolumeWarning(volName2);
            }

            if ((dirId != "X") && (dirId != "Y") && (dirId != "Z")) {
                cout << endl;
                cout << "??????????????????????????????????????????????????" << endl;
                cout << "REST warning : TRestGeant4AnalysisProcess." << endl;
                cout << "------------------------------------------" << endl;
                cout << endl;
                cout << " Direction " << dirId << " is not valid" << endl;
                cout << " Only X, Y or Z accepted" << endl;
                cout << endl;
            }
        }

        if (StartsWith(fObservables[i], "containsProcess")) {
            const string processSuffix = fObservables[i].substr(string("containsProcess").length());
            if (std::find(fContainsProcessObservables.begin(), fContainsProcessObservables.end(),
                          fObservables[i]) != fContainsProcessObservables.end()) {
                continue;
            }
            const string geant4Name = GetGeant4ProcessName(processSuffix);
            if (geant4Name.empty()) {
                RESTWarning << "TRestGeant4AnalysisProcess: unknown process '" << processSuffix
                            << "' in observable '" << fObservables[i] << "'. It will not be filled."
                            << RESTendl;
                continue;
            }
            if (availableProcesses.count(geant4Name) == 0) {
                RESTWarning << "TRestGeant4AnalysisProcess: Geant4 process '" << geant4Name
                            << "' (observable '" << fObservables[i]
                            << "') is not registered in the physics info of this simulation. "
                            << "Its value will always be 0 (processes sharing type and subtype, e.g. "
                            << "eIoni and ionIoni, are registered only once)" << RESTendl;
            }
            fContainsProcessObservables.push_back(fObservables[i]);
            fContainsProcessG4Name.push_back(geant4Name);
            continue;
        }

        // <volume><Process>Process observables. Other observables ending in "Process" are excluded.
        if (EndsWith(fObservables[i], "Process") && !StartsWith(fObservables[i], "firstTrackInSensitive")) {
            const string nameWithoutSuffix =
                fObservables[i].substr(0, fObservables[i].length() - string("Process").length());
            const bool hasAlias = aliasObsToVol.find(fObservables[i]) != aliasObsToVol.end();

            // find the longest trailing substring which is a known process name
            string processName;
            string geant4Name;
            for (size_t length = 1; length <= nameWithoutSuffix.length(); length++) {
                // the volume name may only be empty if it is given through the `volume` attribute
                if (length == nameWithoutSuffix.length() && !hasAlias) {
                    break;
                }
                const string candidate = nameWithoutSuffix.substr(nameWithoutSuffix.length() - length);
                const string candidateGeant4Name = GetGeant4ProcessName(candidate);
                if (!candidateGeant4Name.empty()) {
                    processName = candidate;
                    geant4Name = candidateGeant4Name;
                }
            }

            if (geant4Name.empty()) {
                RESTWarning << "TRestGeant4AnalysisProcess: no known process name found in observable '"
                            << fObservables[i] << "'. It will not be filled." << RESTendl;
                RESTWarning << "Valid process names are:";
                for (const auto& [restName, g4Name] : GetRestToGeant4ProcessNameMap()) {
                    RESTWarning << " " << restName;
                }
                RESTWarning << " (or a Geant4 process name with the first letter capitalized)" << RESTendl;
                continue;
            }

            TString volName3 = nameWithoutSuffix.substr(0, nameWithoutSuffix.length() - processName.length());
            if (hasAlias) {
                volName3 = aliasObsToVol[fObservables[i]].c_str();
            }
            Int_t volId3 = fG4Metadata->GetActiveVolumeID(volName3);

            if (volId3 >= 0) {
                fProcessObservables.push_back(fObservables[i]);
                fVolumeID3.push_back(volId3);
                fProcessVolumeName.emplace_back(volName3.Data());
                fProcessName.push_back(geant4Name);
                if (availableProcesses.count(geant4Name) == 0) {
                    RESTWarning << "TRestGeant4AnalysisProcess: Geant4 process '" << geant4Name
                                << "' (observable '" << fObservables[i]
                                << "') is not registered in the physics info of this simulation. "
                                << "It will be 0 unless energy is deposited under this process name "
                                << "(processes sharing type and subtype, e.g. eIoni and ionIoni, are "
                                << "registered only once)" << RESTendl;
                }
            } else {
                PrintNotActiveVolumeWarning(volName3);
            }
        }

        if (fObservables[i].find("TracksCounter") != string::npos) {
            TString particleName = fObservables[i].substr(0, fObservables[i].length() - 13).c_str();
            fTrackCounterObservables.push_back(fObservables[i]);
            fParticleTrackCounter.emplace_back(particleName.Data());
        }

        if (fObservables[i].find("TracksEDep") != string::npos) {
            TString particleName = fObservables[i].substr(0, fObservables[i].length() - 10).c_str();
            fTracksEDepObservables.push_back(fObservables[i]);
            fParticleTrackEdep.emplace_back(particleName.Data());
        }
    }
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestGeant4AnalysisProcess::ProcessEvent(TRestEvent* inputEvent) {
    fInputG4Event = (TRestGeant4Event*)inputEvent;
    *fOutputG4Event = *((TRestGeant4Event*)inputEvent);

    const auto sensitiveVolumeName = fG4Metadata->GetSensitiveVolume();

    Double_t sensitiveVolumeEnergy = fOutputG4Event->GetEnergyInVolume(sensitiveVolumeName.Data());
    // Get time of the first hit in the sensitive volume
    double hitTime = std::numeric_limits<double>::infinity();
    std::set<int> trackIdsInSensitiveVolume;

    for (const auto& track : fOutputG4Event->GetTracks()) {
        const auto& hits = track.GetHits();
        for (size_t hitIndex = 0; hitIndex < hits.GetNumberOfHits(); hitIndex++) {
            const auto volumeName = hits.GetVolumeName(hitIndex);
            if (volumeName != sensitiveVolumeName) {
                continue;
            }
            const double energy = hits.GetEnergy(hitIndex);
            if (energy <= 0) {
                continue;
            }

            const double time = hits.GetTime(hitIndex);
            if (time < hitTime) {
                hitTime = time;
            }

            trackIdsInSensitiveVolume.insert(track.GetTrackID());
        }
    }

    SetObservableValue("sensitiveVolumeFirstHitTime", hitTime);

    std::set<int> trackParentsOfInterest;
    for (const auto& trackIdInSensitiveVolume : trackIdsInSensitiveVolume) {
        const auto track = fOutputG4Event->GetTrackByID(trackIdInSensitiveVolume);
        TRestGeant4Track* parent = track;
        bool found = false;
        while (parent != nullptr && !found) {
            // iterate over hits
            const auto& hits = parent->GetHits();
            if (hits.GetVolumeName(0) != sensitiveVolumeName) {
                for (size_t hitIndex = 0; hitIndex < hits.GetNumberOfHits(); hitIndex++) {
                    const auto volumeName = hits.GetVolumeName(hitIndex);
                    if (volumeName != sensitiveVolumeName) {
                        found = true;
                        trackParentsOfInterest.insert(parent->GetTrackID());
                        break;
                    }
                }
            }

            parent = parent->GetParentTrack();
        }
    }

    if (!trackParentsOfInterest.empty()) {
        const auto track = fOutputG4Event->GetTrackByID(*trackParentsOfInterest.begin());

        const auto position = track->GetInitialPosition();
        SetObservableValue("firstTrackInSensitivePositionX", position.X());
        SetObservableValue("firstTrackInSensitivePositionY", position.Y());
        SetObservableValue("firstTrackInSensitivePositionZ", position.Z());

        const auto energy = track->GetInitialKineticEnergy();
        SetObservableValue("firstTrackInSensitiveEnergy", energy);

        const string particleName = track->GetParticleName().Data();
        SetObservableValue("firstTrackInSensitiveParticle", particleName);

        string parentParticleName;
        if (track->GetParentTrack() != nullptr) {
            parentParticleName = track->GetParentTrack()->GetParticleName().Data();
        }
        SetObservableValue("firstTrackInSensitiveParentParticle", parentParticleName);

        const string creatorProcess = track->GetCreatorProcess().Data();
        SetObservableValue("firstTrackInSensitiveCreatorProcess", creatorProcess);

        const string volumeName = track->GetHits().GetVolumeName(0).Data();
        SetObservableValue("firstTrackInSensitiveVolumeName", volumeName);
    }

    SetObservableValue("firstTrackInSensitiveOk", trackParentsOfInterest.size() == 1);

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug) {
        cout << "----------------------------" << endl;
        cout << "TRestGeant4Event : " << fOutputG4Event->GetID() << endl;
        cout << "Sensitive volume Energy : " << sensitiveVolumeEnergy << endl;
        cout << "Total energy : " << fOutputG4Event->GetTotalDepositedEnergy() << endl;
    }

    SetObservableValue("sensitiveVolumeEnergy", sensitiveVolumeEnergy);

    Double_t xOrigin = fOutputG4Event->GetPrimaryEventOrigin().X();
    SetObservableValue("xOriginPrimary", xOrigin);

    Double_t yOrigin = fOutputG4Event->GetPrimaryEventOrigin().Y();
    SetObservableValue("yOriginPrimary", yOrigin);

    Double_t zOrigin = fOutputG4Event->GetPrimaryEventOrigin().Z();
    SetObservableValue("zOriginPrimary", zOrigin);

    Double_t xDirection = fOutputG4Event->GetPrimaryEventDirection(0).X();
    SetObservableValue("xDirectionPrimary", xDirection);

    Double_t yDirection = fOutputG4Event->GetPrimaryEventDirection(0).Y();
    SetObservableValue("yDirectionPrimary", yDirection);

    Double_t zDirection = fOutputG4Event->GetPrimaryEventDirection(0).Z();
    SetObservableValue("zDirectionPrimary", zDirection);

    SetObservableValue("eventPrimaryParticleName", fOutputG4Event->GetPrimaryEventParticleName(0));
    string subEventPrimaryParticleName;
    if (fOutputG4Event->GetSubID() != 0) {
        subEventPrimaryParticleName = fOutputG4Event->GetSubEventPrimaryEventParticleName();
    } else {
        subEventPrimaryParticleName = "generator";
    }
    SetObservableValue("subEventPrimaryParticleName", subEventPrimaryParticleName);

    TVector3 primaryDirection(xDirection, yDirection, zDirection);
    primaryDirection = primaryDirection.Unit();
    SetObservableValue("thetaPrimary", primaryDirection.Theta());
    SetObservableValue("phiPrimary", primaryDirection.Phi());
    SetObservableValue("zenithYDegrees", TMath::ACos(-1 * yDirection) * TMath::RadToDeg());
    TVector3 sourceDirection = fG4Metadata->GetParticleSource(0)->GetDirection();
    sourceDirection = sourceDirection.Unit();
    SetObservableValue("zenithSourceDegrees",
                       TMath::ACos(primaryDirection.Dot(sourceDirection)) * TMath::RadToDeg());

    Double_t energyPrimary = fOutputG4Event->GetPrimaryEventEnergy(0);
    SetObservableValue("energyPrimary", energyPrimary);

    Double_t energyTotal = fOutputG4Event->GetTotalDepositedEnergy();
    SetObservableValue("totalEdep", energyTotal);

    Double_t size = fOutputG4Event->GetBoundingBoxSize();
    SetObservableValue("boundingSize", size);

    // containsProcessXxx observables: 1 if the event contains the Geant4 process, 0 otherwise
    const auto& physicsInfo = fG4Metadata->GetGeant4PhysicsInfo();
    for (unsigned int n = 0; n < fContainsProcessObservables.size(); n++) {
        const auto& processName = fContainsProcessG4Name[n];
        const Int_t processID = physicsInfo.GetProcessID(processName);
        // GetProcessID returns a default id (0) for unknown processes, which may be a valid id
        const bool processIsRegistered = physicsInfo.GetProcessName(processID) == processName;
        Int_t containsProcess = 0;
        if (processIsRegistered && fOutputG4Event->ContainsProcess(processID)) {
            containsProcess = 1;
        }
        SetObservableValue(fContainsProcessObservables[n], containsProcess);
    }

    if (!fProcessObservables.empty()) {
        const auto energyInVolumePerProcess = fOutputG4Event->GetEnergyInVolumePerProcessMap();
        for (unsigned int n = 0; n < fProcessObservables.size(); n++) {
            Double_t energy = 0;
            const auto volume = energyInVolumePerProcess.find(fProcessVolumeName[n]);
            if (volume != energyInVolumePerProcess.end()) {
                const auto process = volume->second.find(fProcessName[n]);
                if (process != volume->second.end()) {
                    energy = process->second;
                }
            }
            SetObservableValue(fProcessObservables[n], energy);
        }
    }

    if (!fPerProcessObservables.empty()) {
        const auto energyInVolumePerParticlePerProcess =
            fOutputG4Event->GetEnergyInVolumePerParticlePerProcessMap();
        const auto sensitiveVolume = energyInVolumePerParticlePerProcess.find(sensitiveVolumeName.Data());
        for (unsigned int n = 0; n < fPerProcessObservables.size(); n++) {
            Double_t energy = 0;
            if (sensitiveVolume != energyInVolumePerParticlePerProcess.end()) {
                for (const auto& [particle, energyPerProcess] : sensitiveVolume->second) {
                    if (!fPerProcessParticle[n].empty() && particle != fPerProcessParticle[n]) {
                        continue;
                    }
                    for (const auto& processName : fPerProcessG4Names[n]) {
                        const auto process = energyPerProcess.find(processName);
                        if (process != energyPerProcess.end()) {
                            energy += process->second;
                        }
                    }
                }
            }
            if (fPerProcessSensitiveEnergyNorm) {
                energy = sensitiveVolumeEnergy > 0 ? energy / sensitiveVolumeEnergy : 0;
            }
            SetObservableValue(fPerProcessObservables[n], energy);
        }
    }

    for (unsigned int n = 0; n < fParticleTrackCounter.size(); n++) {
        Int_t nT = fOutputG4Event->GetNumberOfTracksForParticle(fParticleTrackCounter[n]);
        string obsName = fTrackCounterObservables[n];
        SetObservableValue(obsName, nT);
    }

    for (unsigned int n = 0; n < fTracksEDepObservables.size(); n++) {
        Double_t energy = fOutputG4Event->GetEnergyDepositedByParticle(fParticleTrackEdep[n]);
        string obsName = fTracksEDepObservables[n];
        SetObservableValue(obsName, energy);
    }

    for (unsigned int n = 0; n < fEnergyInObservables.size(); n++) {
        Double_t en = fOutputG4Event->GetEnergyInVolume(fVolumeName[n]);
        string obsName = fEnergyInObservables[n];
        SetObservableValue(obsName, en);
    }

    for (unsigned int n = 0; n < fMeanPosObservables.size(); n++) {
        string obsName = fMeanPosObservables[n];

        Double_t mpos = 0;
        if (fDirID[n] == (TString) "X")
            mpos = fOutputG4Event->GetMeanPositionInVolume(fVolumeID2[n]).X();

        else if (fDirID[n] == (TString) "Y")
            mpos = fOutputG4Event->GetMeanPositionInVolume(fVolumeID2[n]).Y();

        else if (fDirID[n] == (TString) "Z")
            mpos = fOutputG4Event->GetMeanPositionInVolume(fVolumeID2[n]).Z();

        SetObservableValue(obsName, mpos);
    }

    if (GetVerboseLevel() >= TRestStringOutput::REST_Verbose_Level::REST_Debug) {
        cout << "G4 Tracks : " << fOutputG4Event->GetNumberOfTracks() << endl;
        cout << "----------------------------" << endl;
    }

    return fOutputG4Event;
}

std::map<std::string, std::string> TRestGeant4AnalysisProcess::GetAliasObservableNameToVolume() {
    TiXmlElement* e = GetElement("observable");
    std::map<std::string, std::string> obsNamesToVolumes;

    while (e != nullptr) {
        const char* obschr = e->Attribute("name");
        const char* _value = e->Attribute("value");
        const char* _vol = e->Attribute("volume");

        string value;
        if (_value == nullptr)
            value = "ON";
        else
            value = _value;

        if (ToUpper(value) == "ON") {
            if (obschr != nullptr) {
                string volume;
                if (_vol) {
                    volume = _vol;
                    obsNamesToVolumes[(string)obschr] = volume;
                }
            }
        }

        e = e->NextSiblingElement("observable");
    }

    return obsNamesToVolumes;
}

///////////////////////////////////////////////
/// \brief Function to include required actions after all events have been processed.
void TRestGeant4AnalysisProcess::EndProcess() {}
