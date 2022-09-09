///_______________________________________________________________________________
///_______________________________________________________________________________
///_______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4Event.h
///
///             G4 Event class to store results from Geant4 REST simulation
///
///             jul 2015:   First concept
///                 Created as part of the conceptualization of existing REST
///                 software.
///                 Javier Galan
///_______________________________________________________________________________

#include "TRestGeant4Event.h"

#include <TFrame.h>
#include <TRestRun.h>
#include <TRestStringHelper.h>
#include <TRestTools.h>
#include <TStyle.h>
#include <fmt/color.h>
#include <fmt/core.h>

#include "TRestGeant4Metadata.h"

using namespace std;
using namespace fmt;

ClassImp(TRestGeant4Event);

TRestGeant4Event::TRestGeant4Event() {
    fNVolumes = 0;
    // TRestGeant4Event default constructor

    Initialize();
}

TRestGeant4Event::~TRestGeant4Event() {
    // TRestGeant4Event destructor
}

void TRestGeant4Event::Initialize() {
    TRestEvent::Initialize();

    fPrimaryParticleNames.clear();

    fTracks.clear();

    // ClearVolumes();
    fXZHitGraph = nullptr;
    fYZHitGraph = nullptr;
    fXYHitGraph = nullptr;

    fXZMultiGraph = nullptr;
    fYZMultiGraph = nullptr;
    fXYMultiGraph = nullptr;

    fXYHisto = nullptr;
    fYZHisto = nullptr;
    fXZHisto = nullptr;

    fXHisto = nullptr;
    fYHisto = nullptr;
    fZHisto = nullptr;

    fPad = nullptr;

    fLegend_XY = nullptr;
    fLegend_XZ = nullptr;
    fLegend_YZ = nullptr;

    fTotalDepositedEnergy = 0;
    fSensitiveVolumeEnergy = 0;

    fMinX = 1e20;
    fMaxX = -1e20;

    fMinY = 1e20;
    fMaxY = -1e20;

    fMinZ = 1e20;
    fMaxZ = -1e20;

    fMinEnergy = 1e20;
    fMaxEnergy = -1e20;
}

void TRestGeant4Event::AddActiveVolume(const string& volumeName) {
    fNVolumes++;
    fVolumeStored.push_back(1);
    fVolumeStoredNames.push_back(volumeName);
    fVolumeDepositedEnergy.push_back(0);
}

void TRestGeant4Event::ClearVolumes() {
    fNVolumes = 0;
    fVolumeStored.clear();
    fVolumeStoredNames.clear();
    fVolumeDepositedEnergy.clear();
}

void TRestGeant4Event::AddEnergyDepositToVolume(Int_t volID, Double_t eDep) {
    fVolumeDepositedEnergy[volID] += eDep;
}

TVector3 TRestGeant4Event::GetMeanPositionInVolume(Int_t volID) const {
    TVector3 pos;
    Double_t eDep = 0;

    for (int t = 0; t < GetNumberOfTracks(); t++) {
        const auto tck = GetTrack(t);
        if (tck.GetEnergyInVolume(volID) > 0) {
            pos += tck.GetMeanPositionInVolume(volID) * tck.GetEnergyInVolume(volID);

            eDep += tck.GetEnergyInVolume(volID);
        }
    }

    if (eDep == 0) {
        Double_t nan = TMath::QuietNaN();
        return {nan, nan, nan};
    }

    pos = (1 / eDep) * pos;
    return pos;
}

///////////////////////////////////////////////
/// \brief A method that gets the standard deviation from the hits happening at a
/// particular volumeId inside the geometry.
///
/// \param volID Int_t specifying volume ID
///
TVector3 TRestGeant4Event::GetPositionDeviationInVolume(Int_t volID) const {
    TVector3 meanPos = this->GetMeanPositionInVolume(volID);

    Double_t nan = TMath::QuietNaN();
    if (meanPos == TVector3(nan, nan, nan)) return TVector3(nan, nan, nan);

    TRestHits hitsInVolume = GetHitsInVolume(volID);

    Double_t edep = 0;
    TVector3 deviation = TVector3(0, 0, 0);

    for (int n = 0; n < hitsInVolume.GetNumberOfHits(); n++) {
        Double_t en = hitsInVolume.GetEnergy(n);
        TVector3 diff = meanPos - hitsInVolume.GetPosition(n);
        diff.SetXYZ(diff.X() * diff.X(), diff.Y() * diff.Y(), diff.Z() * diff.Z());

        deviation = deviation + en * diff;

        edep += en;
    }
    deviation = (1. / edep) * deviation;
    return deviation;
}

///////////////////////////////////////////////
/// \brief Function to get the position (TVector3) of the first track that deposits energy in specified
/// volume. If no hit is found for the volume, returns `TVector3(nan, nan, nan)` vector.
///
///
/// \param volID Int_t specifying volume ID
///
TVector3 TRestGeant4Event::GetFirstPositionInVolume(Int_t volID) const {
    for (int t = 0; t < GetNumberOfTracks(); t++)
        if (GetTrack(t).GetEnergyInVolume(volID) > 0) return GetTrack(t).GetFirstPositionInVolume(volID);

    Double_t nan = TMath::QuietNaN();
    return {nan, nan, nan};
}

///////////////////////////////////////////////
/// \brief Function to get the position (TVector3) of the last track that deposits energy in specified
/// volume. If no hit is found for the volume, returns `TVector3(nan, nan, nan)` vector.
/// If an event enters and exits a volume multiple times this will only return the last exit position, one
/// needs to keep this in mind when using this observable.
///
///
/// \param volID Int_t specifying volume ID
///
TVector3 TRestGeant4Event::GetLastPositionInVolume(Int_t volID) const {
    for (int t = GetNumberOfTracks() - 1; t >= 0; t--)
        if (GetTrack(t).GetEnergyInVolume(volID) > 0) return GetTrack(t).GetLastPositionInVolume(volID);

    Double_t nan = TMath::QuietNaN();
    return {nan, nan, nan};
}

TRestGeant4Track* TRestGeant4Event::GetTrackByID(Int_t trackID) const {
    TRestGeant4Track* result = nullptr;
    if (fTrackIDToTrackIndex.count(trackID) > 0) {
        result = const_cast<TRestGeant4Track*>(&fTracks[fTrackIDToTrackIndex.at(trackID)]);
        if (result->GetTrackID() != trackID) {
            cerr << "TRestGeant4Event::GetTrackByID - ERROR: trackIDToTrackIndex map is corrupted" << endl;
            exit(1);
        }
    }
    return result;
}

///////////////////////////////////////////////
/// \brief Function that returns the total number of hits in the Geant4 event. If
/// a specific volume is given as argument only the hits of that specific volume
/// will be counted.
///
size_t TRestGeant4Event::GetNumberOfHits(Int_t volID) const {
    size_t numberOfHits = 0;
    for (const auto& track : fTracks) {
        numberOfHits += track.GetNumberOfHits(volID);
    }
    return numberOfHits;
}

///////////////////////////////////////////////
/// \brief Function that returns the total number of hits with energy > 0 in the Geant4 event. If
/// a specific volume is given as argument only the hits of that specific volume
/// will be counted.
///
size_t TRestGeant4Event::GetNumberOfPhysicalHits(Int_t volID) const {
    size_t numberOfHits = 0;
    for (const auto& track : fTracks) {
        numberOfHits += track.GetNumberOfPhysicalHits(volID);
    }
    return numberOfHits;
}

///////////////////////////////////////////////
/// \brief Function that returns all the hit depositions in the Geant4 event. If
/// a specific volume is given as argument only the hits of that specific volume
/// will be added to the TRestHits returned object.
///
TRestHits TRestGeant4Event::GetHits(Int_t volID) const {
    TRestHits hits;
    for (int t = 0; t < GetNumberOfTracks(); t++) {
        const auto& g4Hits = GetTrack(t).GetHits();
        for (int n = 0; n < g4Hits.GetNumberOfHits(); n++) {
            if (volID != -1 && g4Hits.GetVolumeId(n) != volID) continue;

            Double_t x = g4Hits.GetX(n);
            Double_t y = g4Hits.GetY(n);
            Double_t z = g4Hits.GetZ(n);
            Double_t en = g4Hits.GetEnergy(n);

            hits.AddHit(x, y, z, en);
        }
    }

    return hits;
}

Int_t TRestGeant4Event::GetNumberOfTracksForParticle(const TString& parName) const {
    Int_t tcks = 0;
    for (Int_t t = 0; t < GetNumberOfTracks(); t++)
        if (GetTrack(t).GetParticleName() == parName) tcks++;

    return tcks;
}

Int_t TRestGeant4Event::GetEnergyDepositedByParticle(const TString& parName) const {
    Double_t en = 0;
    for (Int_t t = 0; t < GetNumberOfTracks(); t++) {
        if (GetTrack(t).GetParticleName() == parName) en += GetTrack(t).GetEnergy();
    }

    return en;
}

void TRestGeant4Event::SetBoundaries(Double_t xMin, Double_t xMax, Double_t yMin, Double_t yMax,
                                     Double_t zMin, Double_t zMax) {
    SetBoundaries();

    fMinX = xMin;
    fMaxX = xMax;

    fMinY = yMin;
    fMaxY = yMax;

    fMinZ = zMin;
    fMaxZ = zMax;
}

void TRestGeant4Event::SetBoundaries() {
    Double_t maxX = -1e10, minX = 1e10, maxZ = -1e10, minZ = 1e10, maxY = -1e10, minY = 1e10;
    Double_t minEnergy = 1e10, maxEnergy = -1e10;

    Int_t nTHits = 0;
    for (int ntck = 0; ntck < this->GetNumberOfTracks(); ntck++) {
        Int_t nHits = GetTrack(ntck).GetNumberOfHits();
        const auto& hits = GetTrack(ntck).GetHits();

        for (int nhit = 0; nhit < nHits; nhit++) {
            Double_t x = hits.GetX(nhit);
            Double_t y = hits.GetY(nhit);
            Double_t z = hits.GetZ(nhit);

            Double_t en = hits.GetEnergy(nhit);

            if (x > maxX) maxX = x;
            if (x < minX) minX = x;
            if (y > maxY) maxY = y;
            if (y < minY) minY = y;
            if (z > maxZ) maxZ = z;
            if (z < minZ) minZ = z;

            if (en > maxEnergy) maxEnergy = en;
            if (en < minEnergy) minEnergy = en;

            nTHits++;
        }
    }

    fMinX = minX;
    fMaxX = maxX;

    fMinY = minY;
    fMaxY = maxY;

    fMinZ = minZ;
    fMaxZ = maxZ;

    fMaxEnergy = maxEnergy;
    fMinEnergy = minEnergy;

    fTotalHits = nTHits;
}

/* {{{ Get{XY,YZ,XZ}MultiGraph methods */
TMultiGraph* TRestGeant4Event::GetXYMultiGraph(Int_t gridElement, vector<TString> pcsList,
                                               Double_t minPointSize, Double_t maxPointSize) {
    if (fXYHitGraph) {
        delete[] fXYHitGraph;
        fXYHitGraph = nullptr;
    }
    fXYHitGraph = new TGraph[fTotalHits];

    fXYMultiGraph = new TMultiGraph();

    if (fLegend_XY) {
        delete fLegend_XY;
        fLegend_XY = nullptr;
    }

    fLegend_XY = new TLegend(0.75, 0.75, 0.9, 0.85);

    char title[256];
    sprintf(title, "Event ID %d (XY)", this->GetID());
    fPad->cd(gridElement)->DrawFrame(fMinX - 10, fMinY - 10, fMaxX + 10, fMaxY + 10, title);

    Double_t m = 1, n = 0;
    if (fMaxEnergy - fMinEnergy > 0) {
        m = (maxPointSize - minPointSize) / (fMaxEnergy - fMinEnergy);
        n = maxPointSize - m * fMaxEnergy;
    }

    for (unsigned int n = 0; n < fXYPcsMarker.size(); n++) delete fXYPcsMarker[n];
    fXYPcsMarker.clear();

    legendAdded.clear();
    for (unsigned int p = 0; p < pcsList.size(); p++) legendAdded.push_back(0);

    Int_t cnt = 0;
    for (int ntck = 0; ntck < GetNumberOfTracks(); ntck++) {
        const auto hits = GetTrack(ntck).GetHits();

        EColor color = GetTrack(ntck).GetParticleColor();

        for (int nhit = 0; nhit < hits.GetNumberOfHits(); nhit++) {
            Double_t xPos = hits.GetX(nhit);
            Double_t yPos = hits.GetY(nhit);
            Double_t energy = hits.GetEnergy(nhit);

            for (unsigned int p = 0; p < pcsList.size(); p++) {
                if (GetTrack(ntck).GetProcessName(hits.GetProcess(nhit)) == pcsList[p]) {
                    TGraph* pcsGraph = new TGraph(1);

                    pcsGraph->SetPoint(0, xPos, yPos);
                    pcsGraph->SetMarkerColor(kBlack);
                    pcsGraph->SetMarkerSize(3);
                    pcsGraph->SetMarkerStyle(30 + p);

                    fXYPcsMarker.push_back(pcsGraph);

                    if (legendAdded[p] == 0) {
                        fLegend_XY->AddEntry(pcsGraph, GetTrack(ntck).GetProcessName(hits.GetProcess(nhit)),
                                             "p");
                        legendAdded[p] = 1;
                    }
                }
            }

            Double_t radius = m * energy + n;
            if (fMaxEnergy - fMinEnergy <= 0) radius = 2.5;

            fXYHitGraph[cnt].SetPoint(0, xPos, yPos);

            fXYHitGraph[cnt].SetMarkerColor(color);
            fXYHitGraph[cnt].SetMarkerSize(radius);
            fXYHitGraph[cnt].SetMarkerStyle(20);

            fXYMultiGraph->Add(&fXYHitGraph[cnt]);

            cnt++;
        }
    }

    fXYMultiGraph->GetXaxis()->SetTitle("X-axis (mm)");
    fXYMultiGraph->GetXaxis()->SetTitleSize(1.25 * fXYMultiGraph->GetXaxis()->GetTitleSize());
    fXYMultiGraph->GetXaxis()->SetTitleOffset(1.25);
    fXYMultiGraph->GetXaxis()->SetLabelSize(1.25 * fXYMultiGraph->GetXaxis()->GetLabelSize());

    fXYMultiGraph->GetYaxis()->SetTitle("Y-axis (mm)");
    fXYMultiGraph->GetYaxis()->SetTitleSize(1.25 * fXYMultiGraph->GetYaxis()->GetTitleSize());
    fXYMultiGraph->GetYaxis()->SetTitleOffset(1.75);
    fXYMultiGraph->GetYaxis()->SetLabelSize(1.25 * fXYMultiGraph->GetYaxis()->GetLabelSize());
    fPad->cd(gridElement);

    return fXYMultiGraph;
}

TMultiGraph* TRestGeant4Event::GetYZMultiGraph(Int_t gridElement, vector<TString> pcsList,
                                               Double_t minPointSize, Double_t maxPointSize) {
    if (fYZHitGraph) {
        delete[] fYZHitGraph;
        fYZHitGraph = nullptr;
    }
    fYZHitGraph = new TGraph[fTotalHits];

    fYZMultiGraph = new TMultiGraph();

    if (fLegend_YZ) {
        delete fLegend_YZ;
        fLegend_YZ = nullptr;
    }

    fLegend_YZ = new TLegend(0.75, 0.75, 0.9, 0.85);

    char title[256];
    sprintf(title, "Event ID %d (YZ)", this->GetID());
    fPad->cd(gridElement)->DrawFrame(fMinY - 10, fMinZ - 10, fMaxY + 10, fMaxZ + 10, title);

    for (unsigned int n = 0; n < fYZPcsMarker.size(); n++) delete fYZPcsMarker[n];
    fYZPcsMarker.clear();

    Double_t m = 1, n = 0;
    if (fMaxEnergy - fMinEnergy > 0) {
        m = (maxPointSize - minPointSize) / (fMaxEnergy - fMinEnergy);
        n = maxPointSize - m * fMaxEnergy;
    }

    legendAdded.clear();
    for (unsigned int p = 0; p < pcsList.size(); p++) legendAdded.push_back(0);

    Int_t cnt = 0;
    for (int ntck = 0; ntck < GetNumberOfTracks(); ntck++) {
        const auto& hits = GetTrack(ntck).GetHits();

        EColor color = GetTrack(ntck).GetParticleColor();

        for (int nhit = 0; nhit < hits.GetNumberOfHits(); nhit++) {
            Double_t yPos = hits.GetY(nhit);
            Double_t zPos = hits.GetZ(nhit);
            Double_t energy = hits.GetEnergy(nhit);

            for (unsigned int p = 0; p < pcsList.size(); p++) {
                if (GetTrack(ntck).GetProcessName(hits.GetProcess(nhit)) == pcsList[p]) {
                    TGraph* pcsGraph = new TGraph(1);

                    pcsGraph->SetPoint(0, yPos, zPos);
                    pcsGraph->SetMarkerColor(kBlack);
                    pcsGraph->SetMarkerSize(3);
                    pcsGraph->SetMarkerStyle(30 + p);

                    fYZPcsMarker.push_back(pcsGraph);

                    if (legendAdded[p] == 0) {
                        fLegend_YZ->AddEntry(pcsGraph, GetTrack(ntck).GetProcessName(hits.GetProcess(nhit)),
                                             "p");
                        legendAdded[p] = 1;
                    }
                }
            }

            Double_t radius = m * energy + n;
            if (fMaxEnergy - fMinEnergy <= 0) radius = 2.5;

            fYZHitGraph[cnt].SetPoint(0, yPos, zPos);

            fYZHitGraph[cnt].SetMarkerColor(color);
            fYZHitGraph[cnt].SetMarkerSize(radius);
            fYZHitGraph[cnt].SetMarkerStyle(20);

            fYZMultiGraph->Add(&fYZHitGraph[cnt]);

            cnt++;
        }
    }

    fYZMultiGraph->GetXaxis()->SetTitle("Y-axis (mm)");
    fYZMultiGraph->GetXaxis()->SetTitleSize(1.25 * fYZMultiGraph->GetXaxis()->GetTitleSize());
    fYZMultiGraph->GetXaxis()->SetTitleOffset(1.25);
    fYZMultiGraph->GetXaxis()->SetLabelSize(1.25 * fYZMultiGraph->GetXaxis()->GetLabelSize());

    fYZMultiGraph->GetYaxis()->SetTitle("Z-axis (mm)");
    fYZMultiGraph->GetYaxis()->SetTitleSize(1.25 * fYZMultiGraph->GetYaxis()->GetTitleSize());
    fYZMultiGraph->GetYaxis()->SetTitleOffset(1.75);
    fYZMultiGraph->GetYaxis()->SetLabelSize(1.25 * fYZMultiGraph->GetYaxis()->GetLabelSize());
    fPad->cd(gridElement);

    return fYZMultiGraph;
}

TMultiGraph* TRestGeant4Event::GetXZMultiGraph(Int_t gridElement, vector<TString> pcsList,
                                               Double_t minPointSize, Double_t maxPointSize) {
    if (fXZHitGraph) {
        delete[] fXZHitGraph;
        fXZHitGraph = nullptr;
    }
    fXZHitGraph = new TGraph[fTotalHits];

    fXZMultiGraph = new TMultiGraph();

    if (fLegend_XZ) {
        delete fLegend_XZ;
        fLegend_XZ = nullptr;
    }

    fLegend_XZ = new TLegend(0.75, 0.75, 0.9, 0.85);

    char title[256];
    sprintf(title, "Event ID %d (XZ)", this->GetID());
    fPad->cd(gridElement)->DrawFrame(fMinX - 10, fMinZ - 10, fMaxX + 10, fMaxZ + 10, title);

    for (unsigned int n = 0; n < fXZPcsMarker.size(); n++) delete fXZPcsMarker[n];
    fXZPcsMarker.clear();

    Double_t m = 1, n = 0;
    if (fMaxEnergy - fMinEnergy > 0) {
        m = (maxPointSize - minPointSize) / (fMaxEnergy - fMinEnergy);
        n = maxPointSize - m * fMaxEnergy;
    }

    legendAdded.clear();
    for (unsigned int p = 0; p < pcsList.size(); p++) legendAdded.push_back(0);

    Int_t cnt = 0;
    for (int ntck = 0; ntck < GetNumberOfTracks(); ntck++) {
        const auto& hits = GetTrack(ntck).GetHits();

        EColor color = GetTrack(ntck).GetParticleColor();

        for (int nhit = 0; nhit < hits.GetNumberOfHits(); nhit++) {
            Double_t xPos = hits.GetX(nhit);
            Double_t zPos = hits.GetZ(nhit);
            Double_t energy = hits.GetEnergy(nhit);

            for (unsigned int p = 0; p < pcsList.size(); p++) {
                if (GetTrack(ntck).GetProcessName(hits.GetProcess(nhit)) == pcsList[p]) {
                    TGraph* pcsGraph = new TGraph(1);

                    pcsGraph->SetPoint(0, xPos, zPos);
                    pcsGraph->SetMarkerColor(kBlack);
                    pcsGraph->SetMarkerSize(3);
                    pcsGraph->SetMarkerStyle(30 + p);

                    fXZPcsMarker.push_back(pcsGraph);

                    if (legendAdded[p] == 0) {
                        fLegend_XZ->AddEntry(pcsGraph, GetTrack(ntck).GetProcessName(hits.GetProcess(nhit)),
                                             "p");
                        legendAdded[p] = 1;
                    }
                }
            }

            Double_t radius = m * energy + n;
            if (fMaxEnergy - fMinEnergy <= 0) radius = 2.5;

            fXZHitGraph[cnt].SetPoint(0, xPos, zPos);

            fXZHitGraph[cnt].SetMarkerColor(color);
            fXZHitGraph[cnt].SetMarkerSize(radius);
            fXZHitGraph[cnt].SetMarkerStyle(20);

            fXZMultiGraph->Add(&fXZHitGraph[cnt]);

            cnt++;
        }
    }

    fXZMultiGraph->GetXaxis()->SetTitle("X-axis (mm)");
    fXZMultiGraph->GetXaxis()->SetTitleOffset(1.25);
    fXZMultiGraph->GetXaxis()->SetTitleSize(1.25 * fXZMultiGraph->GetXaxis()->GetTitleSize());
    fXZMultiGraph->GetXaxis()->SetLabelSize(1.25 * fXZMultiGraph->GetXaxis()->GetLabelSize());

    fXZMultiGraph->GetYaxis()->SetTitle("Z-axis (mm)");
    fXZMultiGraph->GetYaxis()->SetTitleOffset(1.75);
    fXZMultiGraph->GetYaxis()->SetTitleSize(1.25 * fXZMultiGraph->GetYaxis()->GetTitleSize());
    fXZMultiGraph->GetYaxis()->SetLabelSize(1.25 * fXZMultiGraph->GetYaxis()->GetLabelSize());
    fPad->cd(gridElement);

    return fXZMultiGraph;
}
/* }}} */

/* {{{ Get{XY,YZ,XZ}Histogram methods */
TH2F* TRestGeant4Event::GetXYHistogram(Int_t gridElement, vector<TString> optList) {
    if (fXYHisto) {
        delete fXYHisto;
        fXYHisto = nullptr;
    }

    Bool_t stats = false;
    Double_t pitch = 3;
    for (unsigned int n = 0; n < optList.size(); n++) {
        if (optList[n].Contains("binSize=")) {
            TString pitchStr = optList[n].Remove(0, 8);
            pitch = stod((string)pitchStr);
        }

        if (optList[n].Contains("stats")) stats = true;
    }

    Int_t nBinsX = (fMaxX - fMinX + 20) / pitch;
    Int_t nBinsY = (fMaxY - fMinY + 20) / pitch;

    fXYHisto = new TH2F("XY", "", nBinsX, fMinX - 10, fMinX + pitch * nBinsX, nBinsY, fMinY - 10,
                        fMinY + pitch * nBinsY);

    for (int ntck = 0; ntck < GetNumberOfTracks(); ntck++) {
        const auto& hits = GetTrack(ntck).GetHits();

        for (int nhit = 0; nhit < hits.GetNumberOfHits(); nhit++) {
            Double_t xPos = hits.GetX(nhit);
            Double_t yPos = hits.GetY(nhit);
            Double_t energy = hits.GetEnergy(nhit);

            fXYHisto->Fill(xPos, yPos, energy);
        }
    }

    TStyle style;
    style.SetPalette(1);
    style.SetOptStat(1001111);

    fXYHisto->SetStats(stats);

    char title[256];
    sprintf(title, "Event ID %d (XY)", this->GetID());
    fXYHisto->SetTitle(title);

    fXYHisto->GetXaxis()->SetTitle("X-axis (mm)");
    fXYHisto->GetXaxis()->SetTitleOffset(1.25);
    fXYHisto->GetXaxis()->SetTitleSize(1.25 * fXYHisto->GetXaxis()->GetTitleSize());
    fXYHisto->GetXaxis()->SetLabelSize(1.25 * fXYHisto->GetXaxis()->GetLabelSize());

    fXYHisto->GetYaxis()->SetTitle("Y-axis (mm)");
    fXYHisto->GetYaxis()->SetTitleOffset(1.75);
    fXYHisto->GetYaxis()->SetTitleSize(1.25 * fXYHisto->GetYaxis()->GetTitleSize());
    fXYHisto->GetYaxis()->SetLabelSize(1.25 * fXYHisto->GetYaxis()->GetLabelSize());
    fPad->cd(gridElement);

    return fXYHisto;
}

TH2F* TRestGeant4Event::GetXZHistogram(Int_t gridElement, vector<TString> optList) {
    if (fXZHisto) {
        delete fXZHisto;
        fXZHisto = nullptr;
    }

    Bool_t stats = false;
    Double_t pitch = 3;
    for (unsigned int n = 0; n < optList.size(); n++) {
        if (optList[n].Contains("binSize=")) {
            TString pitchStr = optList[n].Remove(0, 8);
            pitch = stod((string)pitchStr);
        }

        if (optList[n].Contains("stats")) stats = true;
    }

    Int_t nBinsX = (fMaxX - fMinX + 20) / pitch;
    Int_t nBinsZ = (fMaxZ - fMinZ + 20) / pitch;

    fXZHisto = new TH2F("XZ", "", nBinsX, fMinX - 10, fMinX + pitch * nBinsX, nBinsZ, fMinZ - 10,
                        fMinZ + pitch * nBinsZ);

    for (int ntck = 0; ntck < GetNumberOfTracks(); ntck++) {
        const auto& hits = GetTrack(ntck).GetHits();

        for (int nhit = 0; nhit < hits.GetNumberOfHits(); nhit++) {
            Double_t xPos = hits.GetX(nhit);
            Double_t zPos = hits.GetZ(nhit);
            Double_t energy = hits.GetEnergy(nhit);

            fXZHisto->Fill(xPos, zPos, energy);
        }
    }

    TStyle style;
    style.SetPalette(1);
    style.SetOptStat(1001111);

    fXZHisto->SetStats(stats);

    char title[256];
    sprintf(title, "Event ID %d (XZ)", this->GetID());
    fXZHisto->SetTitle(title);

    fXZHisto->GetXaxis()->SetTitle("X-axis (mm)");
    fXZHisto->GetXaxis()->SetTitleOffset(1.25);
    fXZHisto->GetXaxis()->SetTitleSize(1.25 * fXZHisto->GetXaxis()->GetTitleSize());
    fXZHisto->GetXaxis()->SetLabelSize(1.25 * fXZHisto->GetXaxis()->GetLabelSize());

    fXZHisto->GetYaxis()->SetTitle("Z-axis (mm)");
    fXZHisto->GetYaxis()->SetTitleOffset(1.75);
    fXZHisto->GetYaxis()->SetTitleSize(1.25 * fXZHisto->GetYaxis()->GetTitleSize());
    fXZHisto->GetYaxis()->SetLabelSize(1.25 * fXZHisto->GetYaxis()->GetLabelSize());
    fPad->cd(gridElement);

    return fXZHisto;
}

TH2F* TRestGeant4Event::GetYZHistogram(Int_t gridElement, vector<TString> optList) {
    if (fYZHisto) {
        delete fYZHisto;
        fYZHisto = nullptr;
    }

    Bool_t stats;
    Double_t pitch = 3;
    for (unsigned int n = 0; n < optList.size(); n++) {
        if (optList[n].Contains("binSize=")) {
            TString pitchStr = optList[n].Remove(0, 8);
            pitch = stod((string)pitchStr);
        }

        if (optList[n].Contains("stats")) stats = true;
    }

    Int_t nBinsY = (fMaxY - fMinY + 20) / pitch;
    Int_t nBinsZ = (fMaxZ - fMinZ + 20) / pitch;

    fYZHisto = new TH2F("YZ", "", nBinsY, fMinY - 10, fMinY + pitch * nBinsY, nBinsZ, fMinZ - 10,
                        fMinZ + pitch * nBinsZ);

    for (int ntck = 0; ntck < GetNumberOfTracks(); ntck++) {
        const auto& hits = GetTrack(ntck).GetHits();

        for (int nhit = 0; nhit < hits.GetNumberOfHits(); nhit++) {
            Double_t yPos = hits.GetY(nhit);
            Double_t zPos = hits.GetZ(nhit);
            Double_t energy = hits.GetEnergy(nhit);

            fYZHisto->Fill(yPos, zPos, energy);
        }
    }

    TStyle style;
    style.SetPalette(1);
    style.SetOptStat(1001111);  // Not Working :(

    fYZHisto->SetStats(stats);

    char title[256];
    sprintf(title, "Event ID %d (YZ)", this->GetID());
    fYZHisto->SetTitle(title);

    fYZHisto->GetXaxis()->SetTitle("Y-axis (mm)");
    fYZHisto->GetXaxis()->SetTitleOffset(1.25);
    fYZHisto->GetXaxis()->SetTitleSize(1.25 * fYZHisto->GetXaxis()->GetTitleSize());
    fYZHisto->GetXaxis()->SetLabelSize(1.25 * fYZHisto->GetXaxis()->GetLabelSize());

    fYZHisto->GetYaxis()->SetTitle("Z-axis (mm)");
    fYZHisto->GetYaxis()->SetTitleOffset(1.75);
    fYZHisto->GetYaxis()->SetTitleSize(1.25 * fYZHisto->GetYaxis()->GetTitleSize());
    fYZHisto->GetYaxis()->SetLabelSize(1.25 * fYZHisto->GetYaxis()->GetLabelSize());
    fPad->cd(gridElement);

    return fYZHisto;
}
/* }}} */

TH1D* TRestGeant4Event::GetXHistogram(Int_t gridElement, vector<TString> optList) {
    if (fXHisto) {
        delete fXHisto;
        fXHisto = nullptr;
    }

    Double_t pitch = 3;
    Bool_t stats = false;
    for (unsigned int n = 0; n < optList.size(); n++) {
        if (optList[n].Contains("binSize=")) {
            TString pitchStr = optList[n].Remove(0, 8);
            pitch = stod((string)pitchStr);
        }

        if (optList[n].Contains("stats")) stats = true;
    }

    Int_t nBinsX = (fMaxX - fMinX + 20) / pitch;

    fXHisto = new TH1D("X", "", nBinsX, fMinX - 10, fMinX + pitch * nBinsX);

    for (int ntck = 0; ntck < GetNumberOfTracks(); ntck++) {
        const auto& hits = GetTrack(ntck).GetHits();

        for (int nhit = 0; nhit < hits.GetNumberOfHits(); nhit++) {
            Double_t xPos = hits.GetX(nhit);
            Double_t energy = hits.GetEnergy(nhit);

            fXHisto->Fill(xPos, energy);
        }
    }

    TStyle style;
    style.SetPalette(1);

    fXHisto->SetStats(stats);

    char title[256];
    sprintf(title, "Event ID %d (X)", this->GetID());
    fXHisto->SetTitle(title);

    fXHisto->GetXaxis()->SetTitle("X-axis (mm)");
    fXHisto->GetXaxis()->SetTitleOffset(1.25);
    fXHisto->GetXaxis()->SetTitleSize(1.25 * fXHisto->GetXaxis()->GetTitleSize());
    fXHisto->GetXaxis()->SetLabelSize(1.25 * fXHisto->GetXaxis()->GetLabelSize());

    fPad->cd(gridElement);

    return fXHisto;
}

TH1D* TRestGeant4Event::GetZHistogram(Int_t gridElement, vector<TString> optList) {
    if (fZHisto) {
        delete fZHisto;
        fZHisto = nullptr;
    }

    Double_t pitch = 3;
    Bool_t stats = false;
    for (unsigned int n = 0; n < optList.size(); n++) {
        if (optList[n].Contains("binSize=")) {
            TString pitchStr = optList[n].Remove(0, 8);
            pitch = stod((string)pitchStr);
        }

        if (optList[n].Contains("stats")) stats = true;
    }

    Int_t nBinsZ = (fMaxZ - fMinZ + 20) / pitch;

    fZHisto = new TH1D("Z", "", nBinsZ, fMinZ - 10, fMinZ + pitch * nBinsZ);

    for (int ntck = 0; ntck < GetNumberOfTracks(); ntck++) {
        const auto& hits = GetTrack(ntck).GetHits();

        for (int nhit = 0; nhit < hits.GetNumberOfHits(); nhit++) {
            Double_t zPos = hits.GetZ(nhit);
            Double_t energy = hits.GetEnergy(nhit);

            fZHisto->Fill(zPos, energy);
        }
    }

    TStyle style;
    style.SetPalette(1);

    fZHisto->SetStats(stats);

    char title[256];
    sprintf(title, "Event ID %d (Z)", this->GetID());
    fZHisto->SetTitle(title);

    fZHisto->GetXaxis()->SetTitle("Z-axis (mm)");
    fZHisto->GetXaxis()->SetTitleOffset(1.25);
    fZHisto->GetXaxis()->SetTitleSize(1.25 * fZHisto->GetXaxis()->GetTitleSize());
    fZHisto->GetXaxis()->SetLabelSize(1.25 * fZHisto->GetXaxis()->GetLabelSize());

    fPad->cd(gridElement);

    return fZHisto;
}

TH1D* TRestGeant4Event::GetYHistogram(Int_t gridElement, vector<TString> optList) {
    if (fYHisto) {
        delete fYHisto;
        fYHisto = nullptr;
    }

    Double_t pitch = 3;
    Bool_t stats = false;
    for (unsigned int n = 0; n < optList.size(); n++) {
        if (optList[n].Contains("binSize=")) {
            TString pitchStr = optList[n].Remove(0, 8);
            pitch = stod((string)pitchStr);
        }

        if (optList[n].Contains("stats")) stats = true;
    }

    Int_t nBinsY = (fMaxY - fMinY + 20) / pitch;

    fYHisto = new TH1D("Y", "", nBinsY, fMinY - 10, fMinY + pitch * nBinsY);

    for (int ntck = 0; ntck < GetNumberOfTracks(); ntck++) {
        const auto& hits = GetTrack(ntck).GetHits();

        for (int nhit = 0; nhit < hits.GetNumberOfHits(); nhit++) {
            Double_t yPos = hits.GetY(nhit);
            Double_t energy = hits.GetEnergy(nhit);

            fYHisto->Fill(yPos, energy);
        }
    }

    TStyle style;
    style.SetPalette(1);

    fYHisto->SetStats(stats);

    char title[256];
    sprintf(title, "Event ID %d (Y)", this->GetID());
    fYHisto->SetTitle(title);

    fYHisto->GetXaxis()->SetTitle("Y-axis (mm)");
    fYHisto->GetXaxis()->SetTitleOffset(1.25);
    fYHisto->GetXaxis()->SetTitleSize(1.25 * fYHisto->GetXaxis()->GetTitleSize());
    fYHisto->GetXaxis()->SetLabelSize(1.25 * fYHisto->GetXaxis()->GetLabelSize());

    fPad->cd(gridElement);

    return fYHisto;
}

///////////////////////////////////////////////
/// \brief This method draws the geant4 event that it is available at the moment
/// in memory. It uses TGraph and TH2D ROOT drawing objects. This method generates
/// a drawing TPad with enough divisions to host all the options given by argument.
///
/// This method receives an optional argument as string that allows to control
/// which plots are generated. Different options can be passed to the string
/// separated by the options using colons, as "option1:option2:option3". Each
/// option will *generate a new independent drawing pad*.
///
/// The following options are allowed:
///
/// 1. **histX,Y,Z**: It generates a 1-dimensional TH1D histogram with the
/// projected event hits at the corresponding axis, the bins are weighted with
/// their correspoding energy. Histograms will accept as argument, between
/// parenthesis, a conventional ROOT option, i.e. `histX(rootOption)`, where
/// rootOption is any valid ROOT option as described by the
/// [THistPainter](https://root.cern/doc/master/classTHistPainter.html) ROOT
/// class. On top of that, we may pass additional options using the [ ] to
/// change the default binning size, which is set to 3, or draw the stats box,
/// i.e. `histX(Cont0,colz)[binSize=1,stats]`.
///
/// 2. **histXY,XZ,YZ**: It generates a bi-dimensional TH2D histogram with
/// the projected event hits at the corresponding plane, the bins are weighted
/// with their corresponding energy. Histograms will accept as argument, between
/// parenthesis, a conventional ROOT option, i.e. `histX(rootOption)`, where
/// rootOption is any valid ROOT option as described by the
/// [THistPainter](https://root.cern/doc/master/classTHistPainter.html) ROOT
/// class. On top of that, we may pass additional options using the [ ] to
/// change the default binning size, which is set to 3, or draw the stats box,
/// i.e. `histXY(Cont0,colz)[binSize=1,stats]`.
///
/// 3. **graphXY,XZ,YZ**: It generates a bi-dimensional TH2D histogram with
/// the projected event hits at the corresponding plane, the bins are weighted
/// with their corresponding energy. We may specify physical Geant4 processes
/// such as photoeletric or Compton, existing at the Geant4 processes list, you
/// may check the process names at the TRestGeant4Track source implementation,
/// till we get a better way of storing the processes list or retrieve them
/// directly from Geant4. Specifying a physical process between square brackets
/// [ ] will mark those hits at the event drawing pad that match those given
/// at the argument, i.e. `graphXY[eBrem,phot,compt]`.
///
/// 4. **print**: Prints in the screen terminal the output of the
/// TRestGeant4Event::PrintEvent method. This option will not generate a new
/// drawing pad.
///
/// Example 1:
/// \code
/// DrawEvent("graphXY[eBrem,phot,compt]:graphXZ[eIoni,annihil]")
/// \endcode
///
/// Example 2:
/// \code
/// DrawEvent("histX[binSize=1]:histY[binSize=2]:histZ[binSize=3]");
/// \endcode
///
/// If not argument is given, the default option list is equivalent to execute
/// the following:
///
/// \code
/// DrawEvent("graphXZ:graphYZ:histXZ(Cont0,colz):histYZ(Cont0,colz)")
/// \endcode
///
TPad* TRestGeant4Event::DrawEvent(const TString& option, Bool_t autoBoundaries) {
    vector<TString> optList = Vector_cast<string, TString>(TRestTools::GetOptions((string)option));

    if (autoBoundaries) SetBoundaries();

    // If no option is given. This is the default
    if (optList.size() == 0) {
        optList.push_back("graphXZ");
        optList.push_back("graphYZ");
        optList.push_back("histXZ(Cont0,colz)");
        optList.push_back("histYZ(Cont0,colz)");
    }

    for (unsigned int n = 0; n < optList.size(); n++) {
        if (optList[n] == "print") this->PrintEvent();
    }

    optList.erase(remove(optList.begin(), optList.end(), "print"), optList.end());

    unsigned int nPlots = optList.size();

    RestartPad(nPlots);

    for (unsigned int n = 0; n < nPlots; n++) {
        fPad->cd(n + 1);

        string optionStr = (string)optList[n];

        /* {{{ Retreiving drawEventType argument. The word key before we find ( or [
         * character. */
        TString drawEventType = optList[n];
        size_t startPos = optionStr.find("(");
        if (startPos == string::npos) startPos = optionStr.find("[");

        if (startPos != string::npos) drawEventType = optList[n](0, startPos);
        /* }}} */

        /* {{{ Retrieving drawOption argument. Inside normal brackets ().
         * We do not separate the different fields we take the full string. */
        string drawOption = "";
        size_t endPos = optionStr.find(")");
        if (endPos != string::npos) {
            TString drawOptionTmp = optList[n](startPos + 1, endPos - startPos - 1);

            drawOption = (string)drawOptionTmp;
            size_t pos = 0;
            while ((pos = drawOption.find(",", pos)) != string::npos) {
                drawOption.replace(pos, 1, ":");
                pos = pos + 1;
            }
        }
        /* }}} */

        /* {{{ Retrieving optList. Inside squared brackets [] and separated by colon
         * [Field1,Field2] */
        vector<TString> optList_2;

        startPos = optionStr.find("[");
        endPos = optionStr.find("]");

        if (endPos != string::npos) {
            TString tmpStr = optList[n](startPos + 1, endPos - startPos - 1);
            optList_2 = Vector_cast<string, TString>(Split((string)tmpStr, ","));
        }
        /* }}} */

        if (drawEventType == "graphXZ") {
            GetXZMultiGraph(n + 1, optList_2)->Draw("P");

            // Markers for physical processes have been already assigned in
            // GetXYMultigraph method
            if (fXZPcsMarker.size() > 0) fLegend_XZ->Draw("same");

            for (unsigned int m = 0; m < fXZPcsMarker.size(); m++) fXZPcsMarker[m]->Draw("P");
        } else if (drawEventType == "graphYZ") {
            GetYZMultiGraph(n + 1, optList_2)->Draw("P");

            if (fYZPcsMarker.size() > 0) fLegend_YZ->Draw("same");

            for (unsigned int m = 0; m < fYZPcsMarker.size(); m++) fYZPcsMarker[m]->Draw("P");
        } else if (drawEventType == "graphXY") {
            GetXYMultiGraph(n + 1, optList_2)->Draw("P");

            if (fXYPcsMarker.size() > 0) fLegend_XY->Draw("same");

            for (unsigned int m = 0; m < fXYPcsMarker.size(); m++) fXYPcsMarker[m]->Draw("P");
        } else if (drawEventType == "histXY") {
            GetXYHistogram(n + 1, optList_2)->Draw((TString)drawOption);
        } else if (drawEventType == "histXZ") {
            GetXZHistogram(n + 1, optList_2)->Draw((TString)drawOption);
        } else if (drawEventType == "histYZ") {
            GetYZHistogram(n + 1, optList_2)->Draw((TString)drawOption);
        } else if (drawEventType == "histX") {
            GetXHistogram(n + 1, optList_2)->Draw((TString)drawOption);
        } else if (drawEventType == "histY") {
            GetYHistogram(n + 1, optList_2)->Draw((TString)drawOption);
        } else if (drawEventType == "histZ") {
            GetZHistogram(n + 1, optList_2)->Draw((TString)drawOption);
        }
    }

    return fPad;
}
void TRestGeant4Event::PrintActiveVolumes() const {
    cout << "Number of active volumes : " << GetNumberOfActiveVolumes() << endl;
    for (int i = 0; i < fVolumeStoredNames.size(); i++) {
        if (isVolumeStored(i)) {
            cout << "Active volume " << i << ":" << fVolumeStoredNames[i] << " has been stored." << endl;
            cout << "Total energy deposit in volume " << i << ":" << fVolumeStoredNames[i] << " : "
                 << fVolumeDepositedEnergy[i] << " keV" << endl;
        } else
            cout << "Active volume " << i << ":" << fVolumeStoredNames[i] << " has not been stored" << endl;
    }
}

// TODO: Find how to place this so that we don't need to copy it in every source file
template <>
struct fmt::formatter<TVector3> : formatter<string> {
    auto format(TVector3 c, format_context& ctx) {
        string s = fmt::format("({:0.3f}, {:0.3f}, {:0.3f})", c.X(), c.Y(), c.Z());
        return formatter<string>::format(s, ctx);
    }
};

void TRestGeant4Event::PrintEvent(int maxTracks, int maxHits) const {
    TRestEvent::PrintEvent();

    print("- Total deposited energy: {}\n", ToEnergyString(fTotalDepositedEnergy));
    print(fg(color::medium_sea_green), "- Sensitive detectors total energy: {}\n",
          ToEnergyString(fSensitiveVolumeEnergy));

    print("- Primary source position: {} mm\n", fPrimaryPosition);

    for (int i = 0; i < GetNumberOfPrimaries(); i++) {
        const auto sourceNumberString =
            GetNumberOfPrimaries() <= 1 ? "" : format(" {} of {}", i + 1, GetNumberOfPrimaries());
        print("   - Source{} primary particle: {}\n", sourceNumberString, fPrimaryParticleNames[i]);
        print("     Direction: {}\n", fPrimaryDirections[i]);
        print(fg(color::light_golden_rod_yellow), "     Energy: {}\n", ToEnergyString(fPrimaryEnergies[i]));
    }

    print("\n");
    print("Total number of tracks: {}\n", GetNumberOfTracks());

    int nTracks = GetNumberOfTracks();
    if (maxTracks > 0 && maxTracks < GetNumberOfTracks()) {
        nTracks = min(maxTracks, int(GetNumberOfTracks()));
        print(fg(color::light_slate_gray), "Printing only the first {} tracks\n", nTracks);
    }

    for (int i = 0; i < nTracks; i++) {
        GetTrack(i).PrintTrack(maxHits);
    }
}

Bool_t TRestGeant4Event::ContainsProcessInVolume(Int_t processID, Int_t volumeID) const {
    for (const auto& track : fTracks) {
        if (track.ContainsProcessInVolume(processID, volumeID)) {
            return true;
        }
    }
    return false;
}

Bool_t TRestGeant4Event::ContainsProcessInVolume(const TString& processName, Int_t volumeID) const {
    const TRestGeant4Metadata* metadata = GetGeant4Metadata();
    if (metadata == nullptr) {
        return false;
    }
    const auto& processID = metadata->GetGeant4PhysicsInfo().GetProcessID(processName);
    for (const auto& track : fTracks) {
        if (track.ContainsProcessInVolume(processID, volumeID)) {
            return true;
        }
    }
    return false;
}

Bool_t TRestGeant4Event::ContainsParticle(const TString& particleName) const {
    for (const auto& track : fTracks) {
        if (track.GetParticleName() == particleName) {
            return true;
        }
    }
    return false;
}

Bool_t TRestGeant4Event::ContainsParticleInVolume(const TString& particleName, Int_t volumeID) const {
    for (const auto& track : fTracks) {
        if (track.GetParticleName() != particleName) {
            continue;
        }
        if (track.GetHits().GetNumberOfHitsInVolume(volumeID) > 0) {
            return true;
        }
    }
    return false;
}

const TRestGeant4Metadata* TRestGeant4Event::GetGeant4Metadata(const char* name) const {
    return dynamic_cast<TRestGeant4Metadata*>(fRun->GetMetadataClass(name));
}

void TRestGeant4Event::InitializeReferences(TRestRun* run) {
    TRestEvent::InitializeReferences(run);
    /*
    This introduces overhead to event loading, but hopefully its small enough.
    If this is a problem, we could rework this approach
     */
    for (auto& track : fTracks) {
        track.SetEvent(this);
    }
}

const set<string> TRestGeant4Event::GetUniqueParticles() const {
    set<string> result;
    for (const auto& track : fTracks) {
        result.insert(track.GetParticleName().Data());
    }
    return result;
}

map<string, map<string, double>> TRestGeant4Event::GetEnergyInVolumePerProcessMap() const {
    map<string, map<string, double>> result;
    for (const auto& [volume, particleProcessMap] : fEnergyInVolumePerParticlePerProcess) {
        for (const auto& [particle, processMap] : particleProcessMap) {
            for (const auto& [process, energy] : processMap) {
                result[volume][process] += energy;
            }
        }
    }
    return result;
}

map<string, map<string, double>> TRestGeant4Event::GetEnergyInVolumePerParticleMap() const {
    map<string, map<string, double>> result;
    for (const auto& [volume, particleProcessMap] : fEnergyInVolumePerParticlePerProcess) {
        for (const auto& [particle, processMap] : particleProcessMap) {
            for (const auto& [process, energy] : processMap) {
                result[volume][particle] += energy;
            }
        }
    }
    return result;
}

map<string, double> TRestGeant4Event::GetEnergyPerProcessMap() const {
    map<string, double> result;
    for (const auto& [volume, particleProcessMap] : fEnergyInVolumePerParticlePerProcess) {
        for (const auto& [particle, processMap] : particleProcessMap) {
            for (const auto& [process, energy] : processMap) {
                result[process] += energy;
            }
        }
    }
    return result;
}

map<string, double> TRestGeant4Event::GetEnergyPerParticleMap() const {
    map<string, double> result;
    for (const auto& [volume, particleProcessMap] : fEnergyInVolumePerParticlePerProcess) {
        for (const auto& [particle, processMap] : particleProcessMap) {
            for (const auto& [process, energy] : processMap) {
                result[particle] += energy;
            }
        }
    }
    return result;
}

map<string, double> TRestGeant4Event::GetEnergyInVolumeMap() const {
    map<string, double> result;
    for (const auto& [volume, particleProcessMap] : fEnergyInVolumePerParticlePerProcess) {
        for (const auto& [particle, processMap] : particleProcessMap) {
            for (const auto& [process, energy] : processMap) {
                result[volume] += energy;
            }
        }
    }
    return result;
}

map<string, map<string, map<string, double>>> TRestGeant4Event::GetEnergyInVolumePerParticlePerProcessMap()
    const {
    return fEnergyInVolumePerParticlePerProcess;
}

void TRestGeant4Event::AddEnergyInVolumeForParticleForProcess(Double_t energy, const string& volumeName,
                                                              const string& particleName,
                                                              const string& processName) {
    if (energy <= 0) {
        return;
    }
    fEnergyInVolumePerParticlePerProcess[volumeName][particleName][processName] += energy;
    fTotalDepositedEnergy += energy;
}
