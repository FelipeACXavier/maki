#pragma once

namespace maki::dezyne
{
static const QString PLUGIN_NAME = "dezyne";
static const QString VERIFY_ACTION_ID = PLUGIN_NAME + ".verify";
static const QString SIMULATE_ACTION_ID = PLUGIN_NAME + ".simulate";
static const QString GENERATE_ACTION_ID = PLUGIN_NAME + ".generate";
static const QString GENERATE_CPP_SOURCE_ACTION_ID = GENERATE_ACTION_ID + "-cpp";

// Products
static const QString SOURCE_ARTIFACT_ID = PLUGIN_NAME + ".source";
static const QString VERIFICATION_ARTIFACT_ID = PLUGIN_NAME + ".verified";
static const QString CPP_SOURCE_ARTIFACT_ID = "cpp.source";
static const QString KODA_IR_ARTIFACT_ID = "koda.ir";

// =================================================================
// For simulation
static const QString LIFELINES = "lifelines";
static const QString STATES = "states";
static const QString HEADER = "header";
static const QString INSTANCE = "instance";
static const QString LABELS = "labels";
static const QString ILLEGAL = "illegal";
static const QString ROLE = "role";
static const QString TEXT = "text";
static const QString DEFER = "<defer>";
static const QString REQUIRES = "requires";
static const QString PROVIDES = "provides";
static const QString VALUE = "value";

}  // namespace maki::dezyne
