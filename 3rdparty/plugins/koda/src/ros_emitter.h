// ros_emitter.h
#pragma once

#include <filesystem>
#include <format>
#include <functional>
#include <map>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <vector>

#include "ast/ast.h"
#include "ast/koda_compiler.h"

namespace koda
{

struct RosEmitterOptions
{
  // These are to be set by the caller
  std::filesystem::path outputDir;
  bool singleLaunch = true;
  bool simulation = false;
  double startWait = 5.0;

  // These can be used by the capability plugins
  std::string callbackGroup = "callback_group_";
  std::string subscriptionOptions = "subscription_options_";
  std::string taskThread = "task_thread_";
  std::string abortLock = "abort_mtx_";
  std::string abortFlag = "aborted_";
  std::string tfBuffer = "tf_buffer_";
  std::function<std::string(const std::string& format)> abort = [this](const std::string& format) {
    std::string output = "";
    output += format + "{\n";
    output += format + std::format("  std::unique_lock<std::mutex> lock({});\n", abortLock);
    output += format + std::format("  {} = true;\n", abortFlag);
    output += format + "}\n";
    return output;
  };

  std::string packageName = "koda_ros";
  std::string packageDescription = "Generated KODA ROS package";
  std::string supervisorClass = "Supervisor";
  std::string launchFile = "koda.launch.py";
};

struct RosEmitResult
{
  bool ok = true;
  std::vector<std::string> errors;

  static RosEmitResult success()
  {
    return {};
  }

  static RosEmitResult failure(std::string error)
  {
    RosEmitResult r;
    r.ok = false;
    r.errors.push_back(std::move(error));
    return r;
  }
};

class RosEmitter
{
public:
  RosEmitResult emitRos(const System& ast, const Compiler::Environment& env, const RosEmitterOptions& options);

private:
  struct Event
  {
    std::string name;
    std::string returnType = "void";
    std::vector<Argument> args;
  };

  struct Capability
  {
    std::string name;
    std::string cppName;
    std::string className;

    std::optional<Event> trigger;
    std::optional<Event> onReturn;
    std::optional<Event> onError;
    std::optional<Event> onAbort;

    std::vector<std::string> supervisorIncludes;
    std::vector<std::string> supervisorMembers;
    std::vector<std::string> supervisorCtor;
    std::vector<std::string> supervisorStart;
    std::vector<std::string> supervisorMethods;
    std::vector<std::string> parameters;
    std::vector<std::string> configParams;

    std::set<std::string> launchIncludes;
    std::vector<std::string> launchDescriptions;
    std::vector<std::string> launchNodes;
  };

  struct Task
  {
    std::string name;
    std::string cppName;
    std::string className;
  };

  RosEmitterOptions mOptions;
  std::deque<Capability> mCapabilities;
  std::vector<Task> mTasks;
  std::set<std::string> mDependencies;
  Compiler::Environment mEnv;
  std::set<std::string> mPackageDeps;
  std::set<std::string> mCmakeDeps;

  void collect(const System& ast);
  void collectComponent(const Component& component);
  void collectActionDef(Capability& cap, const ActionDef& action);
  void collectRosDef(Capability& cap, const RosDef& ros);

  void emitSupportFiles();
  void emitSupervisor();
  void emitGlueClasses();

  void writeFile(const std::filesystem::path& path, const std::string& text);

  std::string emitSupervisorHeader() const;
  std::string emitSupervisorSource() const;
  std::string emitGlueHeader(const Capability& cap) const;
  std::string emitGlueSource(const Capability& cap) const;
  std::string emitCMakeLists() const;
  std::string emitPackageXml() const;
  std::string emitMainCpp() const;
  std::string emitLaunchPy() const;
  std::string emitExtraLaunchPy() const;
  std::string emitParamsYaml() const;

  static std::string cppName(std::string s);
  static std::string lower(std::string s);
  static std::string argDecls(const std::vector<Argument>& args);
  static std::string argNames(const std::vector<Argument>& args);
  static std::string argNames(const std::vector<std::string>& args);
  static std::string callbackType(const Event& e);
  static std::string eventName(RosDef::Kind kind);
  static std::string returnTypeOrVoid(const std::string& type);

  void collectDrive(Capability& cap);
  void collectObjectDetection(Capability& cap);
  void collectGrip(Capability& cap);
  void collectApproach(Capability& cap);
};

}  // namespace koda