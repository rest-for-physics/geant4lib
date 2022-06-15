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

#include "TRestGeant4Metadata.h"

using namespace std;

ClassImp(TRestGeant4Track);

TRestGeant4Track::TRestGeant4Track() {
    // TRestGeant4Track default constructor
}

TRestGeant4Track::~TRestGeant4Track() {
    // TRestGeant4Track destructor
}

Int_t TRestGeant4Track::GetProcessID(const TString& processName) {
    auto geant4Metadata = TRestGeant4Metadata::GetUnambiguousGlobalInstance("TRestGeant4Metadata");
    if (geant4Metadata != nullptr) {
        const auto processID = geant4Metadata->GetGeant4PhysicsInfo().GetProcessID(processName);
        if (processID != Int_t{}) {
            return processID;
        }
    }

    cout << "WARNING : The process " << processName << " was not found" << endl;
    return -1;
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
Int_t TRestGeant4Track::GetNumberOfHits(Int_t volID) const {
    Int_t hits = 0;
    for (int n = 0; n < fHits.GetNumberOfHits(); n++) {
        if (volID != -1 && fHits.GetVolumeId(n) != volID) continue;
        hits++;
    }
    return hits;
}

Double_t TRestGeant4Track::GetTrackLength() const {
    Double_t length = 0;

    length = GetDistance(fHits.GetPosition(0), GetTrackOrigin());

    for (int i = 1; i < GetNumberOfHits(); i++) {
        TVector3 prevHit = fHits.GetPosition(i - 1);
        TVector3 hit = fHits.GetPosition(i);
        length += GetDistance(hit, prevHit);
    }
    return length;
}

TString TRestGeant4Track::GetProcessName(Int_t processID) const {
    auto geant4Metadata = TRestGeant4Metadata::GetUnambiguousGlobalInstance("TRestGeant4Metadata");
    if (geant4Metadata != nullptr) {
        const auto& processName = geant4Metadata->GetGeant4PhysicsInfo().GetProcessName(processID);
        if (processName != TString{}) {
            return processName;
        }
    }

    cout << "WARNING : The process " << processID << " was not found" << endl;
    return "";
}

void TRestGeant4Track::PrintTrack(int maxHits) const {
    cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++" << endl;
    cout.precision(10);
    cout << " SubEvent ID : " << fSubEventId << " Global timestamp : " << GetGlobalTime() << " seconds"
         << endl;
    cout.precision(5);
    cout << " Track ID : " << GetTrackID() << " Parent ID : " << GetParentID();
    cout << " Particle : " << GetParticleName() << " Time track length : " << GetTrackTimeLength() << " us"
         << endl;
    cout << " Origin : X = " << GetTrackOrigin().X() << "mm Y = " << GetTrackOrigin().Y()
         << "mm Z = " << GetTrackOrigin().Z() << "mm  Ekin : " << GetKineticEnergy() << " keV" << endl;
    cout << "++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++"
            "++++++++++++"
         << endl;

    int nHits = GetNumberOfHits();
    if (maxHits > 0) {
        nHits = min(maxHits, GetNumberOfHits());
        cout << " Printing only the first " << nHits << " hits of the track" << endl;
    }

    for (int i = 0; i < nHits; i++) {
        cout << " # Hit " << i << " # process : " << GetProcessName(fHits.GetHitProcess(i))
             << " volume : " << fHits.GetHitVolume(i) << " X : " << fHits.GetX(i) << " Y : " << fHits.GetY(i)
             << " Z : " << fHits.GetZ(i) << " mm" << endl;
        cout << " Edep : " << fHits.GetEnergy(i) << " keV Ekin : " << fHits.GetKineticEnergy(i) << " keV"
             << " Global time : " << fHits.GetTime(i) << " us" << endl;
    }
    cout << endl;
    cout.precision(2);
}
