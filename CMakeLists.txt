# Same structure as the WolfSound JUCE course project (course-project-tremolo).
cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

# Enable setting CMAKE_MSVC_DEBUG_INFORMATION_FORMAT
set(CMAKE_POLICY_DEFAULT_CMP0141 NEW CACHE STRING "" FORCE)

# macOS: minimum deployment target required by JUCE,
# universal binary for Apple Silicon (arm64) and Intel Macs
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11" CACHE STRING "" FORCE)
set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "" FORCE)

project(FaoxiAmp VERSION 0.1.0)

if(PROJECT_IS_TOP_LEVEL)
  if(NOT CMAKE_CXX_STANDARD)
    set(CMAKE_CXX_STANDARD 23)
  endif()
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<$<CONFIG:Debug,RelWithDebInfo>:Embedded>" CACHE STRING "" FORCE)
endif()

# Dependencies are downloaded next to the project (ignored by git).
set(LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/libs)

include(cmake/cpm.cmake)

cpmaddpackage(
  NAME
    JUCE
  GIT_TAG
    8.0.12
  GITHUB_REPOSITORY
    juce-framework/JUCE
  # Downloads only the tagged version, not the whole git history
  # (much smaller, less likely to fail on a slow connection).
  GIT_SHALLOW
    TRUE
  SOURCE_DIR
    ${LIB_DIR}/juce
)

include(cmake/CompilerWarnings.cmake)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
  set_directory_properties(PROPERTIES JUCE_VST3_COPY_DIR "$ENV{LOCALAPPDATA}/Programs/Common/VST3")
endif()

set(FAOXIAMP_PLUGIN_NAME "FaoxiAmp")
juce_add_plugin(
  FaoxiAmpPlugin
  PRODUCT_NAME
    ${FAOXIAMP_PLUGIN_NAME}
  VERSION
    0.0.0 # 0 forces AU hosts (Logic) to always rescan the plugin
  COMPANY_NAME
    "Faoxi"
  PLUGIN_MANUFACTURER_CODE
    Faox
  PLUGIN_CODE
    Famp
  FORMATS
    Standalone
    AU
    VST3
  IS_SYNTH
    FALSE
  NEEDS_MIDI_INPUT
    FALSE
  NEEDS_MIDI_OUTPUT
    FALSE
  VST3_AUTO_MANIFEST
    FALSE
  # Copies the AU/VST3 into ~/Library/Audio/Plug-Ins after each build,
  # so Logic finds the latest version.
  COPY_PLUGIN_AFTER_BUILD
    TRUE
)

target_compile_definitions(FaoxiAmpPlugin
  PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_VST3_CAN_REPLACE_VST2=0
)

# The plugin core is a JUCE module, like in the course:
# faoxiamp_plugin/faoxiamp_plugin.h lists every header,
# faoxiamp_plugin/faoxiamp_plugin.cpp includes every source file.
juce_add_module(faoxiamp_plugin ALIAS_NAMESPACE faoxiamp)

target_compile_definitions(faoxiamp_plugin
  INTERFACE
    FAOXIAMP_PLUGIN_NAME="${FAOXIAMP_PLUGIN_NAME}"
)

target_compile_options(faoxiamp_plugin INTERFACE "${PROJECT_WARNINGS_CXX}")

target_link_libraries(
  FaoxiAmpPlugin
  PRIVATE
    faoxiamp::faoxiamp_plugin
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)


option(BUILD_TESTS "When on, downloads GoogleTest and adds the automated tests project" OFF)
if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()
