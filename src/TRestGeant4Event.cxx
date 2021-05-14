///______________________________________________________________________________
///______________________________________________________________________________
///______________________________________________________________________________
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
#include <TStyle.h>

#include <vector>

#include "TRestStringHelper.h"
#include "TRestTools.h"

using namespace std;

ClassImp(TRestGeant4Event);

TRestGeant4Event::TRestGeant4Event() : { Initialize(); }

TRestGeant4Event::~TRestGeant4Event() = default;

void TRestGeant4Event::Initialize() {
    TRestEvent::Initialize();

    fPrimaryParticleName.clear();
    fPrimaryEventDirection.clear();
    fPrimaryEventEnergy.clear();
    fPrimaryEventOrigin.SetXYZ(0, 0, 0);

    fTrack.clear();
    fNTracks = 0;

    // ClearVolumes();
    fNVolumes = 0;

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
    fMaxSubEventID = 0;

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

void TRestGeant4Event::SetTrackSubEventID(Int_t n, Int_t id) {
    fTrack[n].SetSubEventID(id);
    if (fMaxSubEventID < id) fMaxSubEventID = id;
}

void TRestGeant4Event::AddTrack(TRestGeant4Track trk) {
    fTrack.push_back(trk);
    fNTracks = fTrack.size();
    fTotalDepositedEnergy += trk.GetTotalDepositedEnergy();
    for (int n = 0; n < GetNumberOfActiveVolumes(); n++)
        fVolumeDepositedEnergy[n] += trk.GetEnergyInVolume(n);
}

Double_t TRestGeant4Event::GetTotalDepositedEnergyFromTracks() {
    Double_t eDep = 0;

    for (int tk = 0; tk < GetNumberOfTracks(); tk++) eDep += GetTrack(tk)->GetTotalDepositedEnergy();

    return eDep;
}

TVector3 TRestGeant4Event::GetMeanPositionInVolume(Int_t volID) {
    TVector3 pos;
    Double_t eDep = 0;

    for (int t = 0; t < GetNumberOfTracks(); t++) {
        TRestGeant4Track* tck = GetTrack(t);
        if (tck->GetEnergyInVolume(volID) > 0) {
            pos += tck->GetMeanPositionInVolume(volID) * tck->GetEnergyInVolume(volID);

            eDep += tck->GetEnergyInVolume(volID);
        }
    }

    if (eDep == 0) {
        Double_t nan = TMath::QuietNaN();
        return TVector3(nan, nan, nan);
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
TVector3 TRestGeant4Event::GetPositionDeviationInVolume(Int_t volID) {
    TVector3 meanPos = this->GetMeanPositionInVolume(volID);

    Double_t nan = TMath::QuietNaN();
    if (meanPos == TVector3(nan, nan, nan)) return TVector3(nan, nan, nan);

    TRestHits hitsInVolume = GetHitsInVolume(volID);

    Double_t eDep = 0;
    TVector3 deviation = TVector3(0, 0, 0);

    for (int n = 0; n < hitsInVolume.GetNumberOfHits(); n++) {
        Double_t en = hitsInVolume.GetEnergy(n);
        TVector3 diff = meanPos - hitsInVolume.GetPosition(n);
        diff.SetXYZ(diff.X() * diff.X(), diff.Y() * diff.Y(), diff.Z() * diff.Z());

        deviation = deviation + en * diff;

        eDep += en;
    }
    deviation = (1. / eDep) * deviation;
    return deviation;
}

///////////////////////////////////////////////
/// \brief Function to get the position (TVector3) of the first track that deposits energy in specified
/// volume. If no hit is found for the volume, returns `TVector3(nan, nan, nan)` vector.
///
///
/// \param volID Int_t specifying volume ID
///
TVector3 TRestGeant4Event::GetFirstPositionInVolume(Int_t volID) {
    for (int t = 0; t < GetNumberOfTracks(); t++)
        if (GetTrack(t)->GetEnergyInVolume(volID) > 0) return GetTrack(t)->GetFirstPositionInVolume(volID);

    TVector3 pos;
    Double_t nan = TMath::QuietNaN();
    return TVector3(nan, nan, nan);
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
TVector3 TRestGeant4Event::GetLastPositionInVolume(Int_t volID) {
    for (int t = GetNumberOfTracks() - 1; t >= 0; t--)
        if (GetTrack(t)->GetEnergyInVolume(volID) > 0) return GetTrack(t)->GetLastPositionInVolume(volID);

    TVector3 pos;
    Double_t nan = TMath::QuietNaN();
    return TVector3(nan, nan, nan);
}

TRestGeant4Track* TRestGeant4Event::GetTrackByID(int id) {
    for (int i = 0; i < fNTracks; i++)
        if (fTrack[i].GetTrackID() == id) return &fTrack[i];
    return nullptr;
}

///////////////////////////////////////////////
/// \brief Function that returns the total number of hits in the Geant4 event. If
/// a specific volume is given as argument only the hits of that specific volume
/// will be counted.
///
Int_t TRestGeant4Event::GetNumberOfHits(Int_t volID) {
    Int_t hits = 0;
    for (int i = 0; i < fNTracks; i++) {
        hits += GetTrack(i)->GetNumberOfHits(volID);
    }
    return hits;
}

///////////////////////////////////////////////
/// \brief Function that returns all the hit depositions in the Geant4 event. If
/// a specific volume is given as argument only the hits of that specific volume
/// will be added to the TRestHits returned object.
///
TRestHits TRestGeant4Event::GetHits(Int_t volID) {
    TRestHits hits;
    for (int t = 0; t < fNTracks; t++) {
        TRestGeant4Hits* g4Hits = GetTrack(t)->GetHits();
        for (int n = 0; n < g4Hits->GetNumberOfHits(); n++) {
            if (volID != -1 && g4Hits->GetVolumeId(n) != volID) continue;

            Double_t x = g4Hits->GetX(n);
            Double_t y = g4Hits->GetY(n);
            Double_t z = g4Hits->GetZ(n);
            Double_t en = g4Hits->GetEnergy(n);

            hits.AddHit(x, y, z, en);
        }
    }
    return hits;
}

Int_t TRestGeant4Event::GetNumberOfTracksForParticle(const TString& parName) {
    Int_t tracks = 0;
    for (Int_t t = 0; t < GetNumberOfTracks(); t++)
        if (GetTrack(t)->GetParticleName() == parName) tracks++;

    return tracks;
}

Int_t TRestGeant4Event::GetEnergyDepositedByParticle(const TString& parName) {
    Double_t en = 0;
    for (Int_t t = 0; t < GetNumberOfTracks(); t++) {
        if (GetTrack(t)->GetParticleName() == parName) en += GetTrack(t)->GetEnergy();
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
    for (int iTrack = 0; iTrack < this->GetNumberOfTracks(); iTrack++) {
        Int_t iHits = GetTrack(iTrack)->GetNumberOfHits();
        TRestGeant4Hits* hits = GetTrack(iTrack)->GetHits();

        for (int iHit = 0; iHit < iHits; iHit++) {
            Double_t x = hits->GetX(iHit);
            Double_t y = hits->GetY(iHit);
            Double_t z = hits->GetZ(iHit);

            Double_t en = hits->GetEnergy(iHit);

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
    if (fXYHitGraph != nullptr) {
        delete[] fXYHitGraph;
        fXYHitGraph = nullptr;
    }
    fXYHitGraph = new TGraph[fTotalHits];

    fXYMultiGraph = new TMultiGraph();

    if (fLegend_XY != nullptr) {
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

    for (auto& i : fXYPcsMarker) delete i;
    fXYPcsMarker.clear();

    legendAdded.clear();
    for (unsigned int p = 0; p < pcsList.size(); p++) legendAdded.push_back(0);

    Int_t cnt = 0;
    for (int iTrack = 0; iTrack < GetNumberOfTracks(); iTrack++) {
        TRestGeant4Hits* hits = GetTrack(iTrack)->GetHits();

        EColor color = GetTrack(iTrack)->GetParticleColor();

        for (int iHit = 0; iHit < hits->GetNumberOfHits(); iHit++) {
            Double_t xPos = hits->GetX(iHit);
            Double_t yPos = hits->GetY(iHit);
            Double_t energy = hits->GetEnergy(iHit);

            for (unsigned int p = 0; p < pcsList.size(); p++) {
                if (GetTrack(iTrack)->GetProcessName(hits->GetProcess(iHit)) == pcsList[p]) {
                    auto pcsGraph = new TGraph(1);

                    pcsGraph->SetPoint(0, xPos, yPos);
                    pcsGraph->SetMarkerColor(kBlack);
                    pcsGraph->SetMarkerSize(3);
                    pcsGraph->SetMarkerStyle(30 + p);

                    fXYPcsMarker.push_back(pcsGraph);

                    if (legendAdded[p] == 0) {
                        fLegend_XY->AddEntry(pcsGraph,
                                             GetTrack(iTrack)->GetProcessName(hits->GetProcess(iHit)), "p");
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
    if (fYZHitGraph != nullptr) {
        delete[] fYZHitGraph;
        fYZHitGraph = nullptr;
    }
    fYZHitGraph = new TGraph[fTotalHits];

    fYZMultiGraph = new TMultiGraph();

    if (fLegend_YZ != nullptr) {
        delete fLegend_YZ;
        fLegend_YZ = nullptr;
    }

    fLegend_YZ = new TLegend(0.75, 0.75, 0.9, 0.85);

    char title[256];
    sprintf(title, "Event ID %d (YZ)", this->GetID());
    fPad->cd(gridElement)->DrawFrame(fMinY - 10, fMinZ - 10, fMaxY + 10, fMaxZ + 10, title);

    for (auto& n : fYZPcsMarker) delete n;
    fYZPcsMarker.clear();

    Double_t m = 1, n = 0;
    if (fMaxEnergy - fMinEnergy > 0) {
        m = (maxPointSize - minPointSize) / (fMaxEnergy - fMinEnergy);
        n = maxPointSize - m * fMaxEnergy;
    }

    legendAdded.clear();
    for (unsigned int p = 0; p < pcsList.size(); p++) legendAdded.push_back(0);

    Int_t cnt = 0;
    for (int iTrack = 0; iTrack < GetNumberOfTracks(); iTrack++) {
        TRestGeant4Hits* hits = GetTrack(iTrack)->GetHits();

        EColor color = GetTrack(iTrack)->GetParticleColor();

        for (int iHit = 0; iHit < hits->GetNumberOfHits(); iHit++) {
            Double_t yPos = hits->GetY(iHit);
            Double_t zPos = hits->GetZ(iHit);
            Double_t energy = hits->GetEnergy(iHit);

            for (unsigned int p = 0; p < pcsList.size(); p++) {
                if (GetTrack(iTrack)->GetProcessName(hits->GetProcess(iHit)) == pcsList[p]) {
                    TGraph* pcsGraph = new TGraph(1);

                    pcsGraph->SetPoint(0, yPos, zPos);
                    pcsGraph->SetMarkerColor(kBlack);
                    pcsGraph->SetMarkerSize(3);
                    pcsGraph->SetMarkerStyle(30 + p);

                    fYZPcsMarker.push_back(pcsGraph);

                    if (legendAdded[p] == 0) {
                        fLegend_YZ->AddEntry(pcsGraph,
                                             GetTrack(iTrack)->GetProcessName(hits->GetProcess(iHit)), "p");
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
    if (fXZHitGraph != nullptr) {
        delete[] fXZHitGraph;
        fXZHitGraph = nullptr;
    }
    fXZHitGraph = new TGraph[fTotalHits];

    fXZMultiGraph = new TMultiGraph();

    if (fLegend_XZ != nullptr) {
        delete fLegend_XZ;
        fLegend_XZ = nullptr;
    }

    fLegend_XZ = new TLegend(0.75, 0.75, 0.9, 0.85);

    char title[256];
    sprintf(title, "Event ID %d (XZ)", this->GetID());
    fPad->cd(gridElement)->DrawFrame(fMinX - 10, fMinZ - 10, fMaxX + 10, fMaxZ + 10, title);

    for (auto& n : fXZPcsMarker) delete n;
    fXZPcsMarker.clear();

    Double_t m = 1, n = 0;
    if (fMaxEnergy - fMinEnergy > 0) {
        m = (maxPointSize - minPointSize) / (fMaxEnergy - fMinEnergy);
        n = maxPointSize - m * fMaxEnergy;
    }

    legendAdded.clear();
    for (unsigned int p = 0; p < pcsList.size(); p++) legendAdded.push_back(0);

    Int_t cnt = 0;
    for (int iTrack = 0; iTrack < GetNumberOfTracks(); iTrack++) {
        TRestGeant4Hits* hits = GetTrack(iTrack)->GetHits();

        EColor color = GetTrack(iTrack)->GetParticleColor();

        for (int iHit = 0; iHit < hits->GetNumberOfHits(); iHit++) {
            Double_t xPos = hits->GetX(iHit);
            Double_t zPos = hits->GetZ(iHit);
            Double_t energy = hits->GetEnergy(iHit);

            for (unsigned int p = 0; p < pcsList.size(); p++) {
                if (GetTrack(iTrack)->GetProcessName(hits->GetProcess(iHit)) == pcsList[p]) {
                    auto pcsGraph = new TGraph(1);

                    pcsGraph->SetPoint(0, xPos, zPos);
                    pcsGraph->SetMarkerColor(kBlack);
                    pcsGraph->SetMarkerSize(3);
                    pcsGraph->SetMarkerStyle(30 + p);

                    fXZPcsMarker.push_back(pcsGraph);

                    if (legendAdded[p] == 0) {
                        fLegend_XZ->AddEntry(pcsGraph,
                                             GetTrack(iTrack)->GetProcessName(hits->GetProcess(iHit)), "p");
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
    if (fXYHisto != nullptr) {
        delete fXYHisto;
        fXYHisto = nullptr;
    }

    Bool_t stats = false;
    Double_t pitch = 3;
    for (auto& n : optList) {
        if (n.Contains("binSize=")) {
            TString pitchStr = n.Remove(0, 8);
            pitch = stod((string)pitchStr);
        }

        if (n.Contains("stats")) stats = true;
    }

    Int_t nBinsX = (fMaxX - fMinX + 20) / pitch;
    Int_t nBinsY = (fMaxY - fMinY + 20) / pitch;

    fXYHisto = new TH2F("XY", "", nBinsX, fMinX - 10, fMinX + pitch * nBinsX, nBinsY, fMinY - 10,
                        fMinY + pitch * nBinsY);

    for (int iTrack = 0; iTrack < GetNumberOfTracks(); iTrack++) {
        TRestGeant4Hits* hits = GetTrack(iTrack)->GetHits();

        for (int iHit = 0; iHit < hits->GetNumberOfHits(); iHit++) {
            Double_t xPos = hits->GetX(iHit);
            Double_t yPos = hits->GetY(iHit);
            Double_t energy = hits->GetEnergy(iHit);

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
    if (fXZHisto != nullptr) {
        delete fXZHisto;
        fXZHisto = nullptr;
    }

    Bool_t stats = false;
    Double_t pitch = 3;
    for (auto& n : optList) {
        if (n.Contains("binSize=")) {
            TString pitchStr = n.Remove(0, 8);
            pitch = stod((string)pitchStr);
        }

        if (n.Contains("stats")) stats = true;
    }

    Int_t nBinsX = (fMaxX - fMinX + 20) / pitch;
    Int_t nBinsZ = (fMaxZ - fMinZ + 20) / pitch;

    fXZHisto = new TH2F("XZ", "", nBinsX, fMinX - 10, fMinX + pitch * nBinsX, nBinsZ, fMinZ - 10,
                        fMinZ + pitch * nBinsZ);

    for (int iTrack = 0; iTrack < GetNumberOfTracks(); iTrack++) {
        TRestGeant4Hits* hits = GetTrack(iTrack)->GetHits();

        for (int iHit = 0; iHit < hits->GetNumberOfHits(); iHit++) {
            Double_t xPos = hits->GetX(iHit);
            Double_t zPos = hits->GetZ(iHit);
            Double_t energy = hits->GetEnergy(iHit);

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
    if (fYZHisto != nullptr) {
        delete fYZHisto;
        fYZHisto = nullptr;
    }

    Bool_t stats;
    Double_t pitch = 3;
    for (auto& n : optList) {
        if (n.Contains("binSize=")) {
            TString pitchStr = n.Remove(0, 8);
            pitch = stod((string)pitchStr);
        }

        if (n.Contains("stats")) stats = true;
    }

    Int_t nBinsY = (fMaxY - fMinY + 20) / pitch;
    Int_t nBinsZ = (fMaxZ - fMinZ + 20) / pitch;

    fYZHisto = new TH2F("YZ", "", nBinsY, fMinY - 10, fMinY + pitch * nBinsY, nBinsZ, fMinZ - 10,
                        fMinZ + pitch * nBinsZ);

    for (int iTrack = 0; iTrack < GetNumberOfTracks(); iTrack++) {
        TRestGeant4Hits* hits = GetTrack(iTrack)->GetHits();

        for (int iHit = 0; iHit < hits->GetNumberOfHits(); iHit++) {
            Double_t yPos = hits->GetY(iHit);
            Double_t zPos = hits->GetZ(iHit);
            Double_t energy = hits->GetEnergy(iHit);

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
    if (fXHisto != nullptr) {
        delete fXHisto;
        fXHisto = nullptr;
    }

    Double_t pitch = 3;
    Bool_t stats = false;
    for (auto& n : optList) {
        if (n.Contains("binSize=")) {
            TString pitchStr = n.Remove(0, 8);
            pitch = stod((string)pitchStr);
        }

        if (n.Contains("stats")) stats = true;
    }

    Int_t nBinsX = (fMaxX - fMinX + 20) / pitch;

    fXHisto = new TH1D("X", "", nBinsX, fMinX - 10, fMinX + pitch * nBinsX);

    for (int iTrack = 0; iTrack < GetNumberOfTracks(); iTrack++) {
        TRestGeant4Hits* hits = GetTrack(iTrack)->GetHits();

        for (int iHit = 0; iHit < hits->GetNumberOfHits(); iHit++) {
            Double_t xPos = hits->GetX(iHit);
            Double_t energy = hits->GetEnergy(iHit);

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
    if (fZHisto != nullptr) {
        delete fZHisto;
        fZHisto = nullptr;
    }

    Double_t pitch = 3;
    Bool_t stats = false;
    for (auto& n : optList) {
        if (n.Contains("binSize=")) {
            TString pitchStr = n.Remove(0, 8);
            pitch = stod((string)pitchStr);
        }

        if (n.Contains("stats")) stats = true;
    }

    Int_t nBinsZ = (fMaxZ - fMinZ + 20) / pitch;

    fZHisto = new TH1D("Z", "", nBinsZ, fMinZ - 10, fMinZ + pitch * nBinsZ);

    for (int indexTrack = 0; indexTrack < GetNumberOfTracks(); indexTrack++) {
        TRestGeant4Hits* hits = GetTrack(indexTrack)->GetHits();

        for (int indexHit = 0; indexHit < hits->GetNumberOfHits(); indexHit++) {
            Double_t zPos = hits->GetZ(indexHit);
            Double_t energy = hits->GetEnergy(indexHit);

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
    if (fYHisto != nullptr) {
        delete fYHisto;
        fYHisto = nullptr;
    }

    Double_t pitch = 3;
    Bool_t stats = false;
    for (auto& n : optList) {
        if (n.Contains("binSize=")) {
            TString pitchStr = n.Remove(0, 8);
            pitch = stod((string)pitchStr);
        }

        if (n.Contains("stats")) stats = true;
    }

    Int_t nBinsY = (fMaxY - fMinY + 20) / pitch;

    fYHisto = new TH1D("Y", "", nBinsY, fMinY - 10, fMinY + pitch * nBinsY);

    for (int iTrack = 0; iTrack < GetNumberOfTracks(); iTrack++) {
        TRestGeant4Hits* hits = GetTrack(iTrack)->GetHits();

        for (int iHit = 0; iHit < hits->GetNumberOfHits(); iHit++) {
            Double_t yPos = hits->GetY(iHit);
            Double_t energy = hits->GetEnergy(iHit);

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

TPad* TRestGeant4Event::DrawEvent(const TString& option, Bool_t autoBoundaries) {
    vector<TString> optList = Vector_cast<string, TString>(TRestTools::GetOptions((string)option));

    if (autoBoundaries) SetBoundaries();

    // If no option is given. This is the default
    if (optList.empty()) {
        optList.emplace_back("graphXZ");
        optList.emplace_back("graphYZ");
        optList.emplace_back("histXZ(Cont0,colz)");
        optList.emplace_back("histYZ(Cont0,colz)");
    }

    for (auto& n : optList) {
        if (n == "print") this->PrintEvent();
    }

    optList.erase(remove(optList.begin(), optList.end(), "print"), optList.end());

    unsigned int nPlots = optList.size();

    RestartPad(nPlots);

    for (unsigned int n = 0; n < nPlots; n++) {
        fPad->cd(n + 1);

        string optionStr = (string)optList[n];

        /* {{{ Retrieving drawEventType argument. The word key before we find ( or [
         * character. */
        TString drawEventType = optList[n];
        size_t startPos = optionStr.find('(');
        if (startPos == string::npos) startPos = optionStr.find('[');

        if (startPos != string::npos) drawEventType = optList[n](0, startPos);
        /* }}} */

        /* {{{ Retrieving drawOption argument. Inside normal brackets ().
         * We do not separate the different fields we take the full string. */
        string drawOption;
        size_t endPos = optionStr.find(')');
        if (endPos != string::npos) {
            TString drawOptionTmp = optList[n](startPos + 1, endPos - startPos - 1);

            drawOption = (string)drawOptionTmp;
            size_t pos = 0;
            while ((pos = drawOption.find(',', pos)) != string::npos) {
                drawOption.replace(pos, 1, ":");
                pos = pos + 1;
            }
        }
        /* }}} */

        /* {{{ Retrieving optList. Inside squared brackets [] and separated by colon
         * [Field1,Field2] */
        vector<TString> optList_2;

        startPos = optionStr.find('[');
        endPos = optionStr.find(']');

        if (endPos != string::npos) {
            TString tmpStr = optList[n](startPos + 1, endPos - startPos - 1);
            optList_2 = Vector_cast<string, TString>(Split((string)tmpStr, ","));
        }
        /* }}} */

        if (drawEventType == "graphXZ") {
            GetXZMultiGraph(n + 1, optList_2)->Draw("P");

            // Markers for physical processes have been already assigned in
            // GetXYMultigraph method
            if (!fYZPcsMarker.empty()) fLegend_XZ->Draw("same");

            for (auto& m : fXZPcsMarker) m->Draw("P");
        } else if (drawEventType == "graphYZ") {
            GetYZMultiGraph(n + 1, optList_2)->Draw("P");

            if (!fYZPcsMarker.empty()) fLegend_YZ->Draw("same");

            for (auto& m : fYZPcsMarker) m->Draw("P");
        } else if (drawEventType == "graphXY") {
            GetXYMultiGraph(n + 1, optList_2)->Draw("P");

            if (!fYZPcsMarker.empty()) fLegend_XY->Draw("same");

            for (auto& m : fXYPcsMarker) m->Draw("P");
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
void TRestGeant4Event::PrintActiveVolumes() {
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
void TRestGeant4Event::PrintEvent(int maxTracks, int maxHits) {
    TRestEvent::PrintEvent();

    cout.precision(4);

    cout << "Total energy : " << fTotalDepositedEnergy << " keV" << endl;
    cout << "Sensitive volume energy : " << fSensitiveVolumeEnergy << " keV" << endl;
    cout << "Source origin : (" << fPrimaryEventOrigin.X() << "," << fPrimaryEventOrigin.Y() << ","
         << fPrimaryEventOrigin.Z() << ") mm" << endl;

    for (int n = 0; n < GetNumberOfPrimaries(); n++) {
        TVector3* dir = &fPrimaryEventDirection[n];
        cout << "Source " << n << " Particle name : " << GetPrimaryEventParticleName(n) << endl;
        cout << "Source " << n << " direction : (" << dir->X() << "," << dir->Y() << "," << dir->Z() << ")"
             << endl;
        cout << "Source " << n << " energy : " << fPrimaryEventEnergy[n] << " keV" << endl;
    }

    cout << "Number of active volumes : " << GetNumberOfActiveVolumes() << endl;
    for (int i = 0; i < GetNumberOfActiveVolumes(); i++) {
        if (isVolumeStored(i)) {
            cout << "Active volume " << i << ":"
                 << " has been stored." << endl;
            cout << "Total energy deposit in volume " << i << ":"
                 << " : " << fVolumeDepositedEnergy[i] << " keV" << endl;
        } else
            cout << "Active volume " << i << ":"
                 << " has not been stored" << endl;
    }

    cout << "--------------------------------------------------------------------" << endl;
    cout << "Total number of tracks : " << fNTracks << endl;

    int numberOfTracks = GetNumberOfTracks();
    if (maxTracks > 0) {
        numberOfTracks = min(maxTracks, GetNumberOfTracks());
        cout << " Printing only the first " << numberOfTracks << " tracks" << endl;
    }

    for (int n = 0; n < numberOfTracks; n++) GetTrack(n)->PrintTrack(maxHits);
}
