#include <TDataMember.h>
#include <TRestGeant4AnalysisProcess.h>
#include <gtest/gtest.h>

#include <cmath>
#include <filesystem>

namespace {
const auto ancestryConfig =
    std::filesystem::path(__FILE__).parent_path().parent_path() / "files" / "Geant4AnalysisAncestry.rml";

class AncestryHits : public TRestGeant4Hits {
   public:
    AncestryHits(int volume, double energy) {
        AddHit(TVector3(1, 2, 3), energy, 2);
        fVolumeID.push_back(volume);
        fProcessID.push_back(0);
        fKineticEnergy.push_back(0);
    }
};

class AncestryTrack : public TRestGeant4Track {
   public:
    AncestryTrack(int id, int parent, bool outside, double deposit) {
        fTrackID = id;
        fParentID = parent;
        fParticleName = outside ? "gamma" : "e-";
        fInitialPosition = {11, 12, 13};
        fInitialKineticEnergy = 20;
        fCreatorProcess = outside ? "PrimaryGenerator" : "compt";
        SetHits(AncestryHits(outside ? 1 : 0, deposit));
    }
};

class AncestryEvent : public TRestGeant4Event {
   public:
    explicit AncestryEvent(int ancestors) {
        fPrimaryParticleNames = {"gamma"};
        fPrimaryEnergies = {20};
        fPrimaryDirections = {{0, 0, 1}};
        if (ancestors == 0) {
            fTracks.emplace_back(AncestryTrack(1, 0, false, 2));
        } else {
            for (int i = 0; i < ancestors; ++i) {
                const int parent = 2 * i + 1;
                fTracks.emplace_back(AncestryTrack(parent, 0, true, 0));
                fTracks.emplace_back(AncestryTrack(parent + 1, parent, false, 2.0 / ancestors));
            }
        }
        // Populate the persisted ID index through ROOT; there is no public synthetic-track inserter.
        const auto member = TRestGeant4Event::Class()->GetDataMember("fTrackIDToTrackIndex");
        auto& index = *reinterpret_cast<std::map<Int_t, Int_t>*>(
            reinterpret_cast<char*>(static_cast<TRestGeant4Event*>(this)) + member->GetOffset());
        for (size_t i = 0; i < fTracks.size(); ++i) {
            index[fTracks[i].GetTrackID()] = i;
        }
    }
};

class Geant4AnalysisAncestry : public testing::Test {
   protected:
    TRestRun run;
    TRestGeant4Metadata metadata;
    TRestAnalysisTree tree;
    TRestGeant4AnalysisProcess process;

    void Configure(TRestGeant4AnalysisProcess& target, TRestAnalysisTree& analysisTree) {
        target.LoadConfig(ancestryConfig.string(), "g4Ana");
        target.SetRunInfo(&run);
        target.SetAnalysisTree(&analysisTree);
        target.RegisterAllObservables();
        target.InitProcess();
    }

    void SetUp() override {
        ASSERT_EQ(metadata.LoadConfigFromFile(ancestryConfig.string(), "ancestryMetadata"), 0);
        auto& geometry = const_cast<TRestGeant4GeometryInfo&>(metadata.GetGeant4GeometryInfo());
        geometry.fPhysicalToLogicalVolumeMap["gasVolume"] = "gasLogical";
        geometry.fPhysicalToLogicalVolumeMap["outsideVolume"] = "outsideLogical";
        geometry.InsertVolumeName(0, "gasVolume");
        geometry.InsertVolumeName(1, "outsideVolume");
        metadata.InsertSensitiveVolume("gasVolume");
        metadata.SetActiveVolume("gasVolume", 1);
        metadata.SetActiveVolume("outsideVolume", 1);
        run.AddMetadata(&metadata);
        Configure(process, tree);
    }

    void Process(int ancestors) {
        AncestryEvent event(ancestors);
        ASSERT_NE(process.ProcessEvent(&event), nullptr);
    }

    std::vector<TString> ObservableNames(TRestAnalysisTree& analysisTree) {
        std::vector<TString> names;
        for (int i = 0; i < analysisTree.GetNumberOfObservables(); ++i) {
            names.push_back(analysisTree.GetObservableName(i));
        }
        return names;
    }

    void ExpectMissing() {
        for (const auto* suffix : {"PositionX", "PositionY", "PositionZ", "Energy"}) {
            const std::string name = std::string("g4Ana_firstTrackInSensitive") + suffix;
            ASSERT_GE(tree.GetObservableID(name), 0);
            EXPECT_TRUE(std::isnan(tree.GetObservableValue<double>(name)));
        }
        for (const auto* suffix : {"Particle", "ParentParticle", "CreatorProcess", "VolumeName"}) {
            const std::string name = std::string("g4Ana_firstTrackInSensitive") + suffix;
            ASSERT_GE(tree.GetObservableID(name), 0);
            EXPECT_TRUE(tree.GetObservableValue<std::string>(name).empty());
        }
        EXPECT_FALSE(tree.GetObservableValue<bool>("g4Ana_firstTrackInSensitiveOk"));
    }

    void ExpectSelected(bool unique) {
        EXPECT_DOUBLE_EQ(tree.GetObservableValue<double>("g4Ana_firstTrackInSensitivePositionX"), 11);
        EXPECT_DOUBLE_EQ(tree.GetObservableValue<double>("g4Ana_firstTrackInSensitivePositionY"), 12);
        EXPECT_DOUBLE_EQ(tree.GetObservableValue<double>("g4Ana_firstTrackInSensitivePositionZ"), 13);
        EXPECT_DOUBLE_EQ(tree.GetObservableValue<double>("g4Ana_firstTrackInSensitiveEnergy"), 20);
        EXPECT_EQ(tree.GetObservableValue<std::string>("g4Ana_firstTrackInSensitiveParticle"), "gamma");
        EXPECT_TRUE(
            tree.GetObservableValue<std::string>("g4Ana_firstTrackInSensitiveParentParticle").empty());
        EXPECT_EQ(tree.GetObservableValue<std::string>("g4Ana_firstTrackInSensitiveCreatorProcess"),
                  "PrimaryGenerator");
        EXPECT_EQ(tree.GetObservableValue<std::string>("g4Ana_firstTrackInSensitiveVolumeName"),
                  "outsideVolume");
        EXPECT_EQ(tree.GetObservableValue<bool>("g4Ana_firstTrackInSensitiveOk"), unique);
    }
};

TEST_F(Geant4AnalysisAncestry, MissingPresentMissing) {
    Process(0);
    ExpectMissing();
    const auto names = ObservableNames(tree);
    tree.Fill();
    Process(1);
    ExpectSelected(true);
    EXPECT_EQ(ObservableNames(tree), names);
    tree.Fill();
    Process(0);
    ExpectMissing();
    EXPECT_EQ(ObservableNames(tree), names);
}

TEST_F(Geant4AnalysisAncestry, MultipleAncestorsKeepExistingSelection) {
    Process(2);
    ExpectSelected(false);
    tree.Fill();
    Process(0);
    ExpectMissing();
}

TEST_F(Geant4AnalysisAncestry, DifferentThreadHistoriesHaveCompatibleObservables) {
    Process(0);
    TRestAnalysisTree otherTree;
    TRestGeant4AnalysisProcess otherProcess;
    Configure(otherProcess, otherTree);
    AncestryEvent event(1);
    ASSERT_NE(otherProcess.ProcessEvent(&event), nullptr);
    ASSERT_EQ(ObservableNames(tree), ObservableNames(otherTree));
    for (int i = 0; i < tree.GetNumberOfObservables(); ++i) {
        EXPECT_EQ(tree.GetObservableType(i), otherTree.GetObservableType(i));
    }

    TRestAnalysisTree output;
    // Exercise the same copy operation used when the runner combines thread results.
    for (auto* input : {&tree, &otherTree}) {
        for (int i = 0; i < input->GetNumberOfObservables(); ++i) {
            output.SetObservable(i, input->GetObservable(i));
        }
        output.Fill();
    }
    EXPECT_EQ(output.GetEntries(), 2);
}
}  // namespace
