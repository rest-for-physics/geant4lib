
#include <TRestGeant4VetoAnalysisProcess.h>
#include <gtest/gtest.h>

#include <filesystem>

namespace fs = std::filesystem;

using namespace std;

const auto filesPath = fs::path(__FILE__).parent_path().parent_path() / "files";
const auto vetoAnalysisRml = filesPath / "TRestGeant4VetoAnalysisProcessExample.rml";

TEST(TRestGeant4VetoAnalysisProcess, TestFiles) {
    cout << "Test files path: " << filesPath << endl;

    // Check dir exists and is a directory
    EXPECT_TRUE(fs::is_directory(filesPath));
    // Check it's not empty
    EXPECT_TRUE(!fs::is_empty(filesPath));

    // All used files in this tests
    EXPECT_TRUE(fs::exists(vetoAnalysisRml));
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
