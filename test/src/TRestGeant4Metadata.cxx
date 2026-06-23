
#include <TRestGeant4Metadata.h>
#include <TRestGeant4PhysicsInfo.h>
#include <gtest/gtest.h>

#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

const auto filesPath = fs::path(__FILE__).parent_path().parent_path() / "files";
const auto geant4MetadataRml = filesPath / "TRestGeant4Example.rml";

TEST(TRestGeant4Metadata, TestFiles) {
    cout << "Test files path: " << filesPath << endl;

    // Check dir exists and is a directory
    EXPECT_TRUE(fs::is_directory(filesPath));
    // Check it's not empty
    EXPECT_TRUE(!fs::is_empty(filesPath));

    // All used files in this tests
    EXPECT_TRUE(fs::exists(geant4MetadataRml));
}

TEST(TRestGeant4Metadata, Default) {
    TRestGeant4Metadata restGeant4Metadata;

    restGeant4Metadata.PrintMetadata();

    EXPECT_TRUE(restGeant4Metadata.GetSeed() == 0);
}

TEST(TRestGeant4Metadata, CopyPreservesParticleSources) {
    TRestGeant4Metadata original;
    auto source = new TRestGeant4ParticleSource();
    source->SetName("Fe55 source");
    source->SetParticleName("Fe55");
    source->SetEnergy(5.9);
    source->SetEnergyDistributionRange(TVector2(5.8, 6.0));
    original.AddParticleSource(source);

    TRestGeant4Metadata copy(original);
    ASSERT_EQ(copy.GetNumberOfSources(), 1);
    EXPECT_NE(copy.GetParticleSource(), original.GetParticleSource());
    EXPECT_EQ(copy.GetParticleSource()->GetParticleName(), "Fe55");
    EXPECT_DOUBLE_EQ(copy.GetParticleSource()->GetEnergy(), 5.9);
    EXPECT_DOUBLE_EQ(copy.GetParticleSource()->GetEnergyDistributionRangeMin(), 5.8);

    TRestGeant4Metadata assigned;
    assigned = original;
    ASSERT_EQ(assigned.GetNumberOfSources(), 1);
    EXPECT_NE(assigned.GetParticleSource(), original.GetParticleSource());
    EXPECT_EQ(assigned.GetParticleSource()->GetParticleName(), "Fe55");

    original.GetParticleSource()->SetEnergy(6.5);
    EXPECT_DOUBLE_EQ(copy.GetParticleSource()->GetEnergy(), 5.9);
    EXPECT_DOUBLE_EQ(assigned.GetParticleSource()->GetEnergy(), 5.9);
}

TEST(TRestGeant4PhysicsInfo, MergeKeepsLookupEntries) {
    TRestGeant4PhysicsInfo first;
    first.InsertProcessName(1, "Transportation", "transportation");
    first.InsertParticleName(22, "gamma");

    TRestGeant4PhysicsInfo second;
    second.InsertProcessName(2, "phot", "electromagnetic");
    second.InsertParticleName(11, "e-");

    string conflict;
    EXPECT_TRUE(first.Merge(second, &conflict));
    EXPECT_TRUE(conflict.empty());
    EXPECT_EQ(first.GetProcessName(1), "Transportation");
    EXPECT_EQ(first.GetProcessName(2), "phot");
    EXPECT_EQ(first.GetParticleName(22), "gamma");
    EXPECT_EQ(first.GetParticleName(11), "e-");
}

TEST(TRestGeant4PhysicsInfo, MergeRejectsConflictingLookupEntries) {
    TRestGeant4PhysicsInfo first;
    first.InsertProcessName(1, "Transportation", "transportation");

    TRestGeant4PhysicsInfo second;
    second.InsertProcessName(1, "phot", "electromagnetic");

    string conflict;
    EXPECT_FALSE(first.Merge(second, &conflict));
    EXPECT_FALSE(conflict.empty());
    EXPECT_EQ(first.GetProcessName(1), "Transportation");
}

TEST(TRestGeant4Metadata, FromRml) {
    cout << "Path: " << geant4MetadataRml << endl;

    TRestGeant4Metadata restGeant4Metadata(geant4MetadataRml.c_str());

    GTEST_SKIP_("TODO: fix this");

    restGeant4Metadata.PrintMetadata();

    EXPECT_TRUE(restGeant4Metadata.GetSensitiveVolume() == "sensitiveVolume");
    EXPECT_TRUE(restGeant4Metadata.GetSeed() == 17021981);

    // primary generator
    EXPECT_TRUE(restGeant4Metadata.GetNumberOfSources() == 1);
    const auto particleSource = restGeant4Metadata.GetParticleSource(0);
    EXPECT_TRUE(particleSource->GetParticleName() == "geantino");
    EXPECT_TRUE(particleSource->GetEnergyDistributionType() == "mono");
}
