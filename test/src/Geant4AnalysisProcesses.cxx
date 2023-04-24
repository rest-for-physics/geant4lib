
#include <TRestGeant4VetoAnalysisProcess.h>
#include <gtest/gtest.h>

#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

const auto filesPath = fs::path(__FILE__).parent_path().parent_path() / "files";
const auto processRmlFile = filesPath / "TRestGeant4VetoAnalysisProcessExample.rml";
const auto simulationFile = filesPath / "VetoAnalysisGeant4Run.root";

TEST(TRestGeant4VetoAnalysisProcess, TestFiles) {
    GTEST_SKIP();

    cout << "Test files path: " << filesPath << endl;

    // Check dir exists and is a directory
    EXPECT_TRUE(fs::is_directory(filesPath));
    // Check it's not empty
    EXPECT_TRUE(!fs::is_empty(filesPath));

    // All used files in this tests
    EXPECT_TRUE(fs::exists(processRmlFile));
    EXPECT_TRUE(fs::exists(simulationFile));
}

TEST(TRestGeant4VetoAnalysisProcess, Default) {
    TRestGeant4VetoAnalysisProcess process;

    process.PrintMetadata();

    EXPECT_TRUE(process.GetVetoVolumesExpression().IsNull());
    EXPECT_TRUE(process.GetVetoDetectorExpression().IsNull());
    EXPECT_TRUE(process.GetVetoDetectorOffsetSize() == 0);
    EXPECT_TRUE(process.GetVetoLightAttenuation() == 0);
    EXPECT_TRUE(process.GetVetoQuenchingFactor() == 1.0);
}

TEST(TRestGeant4VetoAnalysisProcess, FromRml) {
    TRestGeant4VetoAnalysisProcess process(processRmlFile.c_str());

    process.PrintMetadata();

    EXPECT_TRUE(process.GetVetoVolumesExpression() == "^scintillatorVolume");
    EXPECT_TRUE(process.GetVetoDetectorExpression() == "^scintillatorLightGuideVolume");
    EXPECT_TRUE(process.GetVetoDetectorOffsetSize() == 0);
    EXPECT_TRUE(process.GetVetoLightAttenuation() == 0);
    EXPECT_TRUE(process.GetVetoQuenchingFactor() == 0);
}

TEST(TRestGeant4VetoAnalysisProcess, Simulation) {
    GTEST_SKIP();

    TRestGeant4VetoAnalysisProcess process(processRmlFile.c_str());

    EXPECT_TRUE(process.GetVetoVolumesExpression() == "^scintillatorVolume");
    EXPECT_TRUE(process.GetVetoDetectorExpression() == "^scintillatorLightGuideVolume");
    EXPECT_TRUE(process.GetVetoDetectorOffsetSize() == 0);
    EXPECT_TRUE(process.GetVetoLightAttenuation() == 0);
    EXPECT_TRUE(process.GetVetoQuenchingFactor() == 0);

    TRestRun run(simulationFile.c_str());
    run.GetInputFile()->ls();

    const auto metadata =
        dynamic_cast<const TRestGeant4Metadata*>(run.GetMetadataClass("TRestGeant4Metadata"));
    EXPECT_TRUE(metadata != nullptr);

    process.SetGeant4Metadata(metadata);

    process.InitProcess();

    process.PrintMetadata();
}
