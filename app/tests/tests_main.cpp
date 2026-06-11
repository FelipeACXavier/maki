#include <gtest/gtest.h>

#include <QApplication>

#include "logging.h"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);
  for (int i = 1; i < argc; ++i)
  {
    QString arg = argv[i];
    if (arg == "--no-logs")
    {
      logging::gSilentLog = true;
    }
  }

  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}