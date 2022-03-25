
#include <TRestGeant4Metadata.h>
#include <gtest/gtest.h>

#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

#define FILES_PATH fs::path(__FILE__).parent_path().parent_path() / "files"
#define GEANT4_METADATA_RML FILES_PATH / "TRestGeant4Example.rml"

TEST(TRestGeant4Metadata, TestFiles) {
    cout << "FrameworkCore test files path: " << FILES_PATH << endl;

    // Check dir exists and is a directory
    EXPECT_TRUE(fs::is_directory(FILES_PATH));
    // Check it's not empty
    EXPECT_TRUE(!fs::is_empty(FILES_PATH));

    // All used files in this tests
    EXPECT_TRUE(fs::exists(GEANT4_METADATA_RML));
}

TEST(TRestGeant4Metadata, Default) {
    TRestGeant4Metadata restGeant4Metadata;

    restGeant4Metadata.PrintMetadata();

    EXPECT_TRUE(restGeant4Metadata.GetSeed() == 0);
    EXPECT_TRUE(restGeant4Metadata.GetSensitiveVolume() == "gas");
}

TEST(TRestGeant4Metadata, FromRml) {
    const auto metadataConfigRml = GEANT4_METADATA_RML;

    TRestGeant4Metadata restGeant4Metadata((char*)metadataConfigRml.c_str());

    restGeant4Metadata.PrintMetadata();

    GTEST_SKIP_("If it doesn't find the gdml, test will always return OK for some reason. TODO: fix this");

    EXPECT_TRUE(restGeant4Metadata.GetSensitiveVolume() == "sensitiveVolume");
    EXPECT_TRUE(restGeant4Metadata.GetSeed() == 17021981);

    // primary generator
    EXPECT_TRUE(restGeant4Metadata.GetNumberOfSources() == 1);
    const auto particleSource = restGeant4Metadata.GetParticleSource(0);
    EXPECT_TRUE(particleSource->GetParticleName() == "geantino");
    EXPECT_TRUE(particleSource->GetEnergyDistType() == "mono");
}
