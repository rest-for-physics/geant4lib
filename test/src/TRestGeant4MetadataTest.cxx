#include <gtest/gtest.h>

#include "TRestGeant4Metadata.h"

TEST(TRestGeant4Metadata, DefaultParameters) {
    auto restGeant4Metadata = TRestGeant4Metadata();
    auto sectionName = restGeant4Metadata.GetSectionName();

    EXPECT_EQ(sectionName, "TRestGeant4Metadata");
}

TEST(TRestGeant4Metadata, Generators) {
    auto restGeant4Metadata = TRestGeant4Metadata("generators/generator.rml");
}