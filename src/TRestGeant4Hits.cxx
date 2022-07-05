///_______________________________________________________________________________
///_______________________________________________________________________________
///_______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4Hits.cxx
///
///             Base class from which to inherit all other event classes in REST
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 J. Galan
///_______________________________________________________________________________

#include "TRestGeant4Hits.h"

using namespace std;

ClassImp(TRestGeant4Hits);

TRestGeant4Hits::TRestGeant4Hits() : TRestHits() {
    // TRestGeant4Hits default constructor
}

TRestGeant4Hits::~TRestGeant4Hits() {
    // TRestGeant4Hits destructor
}

void TRestGeant4Hits::RemoveG4Hits() {
    RemoveHits();

    fProcessID.clear();
    fVolumeID.clear();
    fKineticEnergy.clear();
}

Double_t TRestGeant4Hits::GetEnergyInVolume(Int_t volumeID) const {
    Double_t en = 0;

    for (int n = 0; n < fNHits; n++) {
        if (fVolumeID[n] == volumeID) en += GetEnergy(n);
    }
    return en;
}

TVector3 TRestGeant4Hits::GetMeanPositionInVolume(Int_t volumeID) const {
    TVector3 pos;
    Double_t en = 0;
    for (int n = 0; n < fNHits; n++)
        if (fVolumeID[n] == volumeID) {
            pos += GetPosition(n) * GetEnergy(n);
            en += GetEnergy(n);
        }

    if (en == 0) {
        Double_t nan = TMath::QuietNaN();
        return {nan, nan, nan};
    }

    pos = (1. / en) * pos;
    return pos;
}

TVector3 TRestGeant4Hits::GetFirstPositionInVolume(Int_t volumeID) const {
    for (int n = 0; n < fNHits; n++)
        if (fVolumeID[n] == volumeID) return GetPosition(n);

    Double_t nan = TMath::QuietNaN();
    return {nan, nan, nan};
}

TVector3 TRestGeant4Hits::GetLastPositionInVolume(Int_t volumeID) const {
    for (int n = fNHits - 1; n >= 0; n--)
        if (fVolumeID[n] == volumeID) return GetPosition(n);

    Double_t nan = TMath::QuietNaN();
    return {nan, nan, nan};
}

size_t TRestGeant4Hits::GetNumberOfHitsInVolume(Int_t volumeID) const {
    size_t result = 0;
    for (int n = 0; n < fNHits; n++) {
        if (fVolumeID[n] == volumeID) {
            result++;
        }
    }
    return result;
}
