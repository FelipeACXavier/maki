#pragma once

#include "result.h"
#include "string_helpers.h"

bool IsCommandExecutable(const std::string& command);
bool IsFileExecutable(const std::string& file);
bool DoesFileExist(const std::string& file);
bool DeleteFile(const std::string& file);
bool IsOfType(const std::string& file, const std::string& type);

std::string GetExeDir();
std::string GetFilename(const std::string& path);
std::string RemoveFilename(const std::string& path);

VoidResult CreateDirectory(const std::string& path);
std::vector<std::string> GetFilesInDirectory(const std::string& path);

Result<std::string> GetFileContents(const char* fpath);
Result<std::string> GetFileContents(const std::string& fpath);
VoidResult SetFileContents(const std::string& fpath, const std::string& value);
