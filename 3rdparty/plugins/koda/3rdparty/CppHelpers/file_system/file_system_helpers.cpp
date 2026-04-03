#include "file_system_helpers.h"

#include <libgen.h>
#include <sys/stat.h>
#include <unistd.h>  //using access, X_OK, F_OK
#include <climits>
#include <cstring>
#include <cstdlib>
#include <fstream>

#include "sync_process.h"

bool IsCommandExecutable(const std::string& command)
{
  std::string filePath(command.substr(0, command.find(' ')));
  return IsFileExecutable(filePath.c_str());
}

bool IsFileExecutable(const std::string& file)
{
  return !access(file.c_str(), X_OK);
}

bool DoesFileExist(const std::string& file)
{
  return (access(file.c_str(), F_OK) != -1);
}

bool DeleteFile(const std::string& file)
{
  return (!DoesFileExist(file)) || (std::remove(file.c_str()) == 0);
}

bool IsOfType(const std::string& file, const std::string& type)
{
  size_t pos = file.find_last_of('.');
  if (pos == file.npos)
    return false;

  return file.substr(pos + 1, file.npos) == type;
}

std::string RemoveFilename(const std::string& path)
{
  size_t pos = path.find_last_of('/');
  if (pos == path.npos)
  {
    // No dir part found, assume everything is filename
    return std::string("./");
  }
  else
  {
    return path.substr(0, pos + 1);
  }
}

std::string GetFilename(const std::string& path)
{
  size_t pos = path.find_last_of('/');
  auto fileWithExtension = pos == path.npos ? path : path.substr(pos + 1, path.npos);

  pos = fileWithExtension.find_last_of('.');
  return pos == path.npos ? fileWithExtension : fileWithExtension.substr(0, pos);
}

std::string GetExeDir()
{
  char path[PATH_MAX];
  const char* dir;

  memset(path, 0, PATH_MAX);

  ssize_t count = readlink("/proc/self/exe", path, PATH_MAX);
  if (count < 0)
  {
    return std::string();
  }
  else if (count == PATH_MAX)
  {
    path[PATH_MAX - 1] = '\0';
  }
  else
    path[count] = '\0';

  dir = dirname(path);

  return std::string(dir) + std::string("/");
}

VoidResult CreateDirectory(const std::string& path)
{
  int r = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  if (r == 0 || errno == EEXIST)
    return VoidResult();
  else
    return VoidResult::Failed("Creating directory failed: " +std::string(strerror(errno)));
}

std::vector<std::string> GetFilesInDirectory(const std::string& path)
{
  std::string output;
  SyncProcess::Execute("ls -p " + path, output);
  return Split(output, '\n');
}

Result<std::string> GetFileContents(const char* fpath)
{
  std::string contents;
  try
  {
    std::ifstream finstream(fpath);
    if (finstream.good())
      contents = std::string((std::istreambuf_iterator<char>(finstream)), std::istreambuf_iterator<char>());
    else
      return Result<std::string>::Failed("File '" +std::string(fpath) + "' does not exist");
  }
  catch (const std::exception& e)
  {
    return Result<std::string>::Failed("Can't read the file '" +std::string(fpath) + ": " + e.what());
  }
  return Result<std::string>(contents);
}

Result<std::string> GetFileContents(const std::string& fpath)
{
  return GetFileContents(fpath.c_str());
}

VoidResult SetFileContents(const std::string& fpath, const std::string& value)
{
  try
  {
    std::ofstream stream(fpath.c_str(), std::ios_base::out);
    if (stream.good())
      stream << value;
    else
      return VoidResult::Failed("File '" +std::string(fpath) + "' does not exist");
  }
  catch (const std::exception& e)
  {
    return VoidResult::Failed("Can't write the file " +std::string(fpath) + ": " + e.what());
  }

  return VoidResult();
}
