
#include <TRestGeant4Metadata.h>
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

TEST(TRestGeant4Metadata, FromRml) {
    GTEST_SKIP_("Problem with paths...");

    TRestGeant4Metadata restGeant4Metadata(geant4MetadataRml.c_str());

    restGeant4Metadata.PrintMetadata();

    EXPECT_TRUE(restGeant4Metadata.GetSensitiveVolume() == "sensitiveVolume");
    EXPECT_TRUE(restGeant4Metadata.GetSeed() == 17021981);

    // primary generator
    EXPECT_TRUE(restGeant4Metadata.GetNumberOfSources() == 1);
    const auto particleSource = restGeant4Metadata.GetParticleSource(0);
    EXPECT_TRUE(particleSource->GetParticleName() == "geantino");
    EXPECT_TRUE(particleSource->GetEnergyDistributionType() == "mono");
}
