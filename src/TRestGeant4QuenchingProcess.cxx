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
/// The TRestGeant4QuenchingProcess generates observables based on veto volumes energy
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
///         <addProcess type="TRestGeant4QuenchingProcess" name="g4Neutrons" value="ON"
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
/// 2022-September: Implementation.
///
/// \class      TRestGeant4QuenchingProcess
/// \author     Luis Obis
///
/// <hr>
///

#include "TRestGeant4QuenchingProcess.h"

using namespace std;

ClassImp(TRestGeant4QuenchingProcess);

TRestGeant4QuenchingProcess::TRestGeant4QuenchingProcess() { Initialize(); }

TRestGeant4QuenchingProcess::TRestGeant4QuenchingProcess(const char* configFilename) {
    Initialize();
    if (LoadConfigFromFile(configFilename)) LoadDefaultConfig();
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
    if (LoadConfigFromFile(configFilename, name)) LoadDefaultConfig();
}

///////////////////////////////////////////////
/// \brief The main processing event function
///
TRestEvent* TRestGeant4QuenchingProcess::ProcessEvent(TRestEvent* inputEvent) {
    fInputG4Event = (TRestGeant4Event*)inputEvent;
    *fOutputG4Event = *((TRestGeant4Event*)inputEvent);

    for (int i = 0; i < fOutputG4Event->GetNumberOfTracks(); i++) {
        TRestGeant4Track* trackPointer = fOutputG4Event->GetTrackPointer(i);
        const auto particleName = trackPointer->GetParticleName();
        if (fParticlesNotQuenched.count(particleName) > 0) {
            continue;
        }
        TRestGeant4Hits* hitPointer = trackPointer->GetHitsPointer();
        for (int j = 0; j < hitPointer->GetNumberOfHits(); j++) {
            hitPointer->fEnergy[j] *= fQuenchingFactor;
            hitPointer->fTotalEnergy *= fQuenchingFactor;
        }
    }

    return fOutputG4Event;
}

///////////////////////////////////////////////
/// \brief Function to read input parameters from the RML
/// TRestGeant4QuenchingProcess metadata section
///
void TRestGeant4QuenchingProcess::InitFromConfigFile() {
    SetQuenchingFactor(GetDblParameterWithUnits("quenchingFactor", fQuenchingFactor));
}

void TRestGeant4QuenchingProcess::SetQuenchingFactor(Double_t quenchingFactor) {
    if (quenchingFactor > 1.0) {
        quenchingFactor = 1.0;
    }
    if (quenchingFactor < 0.0) {
        quenchingFactor = 0.0;
    }
    fQuenchingFactor = quenchingFactor;
}
