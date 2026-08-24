#pragma once

#include <QString>
#include <string>
#include <vector>

class MakiErrorListener
{
public:
  struct Error
  {
    QString nodeId;
    QString flowId;
    std::string message;
  };

  std::vector<Error> mErrors;

  void addError(const QString& nodeId, const QString& flowId, const std::string& msg)
  {
    mErrors.push_back({.nodeId = nodeId, .flowId = flowId, .message = msg});
  }

  void addError(const QString& nodeId, const std::string& msg)
  {
    mErrors.push_back({.nodeId = nodeId, .flowId = QString(), .message = msg});
  }

  bool hasErrors() const
  {
    return !mErrors.empty();
  }
};