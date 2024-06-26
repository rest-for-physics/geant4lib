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
/// The TRestGeant4QuenchingProcess modifies the energy deposits of the simulation by multiplying by a
/// user-defined quenching factor. This quenching factor is specified by volume and by particle and has the
/// value 1.0 by default (no change). Physical volumes, logical volumes, or expressions (to be matched to
/// either physical or logical) can be used for volumes. Example usage: \code
///< TRestGeant4QuenchingProcess>
///
///    <volume name="^scintillatorVolume">
///    <particle name="gamma" quenchingFactor="1.0"/>
///    <particle name="e-" quenchingFactor="0.5"/>
///    <particle name="e+" quenchingFactor="0.9"/>
///    <particle name="mu-" quenchingFactor="0.2"/>
///    <particle name="neutron" quenchingFactor="0.1"/>
///    </volume>
///
///    <volume name="gasVolume">
///    <particle name="gamma" quenchingFactor="0.2"/>
///    <particle name="e-" quenchingFactor="0.2"/>
///    </volume>
///
///    </TRestGeant4QuenchingProcess>
/// \endcode
///

#include "TRestGeant4QuenchingProcess.h"

using namespace std;

ClassImp(TRestGeant4QuenchingProcess);

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
    TiXmlElement* volumeElement = GetElement("volume");
    while (volumeElement) {
        const string volumeName = GetParameter("name", volumeElement, "");
        if (volumeName.empty()) {
            cerr << "TRestGeant4QuenchingProcess: No volume expression specified" << endl;
            exit(1);
        }
        fUserVolumeExpressions.insert(volumeName);
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

    const auto geometryInfo = fGeant4Metadata->GetGeant4GeometryInfo();
    // check all the user volume expressions are valid and correspond to at least a volume
    for (const auto& userVolume : fUserVolumeExpressions) {
        set<string> physicalVolumes = {};
        for (const auto& volume : geometryInfo.GetAllPhysicalVolumesMatchingExpression(userVolume)) {
            physicalVolumes.insert(volume.Data());
        }
        if (!physicalVolumes.empty()) {
            continue;
        }
        // maybe it refers to a logical volume
        for (const auto& logicalVolume : geometryInfo.GetAllLogicalVolumesMatchingExpression(userVolume)) {
            for (const auto& volume : geometryInfo.GetAllPhysicalVolumesFromLogical(logicalVolume.Data())) {
                physicalVolumes.insert(volume.Data());
            }
        }

        if (physicalVolumes.empty()) {
            RESTWarning << "TRestGeant4QuenchingProcess: No volume found matching expression: " << userVolume
                        << RESTendl;
        }

        for (const auto& physicalVolume : physicalVolumes) {
            const auto volumeName = geometryInfo.GetAlternativeNameFromGeant4PhysicalName(physicalVolume);
            fVolumes.insert(volumeName.Data());
        }
    }

    RESTDebug << "TRestGeant4QuenchingProcess initialized with volumes" << RESTendl;
    for (const auto& volume : fVolumes) {
        RESTDebug << " " << volume << RESTendl;
    }
}

double QuenchingFactor(double recoilEnergy, int A, int Z) {
    // Lindhard formula
    double gamma = 11.5 * recoilEnergy * TMath::Power(Z, -7.0 / 3.0);
    double g = 3 * TMath::Power(gamma, 0.15) + 0.7 * TMath::Power(gamma, 0.6) + gamma;
    double k = 0.133 * TMath::Power(Z, 2.0 / 3.0) * TMath::Power(A, -1.0 / 2.0);
    return k * g / (1 + k * g);
}
///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestGeant4QuenchingProcess::ProcessEvent(TRestEvent* inputEvent) {
    fInputG4Event = (TRestGeant4Event*)inputEvent;
    *fOutputG4Event = *((TRestGeant4Event*)inputEvent);

    const double sensitiveVolumeEnergyBefore = fOutputG4Event->GetSensitiveVolumeEnergy();

    fOutputG4Event->InitializeReferences(GetRunInfo());
    fOutputG4Event->fEnergyInVolumePerParticlePerProcess.clear();

    // loop over all tracks
    for (int trackIndex = 0; trackIndex < int(fOutputG4Event->GetNumberOfTracks()); trackIndex++) {
        // get the track
        TRestGeant4Track* track = fOutputG4Event->GetTrackPointer(trackIndex);
        const auto& particleName = track->GetParticleName();

        auto hits = track->GetHitsPointer();
        if (!hits->GetHadronicOk()) {
            cerr << "TRestGeant4QuenchingProcess: Hadronic information not available. Use the "
                    "'storeHadronicTargetInfo' parameter in the restG4 configuration"
                 << endl;
            exit(1);
        }
        auto& energy = hits->GetEnergyRef();
        for (int hitIndex = 0; hitIndex < int(hits->GetNumberOfHits()); hitIndex++) {
            const auto& volumeName = hits->GetVolumeName(hitIndex);

            const string isotopeName = hits->GetHadronicTargetIsotopeName(hitIndex);
            const int isotopeA = hits->GetHadronicTargetIsotopeA(hitIndex);
            const int isotopeZ = hits->GetHadronicTargetIsotopeZ(hitIndex);

            double recoilEnergy = hits->GetEnergy(hitIndex);

            double quenchingFactor = 1.0;
            if (fVolumes.count(volumeName.Data()) && recoilEnergy > 0 && !isotopeName.empty()) {
                quenchingFactor = QuenchingFactor(recoilEnergy, isotopeA, isotopeZ);
            }

            const auto processName = hits->GetProcessName(hitIndex);

            if (energy[hitIndex] > 0) {
                fOutputG4Event->fEnergyInVolumePerParticlePerProcess[volumeName.Data()][particleName.Data()]
                                                                    [processName.Data()] +=
                    energy[hitIndex] * quenchingFactor;
            }
        }
    }

    const double sensitiveVolumeEnergyAfter =
        fOutputG4Event->GetEnergyInVolume(fGeant4Metadata->GetSensitiveVolume().Data());

    bool sensitiveQuenched = TMath::Abs(sensitiveVolumeEnergyAfter - sensitiveVolumeEnergyBefore) > 1e-2;
    SetObservableValue("sensitiveQuenched", sensitiveQuenched);
    SetObservableValue("sensitiveVolumeEnergyBefore", sensitiveVolumeEnergyBefore);
    SetObservableValue("sensitiveVolumeEnergyAfter", sensitiveVolumeEnergyAfter);

    return fOutputG4Event;
}

void TRestGeant4QuenchingProcess::EndProcess() {}

void TRestGeant4QuenchingProcess::PrintMetadata() {
    BeginPrintProcess();
    for (auto const& volume : fVolumes) {
        RESTMetadata << "Volume: " << volume << RESTendl;
    }
    EndPrintProcess();
}

std::set<std::string> TRestGeant4QuenchingProcess::GetVolumes() const { return fVolumes; }

std::set<std::string> TRestGeant4QuenchingProcess::GetUserVolumeExpressions() const {
    return fUserVolumeExpressions;
}
