///_______________________________________________________________________________
///_______________________________________________________________________________
///_______________________________________________________________________________
///
///
///             RESTSoft : Software for Rare Event Searches with TPCs
///
///             TRestGeant4EventViewer.h inherited from TRestEveEventViewer
///
///             nov 2015:   First concept
///                 Viewer class for a TRestGeant4Event
///                 Javier Galan/JuanAn Garcia
///_______________________________________________________________________________

#ifndef RestCore_TRestGeant4EventViewer
#define RestCore_TRestGeant4EventViewer

#include "TEveStraightLineSet.h"
#include "TRestEveEventViewer.h"
#include "TRestGeant4Event.h"

class TRestGeant4EventViewer : public TRestEveEventViewer {
   private:
    std::vector<TEveLine*> fHitConnectors;

    TRestGeant4Event* fG4Event = nullptr;
    const TRestGeant4Metadata* fG4Metadata = nullptr;

    TEveStraightLineSet* GetTrackEveDrawable(const TRestGeant4Track& track);

   public:
    void Initialize();
    void DeleteCurrentEvent();
    void AddEvent(TRestEvent* event);

    void NextTrackVertex(Int_t trkID, const TVector3& to);
    void AddTrack(Int_t trkID, Int_t parentID, const TVector3& from, const TString& name);
    void AddParentTrack(Int_t trkID, const TVector3& from, const TString& name);

    void AddText(const TString& text, const TVector3& at);
    void AddMarker(Int_t trkID, const TVector3& at, const TString& name);

    // Constructor
    TRestGeant4EventViewer();
    // Destructor
    ~TRestGeant4EventViewer();

    ClassDef(TRestGeant4EventViewer, 2);  // class inherited from TRestEventViewer
};
#endif
