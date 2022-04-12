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

void TRestGeant4Hits::AddG4Hit(const TVector3& pos, Double_t en, Double_t hit_global_time, Int_t process,
                               Int_t volume, Double_t eKin, const TVector3& momentumDirection) {
    AddHit(pos, en, hit_global_time);

    fProcessID.push_back(process);
    fVolumeID.push_back(volume);
    fKineticEnergy.push_back(eKin);

    const auto momentumDirectionUnit = momentumDirection.Unit();

    fMomentumDirectionX.push_back(momentumDirectionUnit.X());
    fMomentumDirectionY.push_back(momentumDirectionUnit.Y());
    fMomentumDirectionZ.push_back(momentumDirectionUnit.Z());
}

void TRestGeant4Hits::RemoveG4Hits() {
    RemoveHits();

    fProcessID.clear();
    fVolumeID.clear();
    fKineticEnergy.clear();

    fMomentumDirectionX.clear();
    fMomentumDirectionY.clear();
    fMomentumDirectionZ.clear();
}

Double_t TRestGeant4Hits::GetEnergyInVolume(Int_t volID) const {
    Double_t en = 0;

    for (int n = 0; n < fNHits; n++)
        if (fVolumeID[n] == volID) en += GetEnergy(n);

    return en;
}

TVector3 TRestGeant4Hits::GetMeanPositionInVolume(Int_t volID) const {
    TVector3 pos;
    Double_t en = 0;
    for (int n = 0; n < fNHits; n++)
        if (fVolumeID[n] == volID) {
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

TVector3 TRestGeant4Hits::GetFirstPositionInVolume(Int_t volID) const {
    for (int n = 0; n < fNHits; n++)
        if (fVolumeID[n] == volID) return GetPosition(n);

    Double_t nan = TMath::QuietNaN();
    return {nan, nan, nan};
}

TVector3 TRestGeant4Hits::GetLastPositionInVolume(Int_t volID) const {
    for (int n = fNHits - 1; n >= 0; n--)
        if (fVolumeID[n] == volID) return GetPosition(n);

    Double_t nan = TMath::QuietNaN();
    return {nan, nan, nan};
}
