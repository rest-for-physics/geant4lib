//
// Created by lobis on 15/11/2021.
//

#include "TRestGeant4Geometry.h"

using namespace std;

Bool_t TRestGeant4Geometry::IsActiveVolume(const TString& inputPhysicalVolume) const {
    for (const auto& volume : fActiveVolumes) {
        if (inputPhysicalVolume.EqualTo(volume)) {
            return true;
        }
    }
    return false;
}

Bool_t TRestGeant4Geometry::IsPhysicalVolume(const TString& inputPhysicalVolume) const {
    for (const auto& volume : fPhysicalVolumes) {
        if (inputPhysicalVolume.EqualTo(volume)) {
            return true;
        }
    }
    return false;
}

Bool_t TRestGeant4Geometry::IsLogicalVolume(const TString& inputPhysicalVolume) const {
    for (const auto& volume : fLogicalVolumes) {
        if (inputPhysicalVolume.EqualTo(volume)) {
            return true;
        }
    }
    return false;
}

TVector3 TRestGeant4Geometry::GetPhysicalVolumePosition(const TString& inputPhysicalVolume) const {
    /*
     * This method will throw exception if the requested volume does not exist, if not sure, check with
     * `IsPhysicalVolume`
     */
    return fPhysicalToPositionInWorldMap.at(inputPhysicalVolume);
}

/// Attempts to return the unique physical volume associated with a given logical volume.
/// Returns "" if not found
TString TRestGeant4Geometry::GetUniquePhysicalFromLogical(const TString& inputLogicalVolume) const {
    if (IsLogicalVolume(inputLogicalVolume)) {
        auto physicalVolumes = fLogicalToPhysicalMap.at(inputLogicalVolume);
        if (physicalVolumes.size() == 1) {
            return physicalVolumes[0];
        }
    }
    return "";
}

vector<TString> TRestGeant4Geometry::GetAllPhysicalFromLogical(const TString& inputLogicalVolume) const {
    return fLogicalToPhysicalMap.at(inputLogicalVolume);
}

TString TRestGeant4Geometry::GetLogicalFromPhysical(const TString& inputPhysicalVolume) const {
    return fPhysicalToLogicalVolumeMap.at(inputPhysicalVolume);
}

TString TRestGeant4Geometry::GetMaterialFromLogical(const TString& inputLogicalVolume) const {
    return fLogicalToMaterialMap.at(inputLogicalVolume);
}

TString TRestGeant4Geometry::GetMaterialFromPhysical(const TString& inputPhysicalVolume) const {
    return fLogicalToMaterialMap.at(GetLogicalFromPhysical(inputPhysicalVolume));
}

TString TRestGeant4Geometry::GetPhysicalFromGeant4Physical(const TString& inputGeant4PhysicalVolume) const {
    if (fGeant4PhysicalToPhysicalMap.empty()) {
        return inputGeant4PhysicalVolume;
    }
    return fGeant4PhysicalToPhysicalMap.at(inputGeant4PhysicalVolume);
}
