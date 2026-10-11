/*
==============================================================================

BEGIN_JUCE_MODULE_DECLARATION

   ID:            faoxiamp_plugin
   vendor:        Faoxi
   version:       0.1.0
   name:          FaoxiAmp Plugin
   description:   Core of the FaoxiAmp guitar amp simulation plugin
   dependencies:  juce_audio_utils, juce_dsp

   license:       MIT

END_JUCE_MODULE_DECLARATION

==============================================================================
*/

#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_audio_utils/juce_audio_utils.h>
#include <juce_core/juce_core.h>
#include <juce_dsp/juce_dsp.h>
#include <juce_events/juce_events.h>
#include <juce_graphics/juce_graphics.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include <algorithm>
#include <cmath>
#include <memory>
#include <ranges>

// Order matters: a header must come after the headers it depends on.
#include "include/FaoxiAmp/Parameters.h"
#include "include/FaoxiAmp/InputGain.h"
#include "include/FaoxiAmp/PluginProcessor.h"
