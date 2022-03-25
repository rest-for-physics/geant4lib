
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

TEST(TRestGeant4Metadata, Default) { TRestGeant4Metadata metadata; }

TEST(TRestGeant4Metadata, FromRml) {}
