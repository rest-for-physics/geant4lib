///_______________________________________________________________________________
///_______________________________________________________________________________
///_______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4EventViewer.cxx
///
///             nov 2015:   First concept
///                 Viewer class for a TRestGeant4Event
///                 Javier Galan/JuanAn Garcia
///_______________________________________________________________________________

#include "TRestGeant4EventViewer.h"

#include <TEveStraightLineSet.h>
#include <TRestStringOutput.h>

using namespace std;

ClassImp(TRestGeant4EventViewer);

TRestGeant4EventViewer::TRestGeant4EventViewer() { Initialize(); }

TRestGeant4EventViewer::~TRestGeant4EventViewer() {
    // TRestGeant4EventViewer destructor
}

void TRestGeant4EventViewer::Initialize() {
    fG4Event = new TRestGeant4Event();
    fEvent = fG4Event;

    fHitConnectors.clear();
    fHitConnectors.push_back(nullptr);
}

void TRestGeant4EventViewer::DeleteCurrentEvent() {
    TRestEveEventViewer::DeleteCurrentEvent();
    fG4Metadata = nullptr;

    fHitConnectors.clear();
    fHitConnectors.push_back(nullptr);
}

struct TrackVisualConfiguration {
    Color_t fColor = kWhite;
    Style_t fLineStyle = 1;
    Width_t fLineWidth = 4.0;
};

TrackVisualConfiguration GetTrackVisualConfiguration(const TRestGeant4Track& track) {
    auto config = TrackVisualConfiguration();
    // special particles
    const auto particleName = track.GetParticleName();
    if (particleName == "geantino") {
        config.fColor = kRed;
        config.fLineStyle = 9;
        config.fLineWidth = 2;
        return config;
    }
    // color based on charge
    if (particleName.Contains('-')) {
        config.fColor = kMagenta;  // red
    } else if (particleName.Contains('+')) {
        config.fColor = kAzure;
    }
    // color based on particle (overrides charge color)
    if (particleName == "neutron") {
        config.fColor = kOrange;  // white
    } else if (particleName == "gamma") {
        config.fColor = kGreen;  // green
    } else if (particleName == "e-") {
        config.fColor = kRed;  // red
    } else if (particleName == "mu-") {
        config.fColor = kGray;  // red
    } else if (particleName == "alpha") {
        config.fColor = kYellow;  // red
    }

    // width
    Width_t width = TMath::Log10(track.GetInitialKineticEnergy() / 100);
    width = (width > 10 ? 10 : width);
    width = (width < 1 ? 1 : width);
    config.fLineWidth = TMath::Log10(track.GetInitialKineticEnergy() / 10);

    // line style

    if (track.GetCreatorProcess() == "nCapture") {
        config.fLineStyle = 2;
    }

    return config;
}

TEveStraightLineSet* GetTrackEveDrawable(const TRestGeant4Track& track) {
    auto lineSet = new TEveStraightLineSet(
        TString::Format("ID %d | %s | Created by %s | KE: %s",  //
                        track.GetTrackID(), track.GetParticleName().Data(), track.GetCreatorProcess().Data(),
                        ToEnergyString(track.GetInitialKineticEnergy()).c_str()));

    const auto& hits = track.GetHits();
    for (int i = 0; i < hits.GetNumberOfHits() - 1; i++) {
        lineSet->AddLine({static_cast<float>(GEOM_SCALE * hits.GetPosition(i).x()),
                          static_cast<float>(GEOM_SCALE * hits.GetPosition(i).y()),
                          static_cast<float>(GEOM_SCALE * hits.GetPosition(i).z())},  //
                         {static_cast<float>(GEOM_SCALE * hits.GetPosition(i + 1).x()),
                          static_cast<float>(GEOM_SCALE * hits.GetPosition(i + 1).y()),
                          static_cast<float>(GEOM_SCALE * hits.GetPosition(i + 1).z())});

        const auto config = GetTrackVisualConfiguration(track);
        lineSet->SetMainColor(config.fColor);
        lineSet->SetLineColor(config.fColor);
        lineSet->SetLineStyle(config.fLineStyle);
        lineSet->SetLineWidth(config.fLineWidth);
    }

    return lineSet;
}

struct HitsVisualConfiguration {
    Color_t fColor = kBlack;
    Style_t fMarkerStyle = 1;
    Size_t fMarkerSize = 1.0;
};

HitsVisualConfiguration GetHitsVisualConfiguration(const TString& processNameOrType) {
    auto config = HitsVisualConfiguration();
    // based on particle type
    if (processNameOrType.EqualTo("Electromagnetic")) {
        config.fColor = kRed;
    } else if (processNameOrType.EqualTo("Init")) {
        // custom process (not Geant4)
        config.fColor = kWhite;
    }

    // based on particle name
    if (processNameOrType.EqualTo("Transportation")) {
        config.fColor = kWhite;
    } else if (processNameOrType.EqualTo("Init")) {
        // custom process (not Geant4)
        config.fColor = kWhite;
    } else if (processNameOrType.EqualTo("hadElastic")) {
        config.fColor = kOrange;
    } else if (processNameOrType.EqualTo("neutronInelastic")) {
        config.fColor = kGreen;
        config.fMarkerStyle = 4;
    } else if (processNameOrType.EqualTo("nCapture")) {
        config.fColor = kBlue;
        config.fMarkerStyle = 2;
        config.fMarkerSize = 4.0;
    }
    return config;
}

void TRestGeant4EventViewer::AddEvent(TRestEvent* event) {
    DeleteCurrentEvent();

    fG4Event = (TRestGeant4Event*)event;
    fG4Metadata = fG4Event->GetGeant4Metadata();
    if (fG4Metadata == nullptr) {
        cerr << "TRestGeant4EventViewer::Initialize. No TRestGeant4Metadata found in TRestGeant4Event"
             << endl;
        exit(1);
    }

    size_t trackCounter = 0;
    size_t hitsCounter = 0;

    map<Int_t, TEveStraightLineSet*> linesSet;
    map<TString, TEvePointSet*> hitsPoints;
    map<TString, TEveElementList*> hitsType;

    auto trackList = new TEveElementList("Tracks");
    gEve->AddElement(trackList);
    auto hitsList = new TEveElementList("Hits");
    gEve->AddElement(hitsList);

    const auto& physicsInfo = fG4Metadata->GetGeant4PhysicsInfo();

    for (const auto& track : fG4Event->GetTracks()) {
        if (track.GetInitialKineticEnergy() < 1.0 || track.GetLength() < 0.1) {
            continue;
        }

        auto line = GetTrackEveDrawable(track);
        linesSet[track.GetTrackID()] = line;
        TEveElement* parentLine = trackList;
        if (linesSet.count(track.GetParentID())) {
            parentLine = linesSet.at(track.GetParentID());
        }
        gEve->AddElement(line, parentLine);
        trackCounter++;

        const auto& hits = track.GetHits();
        for (int i = 0; i < hits.GetNumberOfHits(); i++) {
            const auto& processName = physicsInfo.GetProcessName(hits.GetProcess(i));
            const auto& processType = physicsInfo.GetProcessType(processName);
            const auto& position = hits.GetPosition(i);

            if (hitsType.count(processType) == 0) {
                hitsType[processType] = new TEveElementList(processType);
                gEve->AddElement(hitsType[processType], hitsList);
            }
            if (hitsPoints.count(processName) == 0) {
                hitsPoints[processName] = new TEvePointSet(processName);
                auto hitPoints = hitsPoints.at(processName);
                auto hitsVisualConfig = GetHitsVisualConfiguration(processName);
                hitPoints->SetMarkerColor(hitsVisualConfig.fColor);
                hitPoints->SetMarkerStyle(hitsVisualConfig.fMarkerStyle);
                hitPoints->SetMarkerSize(hitsVisualConfig.fMarkerSize);

                gEve->AddElement(hitPoints, hitsType[processType]);
            }
            hitsPoints.at(processName)
                ->SetNextPoint(GEOM_SCALE * position.X(), GEOM_SCALE * position.Y(),
                               GEOM_SCALE * position.Z());
            hitsCounter++;
        }
    }

    // Add event text
    const auto& firstTrack = fG4Event->GetTracks().front();
    TVector3 position = {firstTrack.GetInitialPosition().X(), firstTrack.GetInitialPosition().Y(),
                         firstTrack.GetInitialPosition().Z()};
    AddText(
        TString::Format(
            "Event ID: %d%s | Primary origin: (%4.2lf, %4.2lf, %4.2lf) mm", fG4Event->GetID(),
            (fG4Event->GetSubID() > 0 ? TString::Format(" (SubID: %d)", fG4Event->GetSubID()) : "").Data(),
            position.X(), position.Y(), position.Z()),
        position);

    Update();
}

void TRestGeant4EventViewer::AddText(TString text, TVector3 at) {
    TEveText* evText = new TEveText(text);
    evText->SetName("Event title");
    evText->SetFontSize(12);
    evText->RefMainTrans().SetPos((at.X() + 15) * GEOM_SCALE, (at.Y() + 15) * GEOM_SCALE,
                                  (at.Z() + 15) * GEOM_SCALE);

    gEve->AddElement(evText);
}

void TRestGeant4EventViewer::AddMarker(Int_t trkID, TVector3 at, TString name) {
    TEvePointSet* marker = new TEvePointSet(1);
    marker->SetName(name);
    marker->SetMarkerColor(kMagenta);
    marker->SetMarkerStyle(3);
    marker->SetPoint(0, at.X() * GEOM_SCALE, at.Y() * GEOM_SCALE, at.Z() * GEOM_SCALE);
    marker->SetMarkerSize(0.4);
    fHitConnectors[trkID]->AddElement(marker);
}

void TRestGeant4EventViewer::NextTrackVertex(Int_t trkID, TVector3 to) {
    fHitConnectors[trkID]->SetNextPoint(to.X() * GEOM_SCALE, to.Y() * GEOM_SCALE, to.Z() * GEOM_SCALE);
}

void TRestGeant4EventViewer::AddTrack(Int_t trkID, Int_t parentID, TVector3 from, TString name) {
    TEveLine* evLine = new TEveLine();
    evLine->SetName(name);
    fHitConnectors.push_back(evLine);

    fHitConnectors[trkID]->SetMainColor(kWhite);
    fHitConnectors[trkID]->SetLineWidth(4);

    if (name.Contains("gamma")) fHitConnectors[trkID]->SetMainColor(kGreen);
    if (name.Contains("e-")) fHitConnectors[trkID]->SetMainColor(kRed);
    if (name.Contains("mu-")) fHitConnectors[trkID]->SetMainColor(kGray);
    if (name.Contains("alpha")) fHitConnectors[trkID]->SetMainColor(kYellow);
    if (name.Contains("neutron")) fHitConnectors[trkID]->SetMainColor(kBlue);

    fHitConnectors[trkID]->SetNextPoint(from.X() * GEOM_SCALE, from.Y() * GEOM_SCALE, from.Z() * GEOM_SCALE);

    if (fHitConnectors.size() > parentID)
        fHitConnectors[parentID]->AddElement(fHitConnectors[trkID]);
    else {
        RESTWarning << "Parent ID: " << parentID << " of track " << trkID << " was not found!" << RESTendl;
        RESTWarning << "This might be solved by enabling TRestGeant4Metadata::fRegisterEmptyTracks"
                    << RESTendl;
    }
}

void TRestGeant4EventViewer::AddParentTrack(Int_t trkID, TVector3 from, TString name) {
    TEveLine* evLine = new TEveLine();
    evLine->SetName(name);
    fHitConnectors.push_back(evLine);

    fHitConnectors[trkID]->SetMainColor(kWhite);
    fHitConnectors[trkID]->SetLineWidth(4);
    fHitConnectors[trkID]->SetNextPoint(from.X() * GEOM_SCALE, from.Y() * GEOM_SCALE, from.Z() * GEOM_SCALE);

    gEve->AddElement(fHitConnectors[trkID]);
}
