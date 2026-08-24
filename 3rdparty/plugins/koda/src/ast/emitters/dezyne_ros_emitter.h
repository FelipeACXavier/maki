#pragma once

#include <deque>
#include <filesystem>
#include <functional>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "ast/koda_ir.h"
#include "ast/symbol_registry.h"
#include "dezyne_model.h"
#include "result.h"

namespace koda::dezyne
{

struct RosEmitterOptions
{
  std::filesystem::path outputDir;
  bool singleLaunch = true;
  bool simulation = false;
  double startWait = 5.0;

  std::string callbackGroup = "callback_group_";
  std::string subscriptionOptions = "subscription_options_";
  std::string taskThread = "task_thread_";
  std::string abortLock = "abort_mtx_";
  std::string abortFlag = "aborted_";
  std::string tfBuffer = "tf_buffer_";

  std::string packageName = "koda_ros";
  std::string packageDescription = "Generated KODA ROS package";
  std::string supervisorClass = "Supervisor";
  std::string launchFile = "koda.launch.py";

  std::function<std::string(const std::string&)> abort = [this](const std::string& indent) {
    std::string output;
    output += indent + "{\n";
    output += indent + "  std::unique_lock<std::mutex> lock(" + abortLock + ");\n";
    output += indent + "  " + abortFlag + " = true;\n";
    output += indent + "}\n";
    return output;
  };
};

class RosEmitter
{
public:
  VoidResult write(const ir::Program& program, const Model& model, const SymbolRegistry& symbols, const RosEmitterOptions& options);

  const std::vector<std::string>& generatedFiles() const
  {
    return mGeneratedFiles;
  }

private:
  struct Capability
  {
    const ir::Component* component = nullptr;

    std::string sourceName;
    std::string name;
    std::string cppName;
    std::string className;

    // Assumed new IR metadata:
    //   metadata["route"], metadata["message"]
    std::string route;
    std::string message;

    const ir::Event* trigger = nullptr;
    const ir::Event* onReturn = nullptr;
    const ir::Event* onError = nullptr;
    const ir::Event* onAbort = nullptr;

    std::vector<const CallSite*> calls;

    // Capability-specific generation payload. These fields intentionally mirror
    // the previous ROS emitter so the existing Drive/Aruco/Approach/Grip
    // generators can be moved over with minimal changes.
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

  struct PortBinding
  {
    std::string port;
    const ir::Event* event = nullptr;
    const CallSite* call = nullptr;
  };

  RosEmitterOptions mOptions;
  const ir::Program* mProgram = nullptr;
  const Model* mModel = nullptr;
  const SymbolRegistry* mSymbols = nullptr;

  std::deque<Capability> mCapabilities;
  std::vector<Task> mTasks;

  std::set<std::string> mDependencies;
  std::set<std::string> mPackageDeps;
  std::set<std::string> mCmakeDeps;
  std::vector<std::string> mGeneratedFiles;

  VoidResult collect();
  VoidResult collectCapability(const ir::Component& component);
  void collectTask(const ir::Component& component);

  const ir::Event* findEvent(const ir::Component& component, SymbolId symbol) const;
  std::vector<PortBinding> actionPorts(const Capability& capability) const;
  const CallSite* callForPort(const Capability& capability, const std::string& port) const;

  void emitSupportFiles();
  void emitSupervisor();
  void emitGlueClasses();

  std::string emitSupervisorHeader() const;
  std::string emitSupervisorSource() const;
  std::string emitGlueHeader(const Capability& capability) const;
  std::string emitGlueSource(const Capability& capability) const;
  std::string emitBlackboardHeader() const;
  std::string emitAlarmHeader() const;
  std::string emitAlarmSource() const;

  std::string emitCMakeLists() const;
  std::string emitPackageXml() const;
  std::string emitMainCpp() const;
  std::string emitLaunchPy() const;
  std::string emitExtraLaunchPy() const;
  std::string emitParamsYaml() const;

  VoidResult writeFile(const std::filesystem::path& path, const std::string& text);

  std::string emitCallArguments(const Capability& capability, const PortBinding& port) const;
  std::string emitExpression(const ir::PExpression& expression) const;
  std::string cppType(const types::TypeReference& type) const;

  std::string argDecls(const std::vector<ir::Argument>& args) const;
  static std::string argNames(const std::vector<ir::Argument>& args);
  std::string callbackType(const ir::Event& event) const;

  static std::string cppName(std::string value);
  static std::string lower(std::string value);
  static std::string identifier(std::string value);

  // Temporary in-tree capability emitters. These are intentionally kept
  // capability-specific; they are the future plugin boundary.
  void collectDrive(Capability& capability);
  void collectObjectDetection(Capability& capability);
  void collectApproach(Capability& capability);
  void collectGrip(Capability& capability);
};

}  // namespace koda::dezyne