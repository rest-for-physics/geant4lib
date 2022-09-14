///_______________________________________________________________________________
///_______________________________________________________________________________
///_______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4Track.cxx
///
///             Base class from which to inherit all other event classes in REST
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#include "TRestGeant4Track.h"

#include "TRestGeant4Event.h"
#include "TRestGeant4Metadata.h"

using namespace std;

ClassImp(TRestGeant4Track);

TRestGeant4Track::TRestGeant4Track() = default;

TRestGeant4Track::~TRestGeant4Track() = default;

Int_t TRestGeant4Track::GetProcessID(const TString& processName) const {
    const TRestGeant4Metadata* metadata = GetGeant4Metadata();
    if (metadata != nullptr) {
        const auto processID = metadata->GetGeant4PhysicsInfo().GetProcessID(processName);
        if (processID != Int_t{}) {
            return processID;
        }
    }

    cout << "WARNING : The process " << processName << " was not found" << endl;
    return -1;
}

TString TRestGeant4Track::GetProcessName(Int_t processID) const {
    const TRestGeant4Metadata* metadata = GetGeant4Metadata();
    if (metadata != nullptr) {
        const auto& processName = metadata->GetGeant4PhysicsInfo().GetProcessName(processID);
        if (processName != TString{}) {
            return processName;
        }
    }

    cout << "WARNING : The process " << processID << " was not found" << endl;

    return "";
}

EColor TRestGeant4Track::GetParticleColor() const {
    EColor color = kGray;

    if (GetParticleName() == "e-")
        color = kRed;
    else if (GetParticleName() == "e+")
        color = kBlue;
    else if (GetParticleName() == "alpha")
        color = kOrange;
    else if (GetParticleName() == "mu-")
        color = kViolet;
    else if (GetParticleName() == "gamma")
        color = kGreen;
    else
        cout << "TRestGeant4Track::GetParticleColor. Particle NOT found! Returning "
                "gray color."
             << endl;

    return color;
}

///////////////////////////////////////////////
/// \brief Function that returns the number of hit depositions found inside
/// the TRestGeant4Track. If a specific volume id is given as argument only
/// the hits of that specific volume will be counted.
///
size_t TRestGeant4Track::GetNumberOfHits(Int_t volID) const {
    size_t numberOfHits = 0;
    for (int n = 0; n < fHits.GetNumberOfHits(); n++) {
        if (volID != -1 && fHits.GetVolumeId(n) != volID) {
            continue;
        }
        numberOfHits++;
    }
    return numberOfHits;
}

///////////////////////////////////////////////
/// \brief Function that returns the number of hit depositions found inside
/// the TRestGeant4Track with energy > 0. If a specific volume id is given as argument only
/// the hits of that specific volume will be counted.
///
size_t TRestGeant4Track::GetNumberOfPhysicalHits(Int_t volID) const {
    size_t numberOfHits = 0;
    for (int n = 0; n < fHits.GetNumberOfHits(); n++) {
        if (volID != -1 && fHits.GetVolumeId(n) != volID) {
            continue;
        }
        if (fHits.GetEnergy(n) <= 0) {
            continue;
        }
        numberOfHits++;
    }
    return numberOfHits;
}

void TRestGeant4Track::PrintTrack(size_t maxHits) const {
    cout
        << " * TrackID: " << fTrackID << " - Particle: " << fParticleName << " - ParentID: " << fParentID
        << ""
        << (GetParentTrack() != nullptr
                ? TString::Format(" - Parent particle: %s", GetParentTrack()->GetParticleName().Data()).Data()
                : "")
        << " - Created by '" << fCreatorProcess
        << "' with initial "
           "KE of "
        << ToEnergyString(fInitialKineticEnergy) << "" << endl;

    cout << "   Initial position " << VectorToString(fInitialPosition) << " mm at time "
         << ToTimeString(fGlobalTimestamp) << " - Time length of " << ToTimeString(fTimeLength)
         << " and spatial length of " << ToLengthString(fLength) << endl;

    size_t nHits = GetNumberOfHits();
    if (maxHits > 0 && maxHits < nHits) {
        nHits = min(maxHits, nHits);
        cout << "Printing only the first " << nHits << " hits of the track" << endl;
    }

    const TRestGeant4Metadata* metadata = GetGeant4Metadata();
    for (int i = 0; i < nHits; i++) {
        TString processName = GetProcessName(fHits.GetHitProcess(i));
        if (processName.IsNull()) {
            processName =
                TString(std::to_string(fHits.GetHitProcess(i)));  // in case process name is not found, use ID
        }

        TString volumeName = "";
        if (metadata != nullptr) {
            volumeName = metadata->GetGeant4GeometryInfo().GetVolumeFromID(fHits.GetHitVolume(i));
        }
        if (volumeName.IsNull()) {
            // in case process name is not found, use ID
            volumeName = TString(std::to_string(fHits.GetHitVolume(i)));
        }
        cout << "      - Hit " << i << " - Energy: " << ToEnergyString(fHits.GetEnergy(i))
             << " - Process: " << processName << " - Volume: " << volumeName
             << " - Position: " << VectorToString(TVector3(fHits.GetX(i), fHits.GetY(i), fHits.GetZ(i)))
             << " mm - Time: " << ToTimeString(fHits.GetTime(i))
             << " - KE: " << ToEnergyString(fHits.GetKineticEnergy(i)) << endl;
    }
}

Bool_t TRestGeant4Track::ContainsProcessInVolume(Int_t processID, Int_t volumeID) const {
    for (int i = 0; i < GetNumberOfHits(); i++) {
        if (fHits.GetHitProcess(i) != processID) continue;
        if (volumeID == -1 || fHits.GetVolumeId(i) == volumeID) return true;
    }
    return false;
}

Bool_t TRestGeant4Track::ContainsProcessInVolume(const TString& processName, Int_t volumeID) const {
    const TRestGeant4Metadata* metadata = GetGeant4Metadata();
    if (metadata == nullptr) {
        return false;
    }
    const auto& processID = metadata->GetGeant4PhysicsInfo().GetProcessID(processName);
    for (int i = 0; i < GetNumberOfHits(); i++) {
        if (fHits.GetHitProcess(i) != processID) continue;
        if (volumeID == -1 || fHits.GetVolumeId(i) == volumeID) return true;
    }
    return false;
}

const TRestGeant4Metadata* TRestGeant4Track::GetGeant4Metadata() const {
    if (GetEvent() == nullptr) {
        return nullptr;
    }
    return GetEvent()->GetGeant4Metadata();
}

TRestGeant4Track* TRestGeant4Track::GetParentTrack() const {
    if (fEvent == nullptr) {
        return nullptr;
    }
    return fEvent->GetTrackByID(GetParentID());
}

vector<const TRestGeant4Track*> TRestGeant4Track::GetSecondaryTracks() const {
    vector<const TRestGeant4Track*> secondaryTracks = {};
    for (const auto trackID : fSecondaryTrackIDs) {
        const TRestGeant4Track* track = fEvent->GetTrackByID(trackID);
        if (track != nullptr) {
            secondaryTracks.push_back(track);
        }
    }
    return secondaryTracks;
}

TString TRestGeant4Track::GetInitialVolume() const {
    const auto metadata = GetGeant4Metadata();
    if (metadata == nullptr) {
        return "";
    }
    const auto& hits = GetHits();
    return GetGeant4Metadata()->GetGeant4GeometryInfo().GetVolumeFromID(hits.GetVolumeId(0));
}

TString TRestGeant4Track::GetFinalVolume() const {
    const auto metadata = GetGeant4Metadata();
    if (metadata == nullptr) {
        return "";
    }
    const auto& hits = GetHits();
    return GetGeant4Metadata()->GetGeant4GeometryInfo().GetVolumeFromID(
        hits.GetVolumeId(hits.GetNumberOfHits() - 1));
}

Double_t TRestGeant4Track::GetEnergyInVolume(const TString& volumeName, bool children) const {
    const auto metadata = GetGeant4Metadata();
    if (metadata == nullptr) {
        return 0;
    }

    const auto volumeId = metadata->GetGeant4GeometryInfo().GetIDFromVolume(volumeName);

    if (!children) {
        return GetEnergyInVolume(volumeId);
    }

    Double_t energy = 0;
    vector<const TRestGeant4Track*> tracks = {this};
    while (!tracks.empty()) {
        const TRestGeant4Track* track = tracks.back();
        tracks.pop_back();
        if (track == nullptr) {
            continue;
        }
        energy += track->GetEnergyInVolume(volumeId);
        for (const TRestGeant4Track* secondaryTrack : track->GetSecondaryTracks()) {
            tracks.push_back(secondaryTrack);
        }
    }
    return energy;
}

TString TRestGeant4Track::GetLastProcessName() const {
    const auto metadata = GetGeant4Metadata();
    if (metadata == nullptr) {
        return "";
    }

    const auto& hits = GetHits();
    return GetGeant4Metadata()->GetGeant4PhysicsInfo().GetProcessName(
        hits.GetProcess(hits.GetNumberOfHits() - 1));
}
