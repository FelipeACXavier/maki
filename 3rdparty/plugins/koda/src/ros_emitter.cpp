// ros_emitter.cpp
#include "ros_emitter.h"

#include <algorithm>
#include <format>
#include <fstream>
#include <stdexcept>

#include "ast/ast.h"

namespace koda
{
RosEmitResult RosEmitter::emitRos(const koda::System& ast, const Compiler::Environment& env, const RosEmitterOptions& options)
{
  try
  {
    mOptions = options;
    mEnv = env;
    mCapabilities.clear();
    mDependencies.clear();

    mDependencies.insert("rclcpp");
    mDependencies.insert("rclcpp_action");

    collect(ast);

    std::filesystem::create_directories(mOptions.outputDir / "include");
    std::filesystem::create_directories(mOptions.outputDir / "src");
    std::filesystem::create_directories(mOptions.outputDir / "launch");
    std::filesystem::create_directories(mOptions.outputDir / "config");

    emitSupportFiles();
    emitSupervisor();
    emitGlueClasses();

    return RosEmitResult::success();
  } catch (const std::exception& e)
  {
    return RosEmitResult::failure(e.what());
  }
}

void RosEmitter::collect(const System& ast)
{
  for (const auto& component : ast.components)
  {
    if (component)
      collectComponent(*component);
  }
}

void RosEmitter::collectComponent(const Component& component)
{
  if (component.kind != Component::Kind::Capability)
  {
    Task task;
    task.name = component.name + "_task";
    task.cppName = cppName(task.name);
    task.className = "c" + lower(component.name);
    mTasks.push_back(task);
    return;
  }

  Capability cap;
  cap.name = lower(component.name);
  cap.cppName = cppName(component.name);
  cap.className = "c" + lower(component.name);

  for (const auto& stmt : component.statements)
  {
    if (!stmt)
      continue;

    if (auto action = std::get_if<std::shared_ptr<ActionDef>>(&stmt->node))
    {
      if (*action)
        collectActionDef(cap, **action);
    }
    else if (auto ros = std::get_if<std::shared_ptr<RosDef>>(&stmt->node))
    {
      if (*ros)
        collectRosDef(cap, **ros);
    }
  }

  // TODO: This is what plugins are for
  if (component.name == "Drive")
  {
    collectDrive(cap);
    mCapabilities.push_front(std::move(cap));
  }
  else if (component.name == "Vision")
  {
    collectObjectDetection(cap);
    mCapabilities.push_back(std::move(cap));
  }
  else if (component.name == "Grip")
  {
    collectGrip(cap);
    mCapabilities.push_back(std::move(cap));
  }
  else if (component.name == "Approach")
  {
    collectApproach(cap);
    mCapabilities.push_back(std::move(cap));
  }
  else
  {
    LOG_ERROR("Unsupported capability: %s", cap.name.c_str());
  }
}

void RosEmitter::collectActionDef(Capability& cap, const ActionDef& action)
{
  // if (!action.label1.empty())
  //   mCmakeDeps.insert(action.label1);

  for (const auto& ros : action.rosDefs)
    if (ros)
      collectRosDef(cap, *ros);

  // Very generic ROS action assumption.
  // You can replace this with hard-coded blocks for nav2, aruco, etc.
  if (action.kind == ActionDef::Kind::Action)
    cap.supervisorIncludes.push_back("#include <rclcpp_action/rclcpp_action.hpp>");
}

void RosEmitter::collectRosDef(Capability& cap, const RosDef& ros)
{
  if (!ros.def)
    return;

  Event e;
  e.name = ros.def->name.empty() ? eventName(ros.kind) : ros.def->name;
  e.returnType = returnTypeOrVoid(ros.def->typeName);
  for (const auto& arg : ros.def->args)
    if (arg)
      e.args.push_back(*arg);

  switch (ros.kind)
  {
    case RosDef::Kind::Trigger:
      cap.trigger = e;
      break;
    case RosDef::Kind::Return:
      cap.onReturn = e;
      break;
    case RosDef::Kind::Error:
      cap.onError = e;
      break;
    case RosDef::Kind::Abort:
      cap.onAbort = e;
      break;
    default:
      break;
  }
}

void RosEmitter::emitSupportFiles()
{
  writeFile(mOptions.outputDir / "CMakeLists.txt", emitCMakeLists());
  writeFile(mOptions.outputDir / "package.xml", emitPackageXml());
  writeFile(mOptions.outputDir / "src" / "main.cpp", emitMainCpp());
  writeFile(mOptions.outputDir / "launch" / mOptions.launchFile, emitLaunchPy());
  if (!mOptions.singleLaunch)
    writeFile(mOptions.outputDir / "launch" / std::string("extra-" + mOptions.launchFile), emitExtraLaunchPy());

  writeFile(mOptions.outputDir / "config" / "params.yaml", emitParamsYaml());
}

void RosEmitter::emitSupervisor()
{
  writeFile(mOptions.outputDir / "include" / "supervisor.hh", emitSupervisorHeader());
  writeFile(mOptions.outputDir / "src" / "supervisor.cc", emitSupervisorSource());
}

void RosEmitter::emitGlueClasses()
{
  for (const auto& cap : mCapabilities)
  {
    writeFile(mOptions.outputDir / "include" / (cap.className + ".hh"), emitGlueHeader(cap));
    writeFile(mOptions.outputDir / "src" / (cap.className + ".cc"), emitGlueSource(cap));
  }
}

std::string RosEmitter::emitSupervisorHeader() const
{
  std::ostringstream ss;

  ss << "#pragma once\n\n";
  ss << "#include <functional>\n";
  ss << "#include <memory>\n";
  ss << "#include <rclcpp/rclcpp.hpp>\n\n";

  ss << "#include <tf2_ros/buffer.h>\n";
  ss << "#include <tf2_ros/transform_listener.h>\n";

  ss << "// Required for dezyne behaviour tree\n";
  ss << "#include \"types.hh\"\n\n";

  for (const auto& cap : mCapabilities)
    for (const auto& inc : cap.supervisorIncludes)
      ss << inc << "\n";

  ss << "\nclass " << mOptions.supervisorClass << " : public rclcpp::Node\n";
  ss << "{\n";
  ss << "public:\n";
  ss << "  " << mOptions.supervisorClass << "();\n";
  ss << "  void start();\n\n";

  for (const auto& cap : mCapabilities)
  {
    if (cap.trigger)
      ss << "  Result " << cap.name << "Trigger(" << argDecls(cap.trigger->args) << ");\n";

    if (cap.onAbort)
      ss << "  Result " << cap.name << "Abort(" << argDecls(cap.onAbort->args) << ");\n";
  }

  ss << "\n";
  ss << "  // Dezyne callbacks, assigned by generated glue classes.\n";
  ss << "std::function<void()> started;\n";
  for (const auto& cap : mCapabilities)
  {
    if (cap.onReturn)
      ss << "  " << callbackType(*cap.onReturn) << " " << cap.name << "_" << cap.onReturn->name << ";\n";

    if (cap.onError)
      ss << "  " << callbackType(*cap.onError) << " " << cap.name << "_" << cap.onError->name << ";\n";
  }

  ss << "\nprivate:\n";
  ss << "  // Generic ========================================================\n";
  ss << std::format("  std::mutex {};\n", mOptions.abortLock);
  ss << std::format("  bool {}{{false}};\n", mOptions.abortFlag);
  ss << std::format("  std::thread {};\n", mOptions.taskThread);
  ss << "  rclcpp::TimerBase::SharedPtr timer_;\n";
  ss << std::format("  tf2_ros::Buffer {};\n", mOptions.tfBuffer);
  ss << "  tf2_ros::TransformListener tf_listener_;\n";
  ss << std::format("  rclcpp::CallbackGroup::SharedPtr {};\n\n", mOptions.callbackGroup);

  for (const auto& cap : mCapabilities)
  {
    for (const auto& member : cap.supervisorMembers)
      ss << "  " << member << "\n";
    ss << "\n";
  }

  ss
      << "};\n";
  return ss.str();
}

std::string RosEmitter::emitSupervisorSource() const
{
  std::ostringstream ss;

  ss << "#include \"supervisor.hh\"\n\n";
  ss << "using namespace std::chrono_literals;\n\n";
  ss << mOptions.supervisorClass << "::" << mOptions.supervisorClass << "()\n";
  ss << "  : rclcpp::Node(\"supervisor\")\n";
  ss << std::format("  , {}(this->get_clock())\n", mOptions.tfBuffer);
  ss << std::format("  , tf_listener_({})\n", mOptions.tfBuffer);
  ss << "{\n";
  ss << std::format("  {} = create_callback_group(rclcpp::CallbackGroupType::Reentrant);\n", mOptions.callbackGroup);
  ss << std::format("  rclcpp::SubscriptionOptions {};\n", mOptions.subscriptionOptions);
  ss << std::format("  {}.callback_group = {};\n", mOptions.subscriptionOptions, mOptions.callbackGroup);

  for (const auto& cap : mCapabilities)
  {
    for (const auto& p : cap.parameters)
      ss << "  " << p << "\n";

    for (const auto& c : cap.supervisorCtor)
      ss << "  " << c << "\n";
  }

  ss << "// Kick off orchestration once actions are available\n";
  ss << std::format("timer_ = create_wall_timer(2s, std::bind(&{}::start, this));\n", mOptions.supervisorClass);
  ss << "}\n\n";

  ss << "void " << mOptions.supervisorClass << "::start()\n";
  ss << "{\n";
  ss << "  timer_->cancel();\n\n";
  ss << "  {\n";
  ss << std::format("    std::unique_lock<std::mutex> lock({});\n", mOptions.abortLock);
  ss << std::format("    {} = false;\n", mOptions.abortFlag);
  ss << "  }\n\n";
  for (const auto& cap : mCapabilities)
  {
    for (const auto& s : cap.supervisorStart)
      ss << "  " << s << "\n";
    ss << "\n";
  }
  ss << "  if (started)\n";
  ss << "    started();\n";
  ss << "}\n\n";

  for (const auto& cap : mCapabilities)
  {
    for (const auto& s : cap.supervisorMethods)
      ss << s << "\n";
    ss << "\n";
  }

  return ss.str();
}

std::string RosEmitter::emitGlueHeader(const Capability& cap) const
{
  std::ostringstream ss;

  ss << "#pragma once\n\n";
  ss << "#include \"a_" << cap.name << ".hh\"\n\n";
  ss << "#include \"types.hh\"\n\n";
  ss << "class " << cap.className << " : public skel::" << cap.className << "\n";
  ss << "{\n";
  ss << "public:\n";
  ss << "  " << cap.className << "(dzn::locator const& locator);\n";
  ss << "  ~" << cap.className << "() override = default;\n";
  ss << "  void start();\n\n";

  if (cap.trigger)
  {
    if (mEnv.capabilityCalls.contains(cap.name))
    {
      auto calls = mEnv.capabilityCalls.at(cap.name);
      for (const auto& call : calls)
      {
        ss << "  Result " << cap.trigger->name << "_" << call.count << "_" << "trigger() override;\n";
        ss << "  Result " << cap.trigger->name << "_" << call.count << "_" << "abort() override;\n";
        ss << "  Result " << cap.trigger->name << "_" << call.count << "_" << "reset() override;\n";
      }
    }
  }

  if (cap.onAbort)
  {
    ss << "  Result " << cap.onAbort->name << "_" << "trigger() override;\n";
    ss << "  Result " << cap.onAbort->name << "_" << "abort() override;\n";
    ss << "  Result " << cap.onAbort->name << "_" << "reset() override;\n";
  }

  ss << "private:\n";
  ss << "  int mState = 0;\n";
  ss << "};\n";
  return ss.str();
}

std::string RosEmitter::emitGlueSource(const Capability& cap) const
{
  std::ostringstream ss;

  ss << "#include \"" << cap.className << ".hh\"\n";
  ss << "#include \"supervisor.hh\"\n";
  ss << "#include <dzn/pump.hh>\n";
  ss << "#include <iostream>\n\n";

  ss << cap.className << "::" << cap.className << "(dzn::locator const& locator)\n";
  ss << "  : skel::" << cap.className << "(locator)\n";
  ss << "{\n";
  ss << "}\n\n";

  // =================================================================================
  // Start
  ss << "void " << cap.className << "::start()\n";
  ss << "{\n";
  ss << "  auto supervisor = dzn_locator.get<std::shared_ptr<Supervisor>>();\n";
  ss << "  if (!supervisor)\n";
  ss << "  {\n";
  ss << "    std::cout << \"Failed to retrieve Supervisor\" << std::endl;\n";
  ss << "    return;\n";
  ss << "  }\n\n";

  // Connect callbacks
  if (cap.onReturn)
  {
    ss << "  supervisor->" << cap.name << "_" << cap.onReturn->name
       << " = [this](" << argDecls(cap.onReturn->args) << ") {\n";
    ss << std::format("    std::cout << \"{}_{}\" << \": Success\";\n", cap.name, cap.onReturn->name);
    ss << "    auto& pump = dzn_locator.get<dzn::pump>();\n";
    ss << "    pump([this] {\n";
    if (mEnv.capabilityCalls.contains(cap.name))
    {
      auto calls = mEnv.capabilityCalls.at(cap.name);
      for (size_t i = 0; i < calls.size(); ++i)
      {
        if (i == 0)
          ss << std::format("      if (mState == {})\n", calls[i].count);
        else
          ss << std::format("      else if (mState == {})\n", calls[i].count);

        ss << std::format("        {}_{}_success();\n", cap.trigger->name, calls[i].count);
      }
    }
    else
    {
      ss << "      " << cap.trigger->name << "_success();\n";
    }
    ss << "    });\n";
    ss << "  };\n\n";
  }

  if (cap.onError)
  {
    ss << "  supervisor->" << cap.name << "_" << cap.onError->name
       << " = [this](" << argDecls(cap.onError->args) << ") {\n";
    ss << std::format("    std::cout << \"{}_{}\" << \": Failed\";\n", cap.name, cap.onError->name);
    ss << "    auto& pump = dzn_locator.get<dzn::pump>();\n";
    ss << "    pump([this] {\n";
    if (mEnv.capabilityCalls.contains(cap.name))
    {
      auto calls = mEnv.capabilityCalls.at(cap.name);
      for (size_t i = 0; i < calls.size(); ++i)
      {
        if (i == 0)
          ss << std::format("      if (mState == {})\n", calls[i].count);
        else
          ss << std::format("      else if (mState == {})\n", calls[i].count);

        ss << std::format("        {}_{}_failure();\n", cap.trigger->name, calls[i].count);
      }
    }
    else
    {
      ss << "      " << cap.trigger->name << "_failure();\n";
    }
    ss << "    });\n";
    ss << "  };\n\n";
  }

  ss << "}\n\n";

  if (cap.trigger)
  {
    if (mEnv.capabilityCalls.contains(cap.name))
    {
      auto calls = mEnv.capabilityCalls.at(cap.name);
      for (const auto& call : calls)
      {
        ss << "Result " << cap.className << "::" << cap.trigger->name << "_" << call.count << "_trigger()\n";
        ss << "{\n";
        ss << "  auto supervisor = dzn_locator.get<std::shared_ptr<Supervisor>>();\n";
        ss << "  if (!supervisor)\n";
        ss << "  {\n";
        ss << "    std::cout << \"Failed to retrieve Supervisor\" << std::endl;\n";
        ss << "    return Result::Failure;\n";
        ss << "  }\n";
        ss << "";
        ss << std::format("  mState = {};\n", call.count);
        ss << "  return supervisor->" << cap.name << "Trigger(" << argNames(call.args) << ");\n";
        ss << "}\n\n";

        ss << "Result " << cap.className << "::" << cap.trigger->name << "_" << call.count << "_abort()\n";
        ss << "{ return Result::Success; }\n\n";

        ss << "Result " << cap.className << "::" << cap.trigger->name << "_" << call.count << "_reset()\n";
        ss << "{ return Result::Success; }\n\n";
      }
    }
  }

  if (cap.onAbort)
  {
    ss << "Result " << cap.className << "::" << cap.onAbort->name << "_trigger()\n";
    ss << "{\n";
    ss << "  auto supervisor = dzn_locator.get<std::shared_ptr<Supervisor>>();\n";
    ss << "  if (!supervisor)\n";
    ss << "  {\n";
    ss << "    std::cout << \"Failed to retrieve Supervisor\" << std::endl;\n";
    ss << "    return Result::Failure;\n";
    ss << "  }\n";
    ss << "";
    ss << "  return supervisor->" << cap.name << "Abort(" << argNames(cap.onAbort->args) << ");\n";
    ss << "}\n\n";

    ss << "Result " << cap.className << "::" << cap.onAbort->name << "_abort()\n";
    ss << "{ return Result::Success; }\n\n";

    ss << "Result " << cap.className << "::" << cap.onAbort->name << "_reset()\n";
    ss << "{ return Result::Success; }\n\n";
  }

  return ss.str();
}

std::string RosEmitter::emitCMakeLists() const
{
  std::ostringstream ss;

  ss << "cmake_minimum_required(VERSION 3.8)\n";
  ss << "project(" << lower(mOptions.packageName) << ")\n\n";
  ss << "set(CMAKE_CXX_STANDARD 17)\n";
  ss << "set(CMAKE_CXX_STANDARD_REQUIRED ON)\n";
  ss << "set(CMAKE_EXPORT_COMPILE_COMMANDS ON)\n\n";

  ss << "if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES \"Clang\")\n";
  ss << "  add_compile_options(-Wall -Wextra -Wpedantic)\n";
  ss << "endif()\n\n";

  ss << "# find dependencies\n";
  ss << "find_package(ament_cmake REQUIRED)\n";

  ss << "# Added\n";
  for (const auto& dep : mDependencies)
    ss << "find_package(" << dep << " REQUIRED)\n";

  for (const auto& dep : mCmakeDeps)
    ss << "find_package(" << dep << " REQUIRED)\n";

  ss << "\nfile(GLOB SOURCES \"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc\")\n\n";
  ss << "add_executable(${PROJECT_NAME} src/main.cpp ${SOURCES})\n";
  ss << "target_include_directories(${PROJECT_NAME} PRIVATE include include/dzn)\n";
  ss << "ament_target_dependencies(${PROJECT_NAME}";
  for (const auto& dep : mDependencies)
    ss << "  " << dep << "\n";
  for (const auto& dep : mCmakeDeps)
    ss << "  " << dep << "\n";
  ss << ")\n\n";

  ss << "install(TARGETS ${PROJECT_NAME} DESTINATION lib/${PROJECT_NAME})\n";
  ss << "install(DIRECTORY config launch DESTINATION share/${PROJECT_NAME})\n";
  ss << "install(DIRECTORY include/ DESTINATION include/)\n\n";

  ss << "ament_export_include_directories(include)\n";
  ss << "ament_package()\n";

  return ss.str();
}

std::string RosEmitter::emitPackageXml() const
{
  std::ostringstream ss;

  ss << "<?xml version=\"1.0\"?>\n";
  ss << "<package format=\"3\">\n";
  ss << "  <name>" << lower(mOptions.packageName) << "</name>\n";
  ss << "  <version>0.0.0</version>\n";
  ss << "  <description>" << mOptions.packageDescription << "</description>\n";
  ss << "  <maintainer email=\"todo@todo.com\">TODO</maintainer>\n";
  ss << "  <license>Apache-2.0</license>\n\n";
  ss << "  <buildtool_depend>ament_cmake</buildtool_depend>\n\n";

  for (const auto& dep : mDependencies)
    ss << "  <depend>" << dep << "</depend>\n";
  ss << "\n";

  for (const auto& dep : mPackageDeps)
    ss << "  <depend>" << dep << "</depend>\n";
  ss << "\n";

  ss << "  <exec_depend>ros2launch</exec_depend>\n";
  ss << "  <export><build_type>ament_cmake</build_type></export>\n";
  ss << "</package>\n";

  return ss.str();
}

std::string RosEmitter::emitMainCpp() const
{
  std::ostringstream ss;

  ss << "#include <iostream>\n";
  ss << "#include <memory>\n";
  ss << "#include <rclcpp/rclcpp.hpp>\n\n";
  ss << "#include \"supervisor.hh\"\n";
  for (const auto& task : mTasks)
    ss << std::format("#include \"{}.hh\"\n", task.name);
  ss << "\n";

  ss << "std::ostream nullstream(nullptr);\n";
  ss << "dzn::runtime runtime;\n";
  ss << "dzn::locator locator;\n\n";

  ss << "int main(int argc, char** argv)\n";
  ss << "{\n";
  ss << "  rclcpp::init(argc, argv);\n";
  ss << "  auto node = std::make_shared<Supervisor>();\n";
  for (const auto& task : mTasks)
  {
    ss << std::format("  auto system = std::make_shared<{}>(\n", task.className);
    ss << "    locator.set(runtime).set(nullstream).set(node));\n";
  }
  ss << "\n";

  ss << "  system->api.out.success = [] {\n";
  ss << "    std::cout << \"Task succeeded\" << std::endl;\n ";
  ss << "  };\n";
  ss << "  system->api.out.failure = [] {\n";
  ss << "    std::cout << \"Task failed\" << std::endl;\n ";
  ss << "  };\n";

  for (const auto& cap : mCapabilities)
    ss << std::format("  system->{}.start();\n", cap.name);
  ss << "\n";

  ss << "  node->started = [system] {\n";
  ss << "    std::cout << \"Node started\" << std::endl;\n ";
  ss << "    system->api.in.trigger();\n";
  ss << "  };\n\n";

  ss << "  rclcpp::executors::MultiThreadedExecutor exec;\n";
  ss << "  exec.add_node(node);\n";
  ss << "  exec.spin();\n\n";
  ss << "  rclcpp::shutdown();\n";
  ss << "  return 0;\n";
  ss << "}\n";

  return ss.str();
}

std::string RosEmitter::emitLaunchPy() const
{
  std::ostringstream ss;

  // We need to wait a bit when launching with a simulator
  if (mOptions.simulation)
    ss << "from launch.actions import TimerAction\n";

  ss << "from pathlib import Path\n";
  ss << "from launch import LaunchDescription\n";
  ss << "from launch_ros.actions import Node\n";
  ss << "from ament_index_python.packages import get_package_share_directory\n\n";
  if (mOptions.singleLaunch)
  {
    for (const auto& cap : mCapabilities)
      for (const auto& node : cap.launchIncludes)
        ss << node << "\n";
  }

  ss << "def generate_launch_description():\n";
  ss << "    pkg_share = Path(get_package_share_directory('" << lower(mOptions.packageName) << "'))\n";
  ss << "    params = str(pkg_share / 'config' / 'params.yaml')\n\n";

  if (mOptions.singleLaunch)
  {
    ss << "    workspace = Path('/home/felaze/ros2_ws')\n";
    for (const auto& cap : mCapabilities)
    {
      for (const auto& node : cap.launchDescriptions)
        ss << "    " << node << "\n";
      ss << "\n";
    }

    if (mOptions.simulation)
    {
      ss << "    gazebo = IncludeLaunchDescription(\n";
      ss << "            PythonLaunchDescriptionSource(\n";
      ss << "                PathJoinSubstitution([\n";
      ss << "                    FindPackageShare('turtlebot3_manipulation_gazebo'),\n";
      ss << "                    'launch',\n";
      ss << "                    'gazebo.launch.py',\n";
      ss << "                ])\n";
      ss << "            ),\n";
      ss << "            launch_arguments={\n";
      ss << "                'world': str(workspace / 'world' / 'aruco_world.world'),\n";
      ss << "                'use_sim_time': 'true',\n";
      ss << "            }.items(), )\n\n";
    }

    ss << "    supervisor = TimerAction(\n";
    ss << std::format("        period={:.2f},\n", mOptions.startWait);
    ss << "        actions=[\n";
    ss << "          Node(\n";
    ss << "            package='" << lower(mOptions.packageName) << "',\n";
    ss << "            executable='" << lower(mOptions.packageName) << "',\n";
    ss << "            name='supervisor',\n";
    ss << "            output='screen',\n";
    ss << "            parameters=[params],\n";
    ss << "          )\n";
    ss << "        ])\n";
  }
  else
  {
    ss << "    supervisor = Node(\n";
    ss << "            package='" << lower(mOptions.packageName) << "',\n";
    ss << "            executable='" << lower(mOptions.packageName) << "',\n";
    ss << "            name='supervisor',\n";
    ss << "            output='screen',\n";
    ss << "            parameters=[params])\n\n";
  }

  ss << "    return LaunchDescription([\n";
  if (mOptions.singleLaunch)
  {
    for (const auto& cap : mCapabilities)
      for (const auto& node : cap.launchNodes)
        ss << "      " << node << ",\n";

    if (mOptions.simulation)
      ss << "      gazebo,\n";
  }
  ss << "      supervisor,\n";
  ss << "    ])\n";

  return ss.str();
}

std::string RosEmitter::emitExtraLaunchPy() const
{
  std::ostringstream ss;

  ss << "from pathlib import Path\n";
  ss << "from launch import LaunchDescription\n";
  for (const auto& cap : mCapabilities)
    for (const auto& node : cap.launchIncludes)
      ss << node << "\n";
  ss << "from ament_index_python.packages import get_package_share_directory\n\n";

  ss << "def generate_launch_description():\n";
  ss << "    workspace = Path('/home/felaze/ros2_ws')\n";

  for (const auto& cap : mCapabilities)
  {
    for (const auto& node : cap.launchDescriptions)
      ss << "    " << node << "\n";
    ss << "\n";
  }

  if (mOptions.simulation)
  {
    ss << "    gazebo = IncludeLaunchDescription(\n";
    ss << "            PythonLaunchDescriptionSource(\n";
    ss << "                PathJoinSubstitution([\n";
    ss << "                    FindPackageShare('turtlebot3_manipulation_gazebo'),\n";
    ss << "                    'launch',\n";
    ss << "                    'gazebo.launch.py',\n";
    ss << "                ])\n";
    ss << "            ),\n";
    ss << "            launch_arguments={\n";
    ss << "                'world': str(workspace / 'world' / 'aruco_world.world'),\n";
    ss << "                'use_sim_time': 'true',\n";
    ss << "            }.items(), )\n\n";
  }

  ss << "    return LaunchDescription([\n";
  for (const auto& cap : mCapabilities)
    for (const auto& node : cap.launchNodes)
      ss << "      " << node << ",\n";

  if (mOptions.simulation)
    ss << "      gazebo,\n";

  ss << "    ])\n";

  return ss.str();
}

std::string RosEmitter::emitParamsYaml() const
{
  std::ostringstream ss;

  ss << "supervisor:\n";
  ss << "  ros__parameters:\n";

  for (const auto& cap : mCapabilities)
  {
    for (const auto& p : cap.configParams)
      ss << "    " << p << "\n";
  }

  // TODO: This should be a global option
  ss << "    use_sim_time: true\n";

  return ss.str();
}

void RosEmitter::writeFile(const std::filesystem::path& path, const std::string& text)
{
  std::filesystem::create_directories(path.parent_path());

  std::ofstream out(path);
  if (!out)
    throw std::runtime_error("Could not write file: " + path.string());

  out << text;
}

std::string RosEmitter::cppName(std::string s)
{
  if (s.empty())
    return "GeneratedCapability";

  s[0] = static_cast<char>(std::toupper(s[0]));
  s.erase(std::remove_if(s.begin(), s.end(), [](char c) {
            return !(std::isalnum(static_cast<unsigned char>(c)) || c == '_');
          }),
          s.end());

  return s;
}

std::string RosEmitter::lower(std::string s)
{
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) {
    return static_cast<char>(std::tolower(c));
  });
  return s;
}

std::string RosEmitter::argDecls(const std::vector<Argument>& args)
{
  std::ostringstream ss;

  for (std::size_t i = 0; i < args.size(); ++i)
  {
    if (i > 0)
      ss << ", ";

    ss << args[i].a << " " << args[i].b;
  }

  return ss.str();
}

std::string RosEmitter::argNames(const std::vector<Argument>& args)
{
  std::ostringstream ss;

  for (std::size_t i = 0; i < args.size(); ++i)
  {
    if (i > 0)
      ss << ", ";

    ss << args[i].b;
  }

  return ss.str();
}

std::string RosEmitter::argNames(const std::vector<std::string>& args)
{
  std::ostringstream ss;

  for (std::size_t i = 0; i < args.size(); ++i)
  {
    if (i > 0)
      ss << ", ";

    ss << args[i];
  }

  return ss.str();
}

std::string RosEmitter::callbackType(const Event& e)
{
  return "std::function<" + e.returnType + "(" + argDecls(e.args) + ")>";
}

std::string RosEmitter::eventName(RosDef::Kind kind)
{
  switch (kind)
  {
    case RosDef::Kind::Trigger:
      return "trigger";
    case RosDef::Kind::Return:
      return "return";
    case RosDef::Kind::Error:
      return "error";
    case RosDef::Kind::Abort:
      return "abort";
    default:
      return "event";
  }
}

std::string RosEmitter::returnTypeOrVoid(const std::string& type)
{
  return type.empty() ? "void" : type;
}

// TODO: All of these should be moved to their own plugins
void RosEmitter::collectDrive(Capability& cap)
{
  // ======================================================================================================
  // Include dependencies
  mCmakeDeps.insert("nav2_msgs");
  mCmakeDeps.insert("control_msgs");
  mCmakeDeps.insert("trajectory_msgs");

  // ======================================================================================================
  // Package xml dependencies
  mPackageDeps.insert("nav2_msgs");
  mPackageDeps.insert("control_msgs");
  mPackageDeps.insert("trajectory_msgs");

  // ======================================================================================================
  // Includes - These are the necessary headers
  cap.supervisorIncludes.push_back("// Drive ==========================================================================");
  cap.supervisorIncludes.push_back("#include <nav2_msgs/action/navigate_to_pose.hpp>");
  cap.supervisorIncludes.push_back("#include <nav_msgs/msg/odometry.hpp>");

  // ======================================================================================================
  // Members - These are the members needed for this capability
  cap.supervisorMembers.push_back("// Drive ==========================================================================");
  cap.supervisorMembers.push_back("rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::SharedPtr nav_client_;");
  cap.supervisorMembers.push_back("rclcpp_action::ClientGoalHandle<nav2_msgs::action::NavigateToPose>::SharedPtr current_goal_;");
  cap.supervisorMembers.push_back("std::atomic<bool> got_amcl_pose_{false};");
  cap.supervisorMembers.push_back("rclcpp::Publisher<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr initialpose_pub_;\n");
  cap.supervisorMembers.push_back("rclcpp::Subscription<geometry_msgs::msg::PoseWithCovarianceStamped>::SharedPtr amcl_pose_sub_;");
  cap.supervisorMembers.push_back("void publishInitialPose();");

  // ======================================================================================================
  // Constructor - These are the actions necessary for the correct construction of this capability
  cap.supervisorCtor.push_back("// Drive ==========================================================================");
  cap.supervisorCtor.push_back("nav_client_ = ");
  cap.supervisorCtor.push_back(std::format("    rclcpp_action::create_client<nav2_msgs::action::NavigateToPose>(this, \"navigate_to_pose\", {});", mOptions.callbackGroup));
  cap.supervisorCtor.push_back("initialpose_pub_ = create_publisher<geometry_msgs::msg::PoseWithCovarianceStamped>(\"/initialpose\", 10);");
  cap.supervisorCtor.push_back("amcl_pose_sub_ = create_subscription<geometry_msgs::msg::PoseWithCovarianceStamped>(\"/amcl_pose\", 10,");
  cap.supervisorCtor.push_back("    [this](const geometry_msgs::msg::PoseWithCovarianceStamped::SharedPtr) {\n");
  cap.supervisorCtor.push_back("      got_amcl_pose_.store(true);");
  cap.supervisorCtor.push_back("      auto tmp = amcl_pose_sub_;");
  cap.supervisorCtor.push_back(std::format("      amcl_pose_sub_.reset(); }}, {});", mOptions.subscriptionOptions));

  // ======================================================================================================
  // Parameters - These are the paramaters needed for this capability
  cap.parameters.push_back("  // Drive ========================================================================== \n");
  cap.parameters.push_back("declare_parameter<bool>(\"from_start\", true);");
  cap.parameters.push_back("declare_parameter<std::string>(\"map_frame\", \"map\");");
  cap.parameters.push_back("declare_parameter<double>(\"initial_x\", 0.0);");
  cap.parameters.push_back("declare_parameter<double>(\"initial_y\", 0.0);");
  cap.parameters.push_back("declare_parameter<double>(\"initial_yaw\", 0.0);");

  cap.configParams.push_back("initial_x: -0.0252");
  cap.configParams.push_back("initial_y: -0.0276");
  cap.configParams.push_back("initial_yaw: 0.0");

  // ======================================================================================================
  // Start - Here we add any start up actions needed by this capability
  cap.supervisorStart.push_back("if (!nav_client_->wait_for_action_server(5s)) {");
  cap.supervisorStart.push_back("  RCLCPP_ERROR(get_logger(), \"Nav2 \'navigate_to_pose\' action not available.\");");
  cap.supervisorStart.push_back("  rclcpp::shutdown();");
  cap.supervisorStart.push_back("  return;");
  cap.supervisorStart.push_back("}");

  cap.supervisorStart.push_back("publishInitialPose();");

  // ======================================================================================================
  // Methods - Here we add any methods that this capability might add to the supervisor
  // First the trigger
  cap.supervisorMethods.push_back("// Drive ========================================================================== ");
  cap.supervisorMethods.push_back(std::format("Result {}::{}Trigger({})", mOptions.supervisorClass, cap.name, argDecls(cap.trigger->args)));
  cap.supervisorMethods.push_back("{");
  cap.supervisorMethods.push_back("  auto frame_id = get_parameter(\"map_frame\").as_string();");
  cap.supervisorMethods.push_back("  auto goal_msg = nav2_msgs::action::NavigateToPose::Goal();");
  cap.supervisorMethods.push_back("  goal_msg.pose.header.stamp = now();");
  cap.supervisorMethods.push_back("  goal_msg.pose.header.frame_id = frame_id;");
  cap.supervisorMethods.push_back("  goal_msg.pose.pose.position.x = x;");
  cap.supervisorMethods.push_back("  goal_msg.pose.pose.position.y = y;\n");
  cap.supervisorMethods.push_back("  tf2::Quaternion q;");
  cap.supervisorMethods.push_back("  q.setRPY(0, 0, yaw);");
  cap.supervisorMethods.push_back("  goal_msg.pose.pose.orientation = tf2::toMsg(q);\n");
  cap.supervisorMethods.push_back("  rclcpp_action::Client<nav2_msgs::action::NavigateToPose>::SendGoalOptions opts;");
  cap.supervisorMethods.push_back("  opts.goal_response_callback =");
  cap.supervisorMethods.push_back("      [this](auto gh) {");
  cap.supervisorMethods.push_back("        RCLCPP_INFO(this->get_logger(), gh ? \"Goal ACCEPTED\" : \"Goal REJECTED\");");
  cap.supervisorMethods.push_back("        current_goal_ = gh;");
  cap.supervisorMethods.push_back("      };");
  cap.supervisorMethods.push_back("  opts.result_callback =");
  cap.supervisorMethods.push_back("      [this, x, y, yaw](const auto& wr) {");
  cap.supervisorMethods.push_back("        RCLCPP_INFO(this->get_logger(), \"Result code=%d\", (int)wr.code);");
  cap.supervisorMethods.push_back("        if (wr.code != rclcpp_action::ResultCode::SUCCEEDED) {");
  cap.supervisorMethods.push_back(std::format("          if ({}_{})", cap.name, cap.onError->name));
  cap.supervisorMethods.push_back(std::format("            {}_{}({});", cap.name, cap.onError->name, argNames(cap.onError->args)));
  cap.supervisorMethods.push_back("        } else {");
  cap.supervisorMethods.push_back(std::format("          if ({}_{})", cap.name, cap.onReturn->name));
  cap.supervisorMethods.push_back(std::format("            {}_{}({});", cap.name, cap.onReturn->name, argNames(cap.onReturn->args)));
  cap.supervisorMethods.push_back("        }");
  cap.supervisorMethods.push_back("      };\n");
  cap.supervisorMethods.push_back("  nav_client_->async_send_goal(goal_msg, opts);");
  cap.supervisorMethods.push_back("  return Result::Success;");
  cap.supervisorMethods.push_back("}");

  // Then the abort
  cap.supervisorMethods.push_back(std::format("Result {}::{}Abort({})", mOptions.supervisorClass, cap.name, argDecls(cap.onAbort->args)));
  cap.supervisorMethods.push_back("{");
  cap.supervisorMethods.push_back("  if (current_goal_) {");
  cap.supervisorMethods.push_back("    RCLCPP_INFO(this->get_logger(), \"Trying to abort\");");
  cap.supervisorMethods.push_back("    nav_client_->async_cancel_goal(current_goal_);");
  cap.supervisorMethods.push_back("  }");
  cap.supervisorMethods.push_back("  return Result::Done;");
  cap.supervisorMethods.push_back("}");

  // Initial position
  cap.supervisorMethods.push_back(std::format("void {}::publishInitialPose()", mOptions.supervisorClass));
  cap.supervisorMethods.push_back("{");
  cap.supervisorMethods.push_back("  const auto map_frame = get_parameter(\"map_frame\").as_string();");
  cap.supervisorMethods.push_back("  const double x = get_parameter(\"initial_x\").as_double();");
  cap.supervisorMethods.push_back("  const double y = get_parameter(\"initial_y\").as_double();");
  cap.supervisorMethods.push_back("  const double yaw = get_parameter(\"initial_yaw\").as_double();\n");

  cap.supervisorMethods.push_back("  geometry_msgs::msg::PoseWithCovarianceStamped msg;");
  cap.supervisorMethods.push_back("  msg.header.stamp = now();");
  cap.supervisorMethods.push_back("  msg.header.frame_id = map_frame;");
  cap.supervisorMethods.push_back("  msg.pose.pose.position.x = x;");
  cap.supervisorMethods.push_back("  msg.pose.pose.position.y = y;");
  cap.supervisorMethods.push_back("  msg.pose.pose.position.z = 0.0;");
  cap.supervisorMethods.push_back("  tf2::Quaternion q;");
  cap.supervisorMethods.push_back("  q.setRPY(0, 0, yaw);");
  cap.supervisorMethods.push_back("  msg.pose.pose.orientation = tf2::toMsg(q);\n");

  cap.supervisorMethods.push_back("  // Simple covariance: a bit of uncertainty in x/y/yaw (6x6, row-major)");
  cap.supervisorMethods.push_back("  // var(x)=0.25^2, var(y)=0.25^2, var(yaw)=(10deg)^2");
  cap.supervisorMethods.push_back("  const double var_xy = 0.25 * 0.25;");
  cap.supervisorMethods.push_back("  const double var_yaw = (10.0 * M_PI / 180.0) * (10.0 * M_PI / 180.0);");
  cap.supervisorMethods.push_back("  for (double& c : msg.pose.covariance)");
  cap.supervisorMethods.push_back("    c = 0.0;\n");

  cap.supervisorMethods.push_back("  msg.pose.covariance[0] = var_xy;    // x");
  cap.supervisorMethods.push_back("  msg.pose.covariance[7] = var_xy;    // y");
  cap.supervisorMethods.push_back("  msg.pose.covariance[35] = var_yaw;  // yaw (index 35)\n");

  cap.supervisorMethods.push_back("  // Publish a few times (helps in sim/launch races)");
  cap.supervisorMethods.push_back("  RCLCPP_INFO(get_logger(), \"Publishing initial pose at(%.2f, %.2f, yaw = %.2f rad) in %s.\", x, y, yaw, map_frame.c_str());");
  cap.supervisorMethods.push_back("  for (int i = 0; i < 10; ++i)");
  cap.supervisorMethods.push_back("  {");
  cap.supervisorMethods.push_back("    msg.header.stamp = now();");
  cap.supervisorMethods.push_back("    initialpose_pub_->publish(msg);");
  cap.supervisorMethods.push_back("    rclcpp::sleep_for(std::chrono::milliseconds(100));");
  cap.supervisorMethods.push_back("  }\n");

  cap.supervisorMethods.push_back("  // Optional: wait briefly for AMCL to echo a pose once");
  cap.supervisorMethods.push_back("  auto deadline = now() + rclcpp::Duration::from_seconds(2.0);");
  cap.supervisorMethods.push_back("  while (!got_amcl_pose_.load() && now() < deadline)");
  cap.supervisorMethods.push_back("  {");
  cap.supervisorMethods.push_back("    rclcpp::sleep_for(std::chrono::milliseconds(50));");
  cap.supervisorMethods.push_back("  }\n");

  cap.supervisorMethods.push_back("  if (got_amcl_pose_.load())");
  cap.supervisorMethods.push_back("  {");
  cap.supervisorMethods.push_back("    RCLCPP_INFO(get_logger(), \"AMCL pose received after initialpose publish.\");");
  cap.supervisorMethods.push_back("    rclcpp::sleep_for(std::chrono::milliseconds(500));");
  cap.supervisorMethods.push_back("  }");
  cap.supervisorMethods.push_back("  else");
  cap.supervisorMethods.push_back("  {");
  cap.supervisorMethods.push_back("    RCLCPP_WARN(get_logger(), \"No / amcl_pose observed yet; continuing anyway.\");");
  cap.supervisorMethods.push_back("  }");
  cap.supervisorMethods.push_back("}\n");

  cap.launchNodes.push_back("navigation");

  cap.launchIncludes.insert("from launch.substitutions import PathJoinSubstitution");
  cap.launchIncludes.insert("from launch_ros.substitutions import FindPackageShare");
  cap.launchIncludes.insert("from launch.actions import IncludeLaunchDescription");
  cap.launchIncludes.insert("from launch.launch_description_sources import PythonLaunchDescriptionSource");

  cap.launchDescriptions.push_back("navigation = IncludeLaunchDescription(");
  cap.launchDescriptions.push_back("    PythonLaunchDescriptionSource(");
  cap.launchDescriptions.push_back("        PathJoinSubstitution([");
  cap.launchDescriptions.push_back("          FindPackageShare('turtlebot3_navigation2'),");
  cap.launchDescriptions.push_back("          'launch',");
  cap.launchDescriptions.push_back("          'navigation2.launch.py',");
  cap.launchDescriptions.push_back("        ])),");
  cap.launchDescriptions.push_back("    launch_arguments = {");
  cap.launchDescriptions.push_back("      'use_sim_time' : 'true',");
  cap.launchDescriptions.push_back("      'map' : str(workspace /  'turtlebot3_world_map.yaml'),");
  cap.launchDescriptions.push_back("    }.items(),)");
}

void RosEmitter::collectApproach(Capability& cap)
{
  // ======================================================================================================
  // Include dependencies
  mCmakeDeps.insert("sensor_msgs");

  // ======================================================================================================
  // Package xml dependencies
  mPackageDeps.insert("sensor_msgs");

  // ======================================================================================================
  // Constructor - These are the actions necessary for the correct construction of this capability
  {
    cap.supervisorCtor.push_back("// Approach ===============================================================");
    cap.supervisorCtor.push_back("vel_pub_ = create_publisher<geometry_msgs::msg::Twist>(\"cmd_vel\", 10);");
    cap.supervisorCtor.push_back("odom_sub_ = create_subscription<nav_msgs::msg::Odometry>(");
    cap.supervisorCtor.push_back("    \"/odom\", rclcpp::QoS(50),");
    cap.supervisorCtor.push_back("    [this](const nav_msgs::msg::Odometry::SharedPtr msg) {");
    cap.supervisorCtor.push_back("      double x = msg->pose.pose.position.x;");
    cap.supervisorCtor.push_back("      double y = msg->pose.pose.position.y;");
    cap.supervisorCtor.push_back("      const auto& q = msg->pose.pose.orientation;");
    cap.supervisorCtor.push_back("      tf2::Quaternion tq(q.x, q.y, q.z, q.w);");
    cap.supervisorCtor.push_back("      double r, p, yaw;\n");
    cap.supervisorCtor.push_back("      tf2::Matrix3x3(tq).getRPY(r, p, yaw);");
    cap.supervisorCtor.push_back("      std::lock_guard<std::mutex> lk(odom_mtx_);");
    cap.supervisorCtor.push_back("      odom_pose_.x = x;");
    cap.supervisorCtor.push_back("      odom_pose_.y = y;");
    cap.supervisorCtor.push_back("      odom_pose_.yaw = yaw;");
    cap.supervisorCtor.push_back("      have_odom_ = true;");
    cap.supervisorCtor.push_back("    },");
    cap.supervisorCtor.push_back(std::format("    {});", mOptions.subscriptionOptions));
  }

  // ======================================================================================================
  // Members - These are the members needed for this capability
  {
    cap.supervisorMembers.push_back("std::mutex odom_mtx_;");
    cap.supervisorMembers.push_back("bool have_odom_{false};");
    cap.supervisorMembers.push_back("struct OdomPose { double x; double y; double yaw; };");
    cap.supervisorMembers.push_back("OdomPose odom_pose_{0.0, 0.0, 0.0};");
    cap.supervisorMembers.push_back("rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr vel_pub_;");
    cap.supervisorMembers.push_back("rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_sub_;");

    cap.supervisorMembers.push_back("std::optional<geometry_msgs::msg::PoseStamped> lookupMarkerIn(const geometry_msgs::msg::PoseStamped& cam_pose, const std::string& target_frame);");
    cap.supervisorMembers.push_back("std::optional<OdomPose> getOdomPose();");
    cap.supervisorMembers.push_back("void stopBase();");
    cap.supervisorMembers.push_back("Result dockWithOdom(const geometry_msgs::msg::PoseStamped& cam_pose, double standoff_m, double timeout_s);");
    cap.supervisorMembers.push_back("Result undockWithOdom(double standoff_m, double timeout_s);");

    cap.supervisorMethods.push_back("template <typename T>");
    cap.supervisorMethods.push_back("static inline T clamp(T v, T lo, T hi)");
    cap.supervisorMethods.push_back("{");
    cap.supervisorMethods.push_back("  return std::max(lo, std::min(v, hi));");
    cap.supervisorMethods.push_back("}\n");
  }

  // ======================================================================================================
  // Parameters - These are the paramaters needed for this capability
  {
    cap.parameters.push_back("declare_parameter<double>(\"approach_distance\", 0.1);");
    cap.parameters.push_back("declare_parameter<double>(\"depart_distance\", 0.1);");

    cap.configParams.push_back("approach_distance: 0.25");
    cap.configParams.push_back("depart_distance: 0.2");
  }

  // ======================================================================================================
  // Methods - Here we add any methods that this capability might add to the supervisor
  // Trigger
  {
    cap.supervisorMethods.push_back(std::format("Result {}::{}Trigger({})", mOptions.supervisorClass, cap.name, argDecls(cap.trigger->args)));
    cap.supervisorMethods.push_back("{");
    cap.supervisorMethods.push_back("   if (approach)");
    cap.supervisorMethods.push_back("   {");
    cap.supervisorMethods.push_back("     geometry_msgs::msg::PoseStamped goal_pose;");
    cap.supervisorMethods.push_back("     {");
    cap.supervisorMethods.push_back("       std::lock_guard<std::mutex> lock(aruco_pose_mtx_);");
    cap.supervisorMethods.push_back("       if (!aruco_pose_)");
    cap.supervisorMethods.push_back("         return Result::Failure;\n");
    cap.supervisorMethods.push_back("       goal_pose = aruco_pose_.value();");
    cap.supervisorMethods.push_back("     }\n");
    cap.supervisorMethods.push_back("     auto pose_plan = lookupMarkerIn(goal_pose, arm_mgi_->getPlanningFrame());");
    cap.supervisorMethods.push_back("     if (!pose_plan) return Result::Failure;\n");
    cap.supervisorMethods.push_back("     return dockWithOdom(*pose_plan, get_parameter(\"approach_distance\").as_double(), 20);");
    cap.supervisorMethods.push_back("    } else");
    cap.supervisorMethods.push_back("     return undockWithOdom(get_parameter(\"depart_distance\").as_double(), 20);");
    cap.supervisorMethods.push_back("}");
  }

  // Abort
  {
    cap.supervisorMethods.push_back(std::format("Result {}::{}Abort({})", mOptions.supervisorClass, cap.name, argDecls(cap.onAbort->args)));
    cap.supervisorMethods.push_back("{");
    cap.supervisorMethods.push_back("  // We use the standard abort");
    cap.supervisorMethods.push_back("  return Result::Done;");
    cap.supervisorMethods.push_back("}");
  }

  // Extra
  {
    cap.supervisorMethods.push_back(std::format("std::optional<{}::OdomPose> {}::getOdomPose()", mOptions.supervisorClass, mOptions.supervisorClass));
    cap.supervisorMethods.push_back("{");
    cap.supervisorMethods.push_back("  std::lock_guard<std::mutex> lk(odom_mtx_);");
    cap.supervisorMethods.push_back("  if (!have_odom_)");
    cap.supervisorMethods.push_back("    return std::nullopt;\n");
    cap.supervisorMethods.push_back("  return odom_pose_;");
    cap.supervisorMethods.push_back("}\n");

    cap.supervisorMethods.push_back(std::format("void {}::stopBase()", mOptions.supervisorClass));
    cap.supervisorMethods.push_back("{");
    cap.supervisorMethods.push_back("  vel_pub_->publish(geometry_msgs::msg::Twist());");
    cap.supervisorMethods.push_back("}\n");

    cap.supervisorMethods.push_back(std::format("std::optional<geometry_msgs::msg::PoseStamped> {}::lookupMarkerIn(const geometry_msgs::msg::PoseStamped& cam_pose, const std::string& target_frame)", mOptions.supervisorClass));
    cap.supervisorMethods.push_back("{");
    cap.supervisorMethods.push_back("  RCLCPP_INFO(get_logger(),");
    cap.supervisorMethods.push_back("              \"Cam pose in(%s) target frame(%s) p = (%.3f, %.3f, %.3f) q = (%.3f, %.3f, %.3f, %.3f)\",");
    cap.supervisorMethods.push_back("              cam_pose.header.frame_id.c_str(), target_frame.c_str(),");
    cap.supervisorMethods.push_back("              cam_pose.pose.position.x, cam_pose.pose.position.y, cam_pose.pose.position.z,");
    cap.supervisorMethods.push_back("              cam_pose.pose.orientation.x, cam_pose.pose.orientation.y, cam_pose.pose.orientation.z, cam_pose.pose.orientation.w);\n");

    cap.supervisorMethods.push_back("  try");
    cap.supervisorMethods.push_back("  {");
    cap.supervisorMethods.push_back(std::format("    auto out = {}.transform(cam_pose, target_frame, tf2::durationFromSec(0.2));", mOptions.tfBuffer));
    cap.supervisorMethods.push_back("    return out;");
    cap.supervisorMethods.push_back("  } catch (const tf2::TransformException& ex)");
    cap.supervisorMethods.push_back("  {");
    cap.supervisorMethods.push_back("    RCLCPP_ERROR(get_logger(), \"TF to planning frame failed : %s\", ex.what());");
    cap.supervisorMethods.push_back("    return std::nullopt;");
    cap.supervisorMethods.push_back("  }");
    cap.supervisorMethods.push_back("}\n");

    cap.supervisorMethods.push_back(std::format("Result {}::dockWithOdom(const geometry_msgs::msg::PoseStamped& cam_pose, double standoff_m, double timeout_s)", mOptions.supervisorClass));
    cap.supervisorMethods.push_back("{");
    cap.supervisorMethods.push_back("  // 1. Get marker pose in odom frame");
    cap.supervisorMethods.push_back("  auto opt = lookupMarkerIn(cam_pose, \"odom\");");
    cap.supervisorMethods.push_back("  if (!opt)");
    cap.supervisorMethods.push_back("  {");
    cap.supervisorMethods.push_back("    RCLCPP_ERROR(get_logger(), \"Failed to convert to odom frame\");");
    cap.supervisorMethods.push_back("    return Result::Failure;");
    cap.supervisorMethods.push_back("  }\n");

    cap.supervisorMethods.push_back("  auto marker_in_odom = *opt;");
    cap.supervisorMethods.push_back("  auto current_pose = getOdomPose();");
    cap.supervisorMethods.push_back("  if (!current_pose)");
    cap.supervisorMethods.push_back("  {");
    cap.supervisorMethods.push_back("    RCLCPP_ERROR(get_logger(), \"Current pose is not available\");");
    cap.supervisorMethods.push_back("    return Result::Failure;");
    cap.supervisorMethods.push_back("  }\n");

    cap.supervisorMethods.push_back("  // 3. Control loop using odometry");
    cap.supervisorMethods.push_back(std::format("  if ({}.joinable())", mOptions.taskThread));
    cap.supervisorMethods.push_back(std::format("    {}.join();\n", mOptions.taskThread));
    cap.supervisorMethods.push_back(std::format("  {} = std::thread([this, cam_pose, standoff_m, timeout_s, marker_in_odom, current_pose] {{", mOptions.taskThread));
    cap.supervisorMethods.push_back("    rclcpp::Rate rate(20);");
    cap.supervisorMethods.push_back("    auto t0 = now();");
    cap.supervisorMethods.push_back("    const double vx = marker_in_odom.pose.position.x - current_pose->x;");
    cap.supervisorMethods.push_back("    const double vy = marker_in_odom.pose.position.y - current_pose->y;");
    cap.supervisorMethods.push_back("    const double target_yaw = std::atan2(vy, vx);");
    cap.supervisorMethods.push_back("    const double dist = std::hypot(vx, vy);");
    cap.supervisorMethods.push_back("    const double ux = vx / dist, uy = vy / dist;");
    cap.supervisorMethods.push_back("    const double target_x = marker_in_odom.pose.position.x - standoff_m * ux;");
    cap.supervisorMethods.push_back("    const double target_y = marker_in_odom.pose.position.y - standoff_m * uy;");
    cap.supervisorMethods.push_back("    RCLCPP_INFO(get_logger(), \"Dock goal in odom : (%.3f, %.3f, %.1f°)\", target_x, target_y, target_yaw * 180 / M_PI);");
    cap.supervisorMethods.push_back("    RCLCPP_INFO(get_logger(), \"Position in odom : (%.3f, %.3f, %.1f°)\", current_pose->x, current_pose->y, current_pose->yaw * 180 / M_PI);\n");
    cap.supervisorMethods.push_back("    const double k_lin = 0.8;");
    cap.supervisorMethods.push_back("    const double k_ang = 1.1;");
    cap.supervisorMethods.push_back("    const double vmax = 0.1;");
    cap.supervisorMethods.push_back("    const double wmax = 0.3;\n");

    cap.supervisorMethods.push_back("    while (rclcpp::ok())");
    cap.supervisorMethods.push_back("    {");
    cap.supervisorMethods.push_back("      if ((now() - t0).seconds() > timeout_s)");
    cap.supervisorMethods.push_back("      {");
    cap.supervisorMethods.push_back("        RCLCPP_WARN(get_logger(), \"dockWithOdom: timeout\");");
    cap.supervisorMethods.push_back("        break;");
    cap.supervisorMethods.push_back("      }\n");
    cap.supervisorMethods.push_back("      auto od = getOdomPose();");
    cap.supervisorMethods.push_back("      if (!od)");
    cap.supervisorMethods.push_back("      {");
    cap.supervisorMethods.push_back("        rate.sleep();");
    cap.supervisorMethods.push_back("        continue;");
    cap.supervisorMethods.push_back("      }\n");
    cap.supervisorMethods.push_back("      // error in odom frame");
    cap.supervisorMethods.push_back("      double dx = target_x - od->x;");
    cap.supervisorMethods.push_back("      double dy = target_y - od->y;");
    cap.supervisorMethods.push_back("      double dist = std::hypot(dx, dy);");
    cap.supervisorMethods.push_back("      double target_heading = std::atan2(dy, dx);");
    cap.supervisorMethods.push_back("      double heading_err = target_heading - od->yaw;");
    cap.supervisorMethods.push_back("      while (heading_err > M_PI) heading_err -= 2 * M_PI;");
    cap.supervisorMethods.push_back("      while (heading_err < -M_PI) heading_err += 2 * M_PI;");
    cap.supervisorMethods.push_back("      double yaw_err = target_yaw - od->yaw;");
    cap.supervisorMethods.push_back("      while (yaw_err > M_PI) yaw_err -= 2 * M_PI;");
    cap.supervisorMethods.push_back("      while (yaw_err < -M_PI) yaw_err += 2 * M_PI;");
    cap.supervisorMethods.push_back("      RCLCPP_INFO(get_logger(), \"Dock : moving. %.2fm from goal, heading err: %.1f°, yaw err %.1f°(%.3f %.3f)\", dist, heading_err * 180 / M_PI, yaw_err * 180 / M_PI, od->x, od->y);");
    cap.supervisorMethods.push_back("      if (dist < 0.02 && std::abs(yaw_err) < 0.1)");
    cap.supervisorMethods.push_back("      {");
    cap.supervisorMethods.push_back("        stopBase();");
    cap.supervisorMethods.push_back("        RCLCPP_INFO(get_logger(), \"Dock: reached (%.2fm from goal, yaw err %.1f°)\", dist, yaw_err * 180 / M_PI);");
    cap.supervisorMethods.push_back(std::format("        if ({}_{})", cap.name, cap.onReturn->name));
    cap.supervisorMethods.push_back(std::format("          {}_{}({});", cap.name, cap.onReturn->name, argNames(cap.onError->args)));
    cap.supervisorMethods.push_back("        return;");
    cap.supervisorMethods.push_back("      }\n");
    cap.supervisorMethods.push_back("      // Control: rotate toward target, move forward");
    cap.supervisorMethods.push_back("      geometry_msgs::msg::Twist cmd;");
    cap.supervisorMethods.push_back("      if (dist < 0.01)");
    cap.supervisorMethods.push_back("      {");
    cap.supervisorMethods.push_back("        cmd.linear.x = 0.0;");
    cap.supervisorMethods.push_back("        cmd.angular.z = clamp(k_ang * yaw_err, -wmax, wmax);");
    cap.supervisorMethods.push_back("      }");
    cap.supervisorMethods.push_back("      else");
    cap.supervisorMethods.push_back("      {");
    cap.supervisorMethods.push_back("        cmd.angular.z = clamp(k_ang * heading_err, -wmax, wmax);");
    cap.supervisorMethods.push_back("        cmd.linear.x = clamp(k_lin * dist, -vmax, vmax);");
    cap.supervisorMethods.push_back("      }");
    cap.supervisorMethods.push_back("      vel_pub_->publish(cmd);");
    cap.supervisorMethods.push_back("      rate.sleep();");
    cap.supervisorMethods.push_back("    }\n");
    cap.supervisorMethods.push_back("    stopBase();");
    cap.supervisorMethods.push_back(std::format("    if ({}_{})", cap.name, cap.onError->name));
    cap.supervisorMethods.push_back(std::format("      {}_{}({});", cap.name, cap.onError->name, argNames(cap.onError->args)));
    cap.supervisorMethods.push_back("    return;");
    cap.supervisorMethods.push_back("  });");
    cap.supervisorMethods.push_back("  return Result::Success;");
    cap.supervisorMethods.push_back("}");

    cap.supervisorMethods.push_back(std::format("Result {}::undockWithOdom(double back_m, double timeout_s)", mOptions.supervisorClass));
    cap.supervisorMethods.push_back("{");
    cap.supervisorMethods.push_back("  auto od0 = getOdomPose();");
    cap.supervisorMethods.push_back("  if (!od0)");
    cap.supervisorMethods.push_back("  {");
    cap.supervisorMethods.push_back("    RCLCPP_ERROR(get_logger(), \"undock: no odom\");");
    cap.supervisorMethods.push_back("    return Result::Failure;");
    cap.supervisorMethods.push_back("  }\n");
    cap.supervisorMethods.push_back(std::format("  if ({}.joinable())", mOptions.taskThread));
    cap.supervisorMethods.push_back(std::format("    {}.join();\n", mOptions.taskThread));
    cap.supervisorMethods.push_back(std::format("  {} = std::thread([this, back_m, timeout_s, od0] {{", mOptions.taskThread));
    cap.supervisorMethods.push_back("    rclcpp::Rate rate(20);");
    cap.supervisorMethods.push_back("    auto t0 = now();");
    cap.supervisorMethods.push_back("    double last_x = od0->x, last_y = od0->y, traveled = 0.0;\n");
    cap.supervisorMethods.push_back("    while (rclcpp::ok())");
    cap.supervisorMethods.push_back("    {");
    cap.supervisorMethods.push_back("      if ((now() - t0).seconds() > timeout_s)");
    cap.supervisorMethods.push_back("        break;\n");
    cap.supervisorMethods.push_back("      auto od = getOdomPose();");
    cap.supervisorMethods.push_back("      if (!od)");
    cap.supervisorMethods.push_back("      {");
    cap.supervisorMethods.push_back("        rate.sleep();");
    cap.supervisorMethods.push_back("        continue;");
    cap.supervisorMethods.push_back("      }\n");
    cap.supervisorMethods.push_back("      traveled += std::hypot(od->x - last_x, od->y - last_y);");
    cap.supervisorMethods.push_back("      last_x = od->x;");
    cap.supervisorMethods.push_back("      last_y = od->y;");
    cap.supervisorMethods.push_back("      if (traveled >= back_m)");
    cap.supervisorMethods.push_back("      {");
    cap.supervisorMethods.push_back("        stopBase();");
    cap.supervisorMethods.push_back("        RCLCPP_INFO(get_logger(), \"undock: backed %.2fm\", traveled);");
    cap.supervisorMethods.push_back(std::format("        if ({}_{})", cap.name, cap.onReturn->name));
    cap.supervisorMethods.push_back(std::format("          {}_{}({});", cap.name, cap.onReturn->name, argNames(cap.onError->args)));
    cap.supervisorMethods.push_back("        return;");
    cap.supervisorMethods.push_back("      }");
    cap.supervisorMethods.push_back("      geometry_msgs::msg::Twist cmd;");
    cap.supervisorMethods.push_back("      cmd.linear.x = -0.06;  // slow, safe");
    cap.supervisorMethods.push_back("      vel_pub_->publish(cmd);");
    cap.supervisorMethods.push_back("      rate.sleep();");
    cap.supervisorMethods.push_back("    }");
    cap.supervisorMethods.push_back("    stopBase();");
    cap.supervisorMethods.push_back(std::format("    if ({}_{})", cap.name, cap.onError->name));
    cap.supervisorMethods.push_back(std::format("      {}_{}({});", cap.name, cap.onError->name, argNames(cap.onError->args)));
    cap.supervisorMethods.push_back("    return;");
    cap.supervisorMethods.push_back("  });\n");
    cap.supervisorMethods.push_back("  return Result::Success;");
    cap.supervisorMethods.push_back("}");
  }
}

void RosEmitter::collectObjectDetection(Capability& cap)
{
  // ======================================================================================================
  // Include dependencies
  mCmakeDeps.insert("cv_bridge");
  mCmakeDeps.insert("OpenCV");
  mCmakeDeps.insert("sensor_msgs");

  // ======================================================================================================
  // Package xml dependencies
  mPackageDeps.insert("cv_bridge");
  mPackageDeps.insert("opencv");
  mPackageDeps.insert("sensor_msgs");

  // ======================================================================================================
  // Includes - These are the necessary headers
  {
    cap.supervisorIncludes.push_back("// ArucoVision ========================================================================== ");
    cap.supervisorIncludes.push_back("#include <opencv2/aruco.hpp>");
    cap.supervisorIncludes.push_back("#include <opencv2/opencv.hpp>");
    cap.supervisorIncludes.push_back("#include <sensor_msgs/msg/image.hpp>");
    cap.supervisorIncludes.push_back("#include <std_msgs/msg/color_rgba.hpp>");
    cap.supervisorIncludes.push_back("#include <cv_bridge/cv_bridge.h>");
    cap.supervisorIncludes.push_back("#include <sensor_msgs/msg/camera_info.hpp>");
    cap.supervisorIncludes.push_back("#include <sensor_msgs/msg/image.hpp>");
  }
  // ======================================================================================================
  // Members - These are the members needed for this capability
  {
    cap.supervisorMembers.push_back("// ArucoVision ========================================================================== ");
    cap.supervisorMembers.push_back("rclcpp::Subscription<sensor_msgs::msg::Image>::SharedPtr image_sub_;");
    cap.supervisorMembers.push_back("rclcpp::Subscription<sensor_msgs::msg::CameraInfo>::SharedPtr cam_info_sub_;");

    cap.supervisorMembers.push_back("cv::Ptr<cv::aruco::Dictionary> aruco_dict_;");
    cap.supervisorMembers.push_back("cv::Ptr<cv::aruco::DetectorParameters> detector_params_;");

    cap.supervisorMembers.push_back("std::mutex img_mtx_;");
    cap.supervisorMembers.push_back("std::condition_variable img_cv_;");
    cap.supervisorMembers.push_back("sensor_msgs::msg::Image::SharedPtr last_img_;");

    cap.supervisorMembers.push_back("std::array<double, 9> K_{};");
    cap.supervisorMembers.push_back("std::vector<double> D_;");
    cap.supervisorMembers.push_back("std::string cam_optical_frame_;");
    cap.supervisorMembers.push_back("std::atomic<bool> have_cam_info_{false};");
    cap.supervisorMembers.push_back("std::mutex aruco_pose_mtx_;");
    cap.supervisorMembers.push_back("std::optional<geometry_msgs::msg::PoseStamped> aruco_pose_;");

    cap.supervisorMembers.push_back("void imageCb(const sensor_msgs::msg::Image::SharedPtr msg);");
  }
  // ======================================================================================================
  // Constructor - These are the actions necessary for the correct construction of this capability
  {
    cap.supervisorCtor.push_back("// ArucoVision ========================================================================== ");
    cap.supervisorCtor.push_back("aruco_dict_ = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);");
    cap.supervisorCtor.push_back("detector_params_ = cv::aruco::DetectorParameters::create();");
    cap.supervisorCtor.push_back("image_sub_ = create_subscription<sensor_msgs::msg::Image>(");
    cap.supervisorCtor.push_back("     get_parameter(\"camera_topic\").as_string(),");
    cap.supervisorCtor.push_back("     rclcpp::SensorDataQoS(),");
    cap.supervisorCtor.push_back(std::format("     std::bind(&{}::imageCb, this, std::placeholders::_1),", mOptions.supervisorClass));
    cap.supervisorCtor.push_back(std::format("  {});", mOptions.subscriptionOptions));
    cap.supervisorCtor.push_back("cam_info_sub_ = create_subscription<sensor_msgs::msg::CameraInfo>(");
    cap.supervisorCtor.push_back("    \"/pi_camera/camera_info\", 10,");
    cap.supervisorCtor.push_back("    [this](const sensor_msgs::msg::CameraInfo::SharedPtr msg) {");
    cap.supervisorCtor.push_back("      for (int i = 0; i < 9; ++i)");
    cap.supervisorCtor.push_back("        K_[i] = msg->k[i];");
    cap.supervisorCtor.push_back("      D_ = msg->d;");
    cap.supervisorCtor.push_back("      cam_optical_frame_ = msg->header.frame_id;  // usually ..._optical_frame");
    cap.supervisorCtor.push_back("      have_cam_info_.store(true);");
    cap.supervisorCtor.push_back("      RCLCPP_INFO_THROTTLE(get_logger(), *get_clock(), 2000, \"Got CameraInfo %s\", cam_optical_frame_.c_str());");
    cap.supervisorCtor.push_back("      auto tmp = cam_info_sub_;");
    cap.supervisorCtor.push_back("      cam_info_sub_.reset();");
    cap.supervisorCtor.push_back("    },");
    cap.supervisorCtor.push_back(std::format("    {});", mOptions.subscriptionOptions));
  }
  // ======================================================================================================
  // Parameters - These are the paramaters needed for this capability
  {
    cap.parameters.push_back("// ArucoVision ========================================================================== ");
    cap.parameters.push_back("declare_parameter<std::string>(\"camera_topic\", \"/pi_camera/image_raw\");");
    cap.parameters.push_back("declare_parameter<std::string>(\"camera_frame\", \"camera_link\");");
    cap.parameters.push_back("declare_parameter<int>(\"aruco_id\", 23);");
    cap.parameters.push_back("declare_parameter<double>(\"aruco_size_m\", 0.08);");
    cap.parameters.push_back("declare_parameter<double>(\"arrival_tolerance_m\", 0.30);");
    cap.parameters.push_back("declare_parameter<double>(\"aruco_timeout_s\", 15.0);");

    cap.configParams.push_back("aruco_size_m: 0.08");
    cap.configParams.push_back("arrival_tolerance_m: 0.30");
    cap.configParams.push_back("aruco_timeout_s: 15.0");
  }
  // ======================================================================================================
  // Start - Here we add any start up actions needed by this capability

  // ======================================================================================================
  // Methods - Here we add any methods that this capability might add to the supervisor
  // Trigger
  {
    cap.supervisorMethods.push_back("// ArucoVision ========================================================================== ");
    cap.supervisorMethods.push_back(std::format("Result {}::{}Trigger({})", mOptions.supervisorClass, cap.name, argDecls(cap.trigger->args)));
    cap.supervisorMethods.push_back("{");
    cap.supervisorMethods.push_back(std::format("  if ({}.joinable())", mOptions.taskThread));
    cap.supervisorMethods.push_back(std::format("    {}.join();\n", mOptions.taskThread));
    cap.supervisorMethods.push_back(std::format("  {} = std::thread([this] {{", mOptions.taskThread));
    cap.supervisorMethods.push_back("    const int target_id = get_parameter(\"aruco_id\").as_int();");
    cap.supervisorMethods.push_back("    const double size_m = get_parameter(\"aruco_size_m\").as_double();");
    cap.supervisorMethods.push_back("    const double tol_m = get_parameter(\"arrival_tolerance_m\").as_double();");
    cap.supervisorMethods.push_back("    RCLCPP_INFO(get_logger(), \"Checking ArUco id=%d within %.2f size and %.2fm tolerance...\", target_id, size_m, tol_m);\n");

    cap.supervisorMethods.push_back("    while (true) {");
    cap.supervisorMethods.push_back("      {");
    cap.supervisorMethods.push_back(std::format("        std::unique_lock<std::mutex> lock({});", mOptions.abortLock));
    cap.supervisorMethods.push_back(std::format("        if ({})", mOptions.abortFlag));
    cap.supervisorMethods.push_back("          break;");
    cap.supervisorMethods.push_back("      }\n");
    cap.supervisorMethods.push_back("      sensor_msgs::msg::Image::SharedPtr img;");
    cap.supervisorMethods.push_back("      {");
    cap.supervisorMethods.push_back("        std::unique_lock<std::mutex> lk(img_mtx_);");
    cap.supervisorMethods.push_back("        if (!img_cv_.wait_for(lk, 500ms, [&] { return last_img_ != nullptr; }))");
    cap.supervisorMethods.push_back("          continue;");
    cap.supervisorMethods.push_back("        img = last_img_;");
    cap.supervisorMethods.push_back("        last_img_.reset();  // consume");
    cap.supervisorMethods.push_back("      }\n");
    cap.supervisorMethods.push_back("      cv::Mat frame;");
    cap.supervisorMethods.push_back("      try {");
    cap.supervisorMethods.push_back("        frame = cv_bridge::toCvCopy(img, \"bgr8\")->image;");
    cap.supervisorMethods.push_back("      } catch (const std::exception& e) {");
    cap.supervisorMethods.push_back("        RCLCPP_WARN(get_logger(), \"Failed to cv_bridge::toCvCopy\");");
    cap.supervisorMethods.push_back("        continue;");
    cap.supervisorMethods.push_back("      }");
    cap.supervisorMethods.push_back("      if (frame.empty()) {\n");
    cap.supervisorMethods.push_back("        RCLCPP_WARN(get_logger(), \"Frame is empty\");");
    cap.supervisorMethods.push_back("        continue;");
    cap.supervisorMethods.push_back("      }\n");
    cap.supervisorMethods.push_back("      std::vector<int> ids;");
    cap.supervisorMethods.push_back("      std::vector<std::vector<cv::Point2f>> corners;");
    cap.supervisorMethods.push_back("      cv::aruco::detectMarkers(frame, aruco_dict_, corners, ids, detector_params_);");
    cap.supervisorMethods.push_back("      if (ids.empty()) {");
    cap.supervisorMethods.push_back("        RCLCPP_WARN(get_logger(), \"Failed to detect markers, saving image\");");
    cap.supervisorMethods.push_back("        cv::imwrite(\"/tmp/cam.png\", frame);");
    cap.supervisorMethods.push_back("        continue;");
    cap.supervisorMethods.push_back("      } else {");
    cap.supervisorMethods.push_back("        RCLCPP_INFO(get_logger(), \"Found %ld ids\", ids.size());");
    cap.supervisorMethods.push_back("        cv::Mat K = (cv::Mat_<double>(3, 3) << K_[0], K_[1], K_[2], K_[3], K_[4], K_[5], K_[6], K_[7], K_[8]);");
    cap.supervisorMethods.push_back("        cv::Mat D(D_);");
    cap.supervisorMethods.push_back("        std::vector<cv::Vec3d> rvecs, tvecs;");
    cap.supervisorMethods.push_back("        cv::aruco::estimatePoseSingleMarkers(corners, size_m, K, D, rvecs, tvecs);");
    cap.supervisorMethods.push_back("        for (size_t i = 0; i < ids.size(); ++i)");
    cap.supervisorMethods.push_back("        {");
    cap.supervisorMethods.push_back("          if (ids[i] != target_id) continue;");
    cap.supervisorMethods.push_back("          cv::Mat Rcv;");
    cap.supervisorMethods.push_back("          cv::Rodrigues(rvecs[i], Rcv);  // 3x3 rotation matrix");
    cap.supervisorMethods.push_back("          tf2::Matrix3x3 R(");
    cap.supervisorMethods.push_back("              Rcv.at<double>(0, 0), Rcv.at<double>(0, 1), Rcv.at<double>(0, 2),");
    cap.supervisorMethods.push_back("              Rcv.at<double>(1, 0), Rcv.at<double>(1, 1), Rcv.at<double>(1, 2),");
    cap.supervisorMethods.push_back("              Rcv.at<double>(2, 0), Rcv.at<double>(2, 1), Rcv.at<double>(2, 2));");
    cap.supervisorMethods.push_back("          tf2::Quaternion q;");
    cap.supervisorMethods.push_back("          R.getRotation(q);");
    cap.supervisorMethods.push_back("          geometry_msgs::msg::PoseStamped out;");
    cap.supervisorMethods.push_back("          out.header.stamp = now();");
    cap.supervisorMethods.push_back("          out.header.frame_id = \"camera_rgb_optical_frame\";");
    cap.supervisorMethods.push_back("          out.pose.position.x = tvecs[i][0];");
    cap.supervisorMethods.push_back("          out.pose.position.y = tvecs[i][1];");
    cap.supervisorMethods.push_back("          out.pose.position.z = tvecs[i][2];");
    cap.supervisorMethods.push_back("          out.pose.orientation = tf2::toMsg(q.normalize());");
    cap.supervisorMethods.push_back("          RCLCPP_ERROR(get_logger(), \"Found ArUco (%s) %.2f %.2f %.2f\", cam_optical_frame_.c_str(), out.pose.position.x, out.pose.position.y, out.pose.position.z);\n");
    cap.supervisorMethods.push_back("          {");
    cap.supervisorMethods.push_back("            std::lock_guard<std::mutex> lock(aruco_pose_mtx_);");
    cap.supervisorMethods.push_back("            aruco_pose_ = out;");
    cap.supervisorMethods.push_back("          }");
    cap.supervisorMethods.push_back(std::format("          if ({}_{})", cap.name, cap.onReturn->name));
    cap.supervisorMethods.push_back(std::format("            {}_{}({});", cap.name, cap.onReturn->name, argNames(cap.onReturn->args)));
    cap.supervisorMethods.push_back("          break;");
    cap.supervisorMethods.push_back("        }");
    cap.supervisorMethods.push_back("        return;");
    cap.supervisorMethods.push_back("      }");
    cap.supervisorMethods.push_back("    }");
    cap.supervisorMethods.push_back("  });");
    cap.supervisorMethods.push_back("  return Result::Success;");
    cap.supervisorMethods.push_back("}");
  }

  // Abort
  {
    cap.supervisorMethods.push_back(std::format("Result {}::{}Abort({})", mOptions.supervisorClass, cap.name, argDecls(cap.onAbort->args)));
    cap.supervisorMethods.push_back("{");
    cap.supervisorMethods.push_back("  // We use the standard abort");
    cap.supervisorMethods.push_back("  return Result::Done;");
    cap.supervisorMethods.push_back("}");
  }

  cap.supervisorMethods.push_back(std::format("void {}::imageCb(const sensor_msgs::msg::Image::SharedPtr msg)", mOptions.supervisorClass));
  cap.supervisorMethods.push_back("{");
  cap.supervisorMethods.push_back("  {");
  cap.supervisorMethods.push_back("    std::unique_lock<std::mutex> lk(img_mtx_);");
  cap.supervisorMethods.push_back("    last_img_ = msg;");
  cap.supervisorMethods.push_back("  }");
  cap.supervisorMethods.push_back("  img_cv_.notify_all();");
  cap.supervisorMethods.push_back("}");
}

void RosEmitter::collectGrip(Capability& cap)
{
  mCmakeDeps.insert("moveit_ros_planning_interface");
  mCmakeDeps.insert("tf2_geometry_msgs");

  // ======================================================================================================
  // Package xml dependencies
  mPackageDeps.insert("moveit_ros_planning_interface");
  mPackageDeps.insert("tf2_geometry_msgs");

  // ======================================================================================================
  // Includes - These are the necessary headers
  cap.supervisorIncludes.push_back("// Grip ========================================================================== ");
  cap.supervisorIncludes.push_back("#include <moveit/move_group_interface/move_group_interface.h>");
  cap.supervisorIncludes.push_back("#include <tf2_geometry_msgs/tf2_geometry_msgs.hpp>");

  // ======================================================================================================
  // Members - These are the members needed for this capability
  cap.supervisorMembers.push_back("// Grip ========================================================================== ");
  cap.supervisorMembers.push_back("std::shared_ptr<moveit::planning_interface::MoveGroupInterface> arm_mgi_;");
  cap.supervisorMembers.push_back("std::shared_ptr<moveit::planning_interface::MoveGroupInterface> gripper_mgi_;");
  cap.supervisorMembers.push_back("bool moveGripperHome();\n");
  cap.supervisorMembers.push_back("geometry_msgs::msg::PoseStamped applyToolOffset(const geometry_msgs::msg::PoseStamped& at_marker);\n");

  // ======================================================================================================
  // Constructor - These are the actions necessary for the correct construction of this capability

  // ======================================================================================================
  // Parameters - These are the paramaters needed for this capability
  cap.parameters.push_back("// Grip ========================================================================== ");
  cap.parameters.push_back("declare_parameter<std::vector<double>>(\"target_offset_xyz\", {0.18, 0.0, 0.2});");
  cap.parameters.push_back("declare_parameter<std::vector<double>>(\"target_offset_rpy\", {0.0, 0.0, 0.03});");
  cap.parameters.push_back("declare_parameter<std::vector<double>>(\"drive_pose\", {-0.068, 0.0, 0.26});");
  cap.parameters.push_back("declare_parameter<std::string>(\"arm_group\", \"arm\");");
  cap.parameters.push_back("declare_parameter<std::string>(\"eef_link\", \"end_effector_link\");\n");

  cap.configParams.push_back("arm_group: \"arm\"");
  cap.configParams.push_back("eef_link: \"end_effector_link\"");
  cap.configParams.push_back("drive_pose: [-0.068, 0.0, 0.26]");
  cap.configParams.push_back("target_offset_xyz: [0.2, 0.0, 0.15]");
  cap.configParams.push_back("target_offset_rpy: [0.0, 0.0, 0.2]");

  // ======================================================================================================
  // Start - Here we add any start up actions needed by this capability
  cap.supervisorStart.push_back("if (arm_mgi_ == nullptr){ ");
  cap.supervisorStart.push_back("  arm_mgi_ = std::make_shared<moveit::planning_interface::MoveGroupInterface>(shared_from_this(), get_parameter(\"arm_group\").as_string());");
  cap.supervisorStart.push_back("  arm_mgi_->setEndEffectorLink(get_parameter(\"eef_link\").as_string());");
  cap.supervisorStart.push_back("}");
  cap.supervisorStart.push_back("if (gripper_mgi_ == nullptr){ ");
  cap.supervisorStart.push_back("  gripper_mgi_ = std::make_shared<moveit::planning_interface::MoveGroupInterface>(shared_from_this(), \"gripper\");");
  cap.supervisorStart.push_back("}");
  cap.supervisorStart.push_back("if (!moveGripperHome()) {");
  cap.supervisorStart.push_back("  RCLCPP_ERROR(get_logger(), \"Failed to set arm to drive position.\");");
  cap.supervisorStart.push_back("  rclcpp::shutdown();");
  cap.supervisorStart.push_back("  return;");
  cap.supervisorStart.push_back("}");

  // ======================================================================================================
  // Methods - Here we add any methods that this capability might add to the supervisor
  // Trigger
  {
    cap.supervisorMethods.push_back(std::format("Result {}::{}Trigger({})", mOptions.supervisorClass, cap.name, argDecls(cap.trigger->args)));
    cap.supervisorMethods.push_back("{");
    cap.supervisorMethods.push_back("  geometry_msgs::msg::PoseStamped goal_pose;");
    cap.supervisorMethods.push_back("  {");
    cap.supervisorMethods.push_back("    std::lock_guard<std::mutex> lock(aruco_pose_mtx_);");
    cap.supervisorMethods.push_back("    if (!aruco_pose_)");
    cap.supervisorMethods.push_back("      return Result::Failure;\n");
    cap.supervisorMethods.push_back("    goal_pose = aruco_pose_.value();");
    cap.supervisorMethods.push_back("  }");
    cap.supervisorMethods.push_back(std::format("  if ({}.joinable())", mOptions.taskThread));
    cap.supervisorMethods.push_back(std::format("    {}.join();\n", mOptions.taskThread));
    cap.supervisorMethods.push_back(std::format("  {} = std::thread([this, goal_pose, grip] {{", mOptions.taskThread));
    cap.supervisorMethods.push_back("    auto goal = applyToolOffset(goal_pose);");
    cap.supervisorMethods.push_back("    const auto planning_frame = arm_mgi_->getPlanningFrame();");
    cap.supervisorMethods.push_back("    arm_mgi_->setPoseReferenceFrame(planning_frame);");
    cap.supervisorMethods.push_back("    arm_mgi_->setStartStateToCurrentState();");
    cap.supervisorMethods.push_back("    arm_mgi_->clearPoseTargets();");
    cap.supervisorMethods.push_back("    arm_mgi_->setGoalTolerance(0.01);");
    cap.supervisorMethods.push_back("    arm_mgi_->setGoalOrientationTolerance(0.25);");
    cap.supervisorMethods.push_back("    arm_mgi_->setPlanningTime(10.0);");
    cap.supervisorMethods.push_back("    arm_mgi_->setNumPlanningAttempts(10);\n");
    cap.supervisorMethods.push_back("    arm_mgi_->setPositionTarget(goal.pose.position.x, goal.pose.position.y, goal.pose.position.z, arm_mgi_->getEndEffectorLink());\n");
    cap.supervisorMethods.push_back("    moveit::planning_interface::MoveGroupInterface::Plan plan;");
    cap.supervisorMethods.push_back("    if (arm_mgi_->plan(plan) != moveit::core::MoveItErrorCode::SUCCESS) {");
    cap.supervisorMethods.push_back("      RCLCPP_ERROR(get_logger(), \"MoveIt plan failed.\");");
    cap.supervisorMethods.push_back(std::format("      if ({}_{})", cap.name, cap.onError->name));
    cap.supervisorMethods.push_back(std::format("        {}_{}();", cap.name, cap.onError->name, argNames(cap.onError->args)));
    cap.supervisorMethods.push_back("      return;");
    cap.supervisorMethods.push_back("    }\n");
    cap.supervisorMethods.push_back("    if (arm_mgi_->execute(plan) != moveit::core::MoveItErrorCode::SUCCESS) {");
    cap.supervisorMethods.push_back("      RCLCPP_ERROR(get_logger(), \"MoveIt exec failed.\");");
    cap.supervisorMethods.push_back(std::format("      if ({}_{})", cap.name, cap.onError->name));
    cap.supervisorMethods.push_back(std::format("        {}_{}();", cap.name, cap.onError->name, argNames(cap.onError->args)));
    cap.supervisorMethods.push_back("      return;");
    cap.supervisorMethods.push_back("    }\n");
    // TODO: Do we really need this?
    cap.supervisorMethods.push_back("    rclcpp::sleep_for(std::chrono::seconds(1));\n");
    cap.supervisorMethods.push_back("    if (gripper_mgi_) {");
    cap.supervisorMethods.push_back("      gripper_mgi_->setNamedTarget(grip ? \"close\" : \"open\");");
    cap.supervisorMethods.push_back("      gripper_mgi_->move();");
    cap.supervisorMethods.push_back("    }\n");
    cap.supervisorMethods.push_back("    if (!moveGripperHome())");
    cap.supervisorMethods.push_back("    {");
    cap.supervisorMethods.push_back("      RCLCPP_ERROR(get_logger(), \"Failed to set arm to drive position.\");");
    cap.supervisorMethods.push_back(std::format("      if ({}_{})", cap.name, cap.onError->name));
    cap.supervisorMethods.push_back(std::format("        {}_{}();", cap.name, cap.onError->name, argNames(cap.onError->args)));
    cap.supervisorMethods.push_back("      return;");
    cap.supervisorMethods.push_back("    }\n");
    cap.supervisorMethods.push_back(std::format("    if ({}_{})", cap.name, cap.onReturn->name));
    cap.supervisorMethods.push_back(std::format("      {}_{}();", cap.name, cap.onReturn->name, argNames(cap.onReturn->args)));
    cap.supervisorMethods.push_back("  });");
    cap.supervisorMethods.push_back("  return Result::Success;");
    cap.supervisorMethods.push_back("}\n");
  }

  // Abort
  {
    cap.supervisorMethods.push_back(std::format("Result {}::{}Abort({})", mOptions.supervisorClass, cap.name, argDecls(cap.onAbort->args)));
    cap.supervisorMethods.push_back("{");
    cap.supervisorMethods.push_back("  // We use the standard abort");
    cap.supervisorMethods.push_back("  return Result::Done;");
    cap.supervisorMethods.push_back("}\n");
  }

  cap.supervisorMethods.push_back(std::format("geometry_msgs::msg::PoseStamped {}::applyToolOffset(const geometry_msgs::msg::PoseStamped& at_marker)", mOptions.supervisorClass));
  cap.supervisorMethods.push_back("{");
  cap.supervisorMethods.push_back("  auto offs_xyz = get_parameter(\"target_offset_xyz\").as_double_array();");
  cap.supervisorMethods.push_back("  auto offs_rpy = get_parameter(\"target_offset_rpy\").as_double_array();\n");
  cap.supervisorMethods.push_back("  RCLCPP_INFO(get_logger(),");
  cap.supervisorMethods.push_back("              \"Position before (%s) p=(%.3f, %.3f, %.3f) q=(%.3f, %.3f, %.3f, %.3f)\",");
  cap.supervisorMethods.push_back("              at_marker.header.frame_id.c_str(),");
  cap.supervisorMethods.push_back("              at_marker.pose.position.x, at_marker.pose.position.y, at_marker.pose.position.z,");
  cap.supervisorMethods.push_back("              at_marker.pose.orientation.x, at_marker.pose.orientation.y, at_marker.pose.orientation.z, at_marker.pose.orientation.w);\n");
  cap.supervisorMethods.push_back("  geometry_msgs::msg::PoseStamped tgt = at_marker;  // same frame_id (base_footprint)");
  cap.supervisorMethods.push_back("  // 1) Position offset in base_footprint (world) axes");
  cap.supervisorMethods.push_back("  tgt.pose.position.x = offs_xyz[0];");
  cap.supervisorMethods.push_back("  tgt.pose.position.y = offs_xyz[1];");
  cap.supervisorMethods.push_back("  tgt.pose.position.z = offs_xyz[2];");
  cap.supervisorMethods.push_back("  // 2) Orientation offset relative to world (optional)");
  cap.supervisorMethods.push_back("  tf2::Quaternion q_world_offset;");
  cap.supervisorMethods.push_back("  q_world_offset.setRPY(offs_rpy[0], offs_rpy[1], offs_rpy[2]);  // radians");
  cap.supervisorMethods.push_back("  tf2::Quaternion q_marker_world;");
  cap.supervisorMethods.push_back("  tf2::fromMsg(at_marker.pose.orientation, q_marker_world);");
  cap.supervisorMethods.push_back("  // If you want to keep the same orientation but add a world-frame tweak, pre-multiply:");
  cap.supervisorMethods.push_back("  tf2::Quaternion q_target_world = q_world_offset * q_marker_world;");
  cap.supervisorMethods.push_back("  q_target_world.normalize();");
  cap.supervisorMethods.push_back("  tgt.pose.orientation = tf2::toMsg(q_world_offset);");
  cap.supervisorMethods.push_back("  return tgt;");
  cap.supervisorMethods.push_back("}");

  cap.supervisorMethods.push_back(std::format("bool {}::moveGripperHome()", mOptions.supervisorClass));
  cap.supervisorMethods.push_back("{");
  cap.supervisorMethods.push_back("  if (!arm_mgi_)");
  cap.supervisorMethods.push_back("  {");
  cap.supervisorMethods.push_back("    RCLCPP_WARN(get_logger(), \"No arm mgi\");");
  cap.supervisorMethods.push_back("    return false;");
  cap.supervisorMethods.push_back("  }\n");
  cap.supervisorMethods.push_back("  auto drive_pose = get_parameter(\"drive_pose\").as_double_array();\n");
  cap.supervisorMethods.push_back("  const auto planning_frame = arm_mgi_->getPlanningFrame();");
  cap.supervisorMethods.push_back("  arm_mgi_->setPoseReferenceFrame(planning_frame);");
  cap.supervisorMethods.push_back("  arm_mgi_->setStartStateToCurrentState();");
  cap.supervisorMethods.push_back("  arm_mgi_->clearPoseTargets();");
  cap.supervisorMethods.push_back("  arm_mgi_->setGoalTolerance(0.01);             // 1 cm");
  cap.supervisorMethods.push_back("  arm_mgi_->setGoalOrientationTolerance(0.25);  // ~20°");
  cap.supervisorMethods.push_back("  arm_mgi_->setPlanningTime(10.0);");
  cap.supervisorMethods.push_back("  arm_mgi_->setNumPlanningAttempts(10);\n");
  cap.supervisorMethods.push_back("  arm_mgi_->setPositionTarget(drive_pose[0], drive_pose[1], drive_pose[2], arm_mgi_->getEndEffectorLink());\n");
  cap.supervisorMethods.push_back("  moveit::planning_interface::MoveGroupInterface::Plan plan;");
  cap.supervisorMethods.push_back("  if (arm_mgi_->plan(plan) != moveit::core::MoveItErrorCode::SUCCESS) {");
  cap.supervisorMethods.push_back("    RCLCPP_ERROR(get_logger(), \"MoveIt plan to home failed.\");");
  cap.supervisorMethods.push_back("    return false;");
  cap.supervisorMethods.push_back("  }\n");
  cap.supervisorMethods.push_back("  if (arm_mgi_->execute(plan) != moveit::core::MoveItErrorCode::SUCCESS)");
  cap.supervisorMethods.push_back("  {");
  cap.supervisorMethods.push_back("    RCLCPP_ERROR(get_logger(), \"MoveIt exec failed.\");");
  cap.supervisorMethods.push_back("    return false;");
  cap.supervisorMethods.push_back("  }\n");
  cap.supervisorMethods.push_back("  return true;");
  cap.supervisorMethods.push_back("}\n");

  cap.launchNodes.push_back("move_group");
  cap.launchDescriptions.push_back("move_group = IncludeLaunchDescription(");
  cap.launchDescriptions.push_back("        PythonLaunchDescriptionSource(");
  cap.launchDescriptions.push_back("            PathJoinSubstitution([");
  cap.launchDescriptions.push_back("                FindPackageShare('turtlebot3_manipulation_moveit_config'),");
  cap.launchDescriptions.push_back("                'launch',");
  cap.launchDescriptions.push_back("                'move_group.launch.py',");
  cap.launchDescriptions.push_back("            ])");
  cap.launchDescriptions.push_back("        )");
  cap.launchDescriptions.push_back("    )");
}

}  // namespace koda