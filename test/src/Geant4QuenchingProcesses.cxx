
#include <TRestGeant4QuenchingProcess.h>
#include <gtest/gtest.h>

#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

const auto filesPath = fs::path(__FILE__).parent_path().parent_path() / "files";
const auto processRmlFile = filesPath / "TRestGeant4QuenchingProcessExample.rml";
const auto simulationFile = "/tmp/tmp.CXV7dwGPdN/source/packages/restG4/examples/13.IAXO/Neutrons.root";

TEST(TRestGeant4QuenchingProcess, TestFiles) {
    cout << "Test files path: " << filesPath << endl;

    // Check dir exists and is a directory
    EXPECT_TRUE(fs::is_directory(filesPath));
    // Check it's not empty
    EXPECT_TRUE(!fs::is_empty(filesPath));

    // All used files in this tests
    EXPECT_TRUE(fs::exists(processRmlFile));

    // file needs to be generated by a previous test
    // EXPECT_TRUE(fs::exists(simulationFile));
}

TEST(TRestGeant4QuenchingProcess, Default) {
    TRestGeant4QuenchingProcess process;

    process.PrintMetadata();

    const auto& volumes = process.GetUserVolumeExpressions();
    // print volumes
    for (const auto& volume : volumes) {
        cout << volume << endl;
    }
    EXPECT_TRUE(volumes.empty());

    // verify default quenching factor is 1.0
    vector<string> randomVolumes = {"scintillatorVolume", "gasVolume", "shieldingVolume"};
    vector<string> particles = {"gamma", "e-", "e+"};
    for (const auto& volume : randomVolumes) {
        for (const auto& particle : particles) {
            EXPECT_TRUE(process.GetQuenchingFactorForVolumeAndParticle(volume, particle) == 1.0);
        }
    }
}

TEST(TRestGeant4QuenchingProcess, FromRml) {
    TRestGeant4QuenchingProcess process(processRmlFile.c_str());

    process.PrintMetadata();

    EXPECT_TRUE(process.GetUserVolumeExpressions().size() == 2);
}

TEST(TRestGeant4QuenchingProcess, Simulation) {
    TRestGeant4QuenchingProcess process(processRmlFile.c_str());

    process.PrintMetadata();

    EXPECT_TRUE(process.GetUserVolumeExpressions().size() == 2);


}