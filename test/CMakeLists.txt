project(FaoxiAmpPluginTest)

enable_testing()

cpmaddpackage(
        NAME
        GOOGLETEST
        GITHUB_REPOSITORY
        google/googletest
        VERSION
        1.16.0
        SOURCE_DIR
        ${LIB_DIR}/googletest
        OPTIONS
        "INSTALL_GTEST OFF"
        "gtest_force_shared_crt ON"
)

add_executable(FaoxiAmpPluginTest
        source/PluginProcessorTest.cpp
        source/InputGainTest.cpp
        source/WaveshaperTest.cpp
)

target_compile_definitions(FaoxiAmpPluginTest
        PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        # Re-exported because faoxiamp_core is not a plugin target
        JucePlugin_Manufacturer="$<TARGET_PROPERTY:FaoxiAmpPlugin,JUCE_COMPANY_NAME>"
        JucePlugin_Name="$<TARGET_PROPERTY:FaoxiAmpPlugin,JUCE_PLUGIN_NAME>"
        JucePlugin_VersionString="$<TARGET_PROPERTY:FaoxiAmpPlugin,JUCE_VERSION>"
)

target_link_libraries(
        FaoxiAmpPluginTest
        PRIVATE
        faoxiamp_core
        GTest::gtest_main
        juce::juce_recommended_warning_flags
)

include(GoogleTest)
gtest_discover_tests(FaoxiAmpPluginTest DISCOVERY_TIMEOUT 60 DISCOVERY_MODE PRE_TEST)
