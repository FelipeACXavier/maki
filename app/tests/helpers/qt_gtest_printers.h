#pragma once

#include <gtest/gtest.h>

#include <QString>
#include <ostream>

// GoogleTest finds this function through argument-dependent lookup.
inline void PrintTo(const QString& value, std::ostream* os)
{
  *os << ::testing::PrintToString(value.toStdString());
}