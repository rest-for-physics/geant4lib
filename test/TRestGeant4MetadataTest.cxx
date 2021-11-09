#include <gtest/gtest.h>

#include "TRestGeant4Metadata.h"

TEST(TRestGeant4Metadata, BasicTest) { EXPECT_EQ(2 * 2, 4); }

TEST(TRestGeant4Metadata, DefaultParameters) {
    auto restGeant4Metadata = TRestGeant4Metadata();
    auto sectionName = restGeant4Metadata.GetSectionName();

    EXPECT_EQ(sectionName, "TRestGeant4Metadata");
}
