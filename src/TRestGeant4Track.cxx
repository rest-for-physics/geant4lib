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

#include <fmt/color.h>
#include <fmt/core.h>

#include "TRestGeant4Event.h"
#include "TRestGeant4Metadata.h"

using namespace std;
using namespace fmt;

ClassImp(TRestGeant4Track);

TRestGeant4Track::TRestGeant4Track() {}

TRestGeant4Track::~TRestGeant4Track() {}

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

template <>
struct fmt::formatter<TVector3> : formatter<string> {
    auto format(TVector3 c, format_context& ctx) {
        string s = fmt::format("({:0.3f}, {:0.3f}, {:0.3f})", c.X(), c.Y(), c.Z());
        return formatter<string>::format(s, ctx);
    }
};

void TRestGeant4Track::PrintTrack(size_t maxHits) const {
    print(fg(color::green_yellow),
          " * TrackID: {} - Particle: {} - ParentID: {}{} - Created by '{}' with initial "
          "KE of {}\n",
          fTrackID, fParticleName, fParentID,
          (GetParentTrack() != nullptr ? format(" - Parent particle: {}", GetParentTrack()->GetParticleName())
                                       : ""),
          fCreatorProcess, ToEnergyString(fInitialKineticEnergy));
    print(fg(color::green_yellow),
          "   Initial position {} mm at time {} - Time length of {} and spatial length of {}\n",
          fInitialPosition, ToTimeString(fGlobalTimestamp), ToTimeString(fTimeLength),
          ToLengthString(fLength));

    size_t nHits = GetNumberOfHits();
    if (maxHits > 0 && maxHits < nHits) {
        nHits = min(maxHits, nHits);
        print(fg(color::light_slate_gray), "Printing only the first {} hits of the track\n", nHits);
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

        print(
            (fHits.GetEnergy(i) > 0 ? fg(color::light_salmon) : fg(color::antique_white)),
            "      - Hit {} - Energy: {} - Process: {} - Volume: {} - Position: {} mm - Time: {} - KE: {}\n",
            i, ToEnergyString(fHits.GetEnergy(i)), processName, volumeName,
            TVector3(fHits.GetX(i), fHits.GetY(i), fHits.GetZ(i)), ToTimeString(fHits.GetTime(i)),
            ToEnergyString(fHits.GetKineticEnergy(i)));
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
