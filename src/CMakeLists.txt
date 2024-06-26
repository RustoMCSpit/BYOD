if(NOT (IOS OR BUILD_RELEASE))
    message(STATUS "Configuring BYOD headless")
    add_subdirectory(headless)
endif()

# main source files
target_sources(BYOD PRIVATE
    BYOD.cpp

    gui/BYODPluginEditor.cpp
    gui/TitleBar.cpp

    gui/pedalboard/BoardComponent.cpp
    gui/pedalboard/BoardViewport.cpp
    gui/pedalboard/InfoComponent.cpp
    gui/pedalboard/cables/CableDrawingHelpers.cpp
    gui/pedalboard/cables/CableViewConnectionHelper.cpp
    gui/pedalboard/cables/CableViewPortLocationHelper.cpp
    gui/pedalboard/cables/CableView.cpp
    gui/pedalboard/cables/Cable.cpp
    gui/pedalboard/editors/KnobsComponent.cpp
    gui/pedalboard/editors/Port.cpp
    gui/pedalboard/editors/ProcessorEditor.cpp
    gui/pedalboard/editors/EditorSelector.cpp

    gui/toolbar/ToolBar.cpp
    gui/toolbar/UndoRedoComponent.cpp
    gui/toolbar/SettingsButton.cpp
    gui/toolbar/GlobalParamControls.cpp
    gui/toolbar/presets/PresetsComp.cpp
    gui/toolbar/presets/PresetsLoginDialog.cpp
    gui/toolbar/presets/PresetsSaveDialog.cpp
    gui/toolbar/presets/PresetSearchWindow.cpp
    gui/toolbar/presets/PresetSearchHelpers.cpp
    gui/toolbar/presets/PresetsSyncDialog.cpp

    gui/utils/HostContextProvider.cpp
    gui/utils/LevelMeterComponent.cpp
    gui/utils/LookAndFeels.cpp
    gui/utils/ModulatableSlider.cpp
    gui/utils/TextSlider.cpp
    gui/utils/ErrorMessageView.cpp

    state/StateManager.cpp
    state/ParamForwardManager.cpp
    state/presets/PresetInfoHelpers.cpp
    state/presets/PresetManager.cpp
    state/presets/PresetDiscovery.cpp
    state/presets/PresetsServerSyncManager.cpp
    state/presets/PresetsServerUserManager.cpp
    state/presets/PresetsServerCommunication.cpp

    processors/BaseProcessor.cpp
    processors/ProcessorStore.cpp
    
    processors/chain/ChainIOProcessor.cpp
    processors/chain/DryWetProcessor.cpp
    processors/chain/ProcessorChain.cpp
    processors/chain/ProcessorChainActions.cpp
    processors/chain/ProcessorChainActionHelper.cpp
    processors/chain/ProcessorChainPortMagnitudesHelper.cpp
    processors/chain/ProcessorChainStateHelper.cpp

    processors/drive/GuitarMLAmp.cpp
    processors/drive/MetalFace.cpp
    processors/drive/BassFace.cpp
    processors/drive/BlondeDrive.cpp
    processors/drive/RangeBooster.cpp
    processors/drive/RONN.cpp
    processors/drive/Warp.cpp
    processors/drive/big_muff/BigMuffDrive.cpp
    processors/drive/big_muff/BigMuffClippingStage.cpp
    processors/drive/centaur/Centaur.cpp
    processors/drive/centaur/GainStageML.cpp
    processors/drive/centaur/GainStageProc.cpp
    processors/drive/diode_circuits/DiodeClipper.cpp
    processors/drive/diode_circuits/DiodeRectifier.cpp
    processors/drive/flapjack/Flapjack.cpp
    processors/drive/fuzz_machine/FuzzMachine.cpp
    processors/drive/hysteresis/Hysteresis.cpp
    processors/drive/hysteresis/HysteresisProcessing.cpp
    processors/drive/junior_b/JuniorB.cpp
    processors/drive/king_of_tone/KingOfToneDrive.cpp
    processors/drive/mouse_drive/MouseDrive.cpp
    processors/drive/muff_clipper/MuffClipper.cpp
    processors/drive/muff_clipper/MuffClipperStage.cpp
    processors/drive/mxr_distortion/MXRDistortion.cpp
    processors/drive/neural_utils/ResampledRNN.cpp
    processors/drive/neural_utils/ResampledRNNAccelerated.cpp
    processors/drive/tube_amp/TubeAmp.cpp
    processors/drive/tube_screamer/TubeScreamer.cpp
    processors/drive/waveshaper/SurgeWaveshapers.cpp
    processors/drive/waveshaper/Waveshaper.cpp
    processors/drive/zen_drive/ZenDrive.cpp

    processors/tone/BassCleaner.cpp
    processors/tone/BigMuffTone.cpp
    processors/tone/BlondeTone.cpp
    processors/tone/GraphicEQ.cpp
    processors/tone/HighCut.cpp
    processors/tone/LofiIrs.cpp
    processors/tone/StateVariableFilter.cpp
    processors/tone/TrebleBooster.cpp
    processors/tone/amp_irs/AmpIRs.cpp
    processors/tone/amp_irs/AmpIRsSaveLoad.cpp
    processors/tone/amp_irs/AmpIRsSelector.cpp
    processors/tone/bassman/BassmanTone.cpp
    processors/tone/bassman/BassmanToneStack.cpp
    processors/tone/baxandall/BaxandallEQ.cpp
    processors/tone/baxandall/BaxandallWDF.cpp
    processors/tone/tube_screamer_tone/TubeScreamerTone.cpp
    processors/tone/ladder_filter/LadderFilterProcessor.cpp
    processors/tone/ladder_filter/HighPassOnePole.cpp
    processors/tone/ladder_filter/LowPassOnePole.cpp
    processors/tone/ladder_filter/HighPassLadder.cpp
    processors/tone/ladder_filter/LowPassLadder.cpp
    processors/tone/ladder_filter/LadderParameters.cpp

    processors/modulation/Chorus.cpp
    processors/modulation/Panner.cpp
    processors/modulation/Rotary.cpp
    processors/modulation/Tremolo.cpp
    processors/modulation/FrequencyShifter.cpp
    processors/modulation/Flanger.cpp
    processors/modulation/MIDIModulator.cpp
    processors/modulation/ParamModulator.cpp
    processors/modulation/phaser/Phaser4.cpp
    processors/modulation/phaser/Phaser8.cpp
    processors/modulation/scanner_vibrato/ScannerVibrato.cpp
    processors/modulation/uni_vibe/UniVibe.cpp
    processors/modulation/uni_vibe/UniVibeStage.cpp

    processors/other/Compressor.cpp
    processors/other/Delay.cpp
    processors/other/EnvelopeFilter.cpp
    processors/other/LevelDetective.cpp
    processors/other/Gate.cpp
    processors/other/Octaver.cpp
    processors/other/ShimmerReverb.cpp
    processors/other/SmoothReverb.cpp
    processors/other/cry_baby/CryBaby.cpp
    processors/other/cry_baby/CryBabyNDK.cpp
    processors/other/poly_octave/PolyOctave.cpp
    processors/other/spring_reverb/SpringReverb.cpp
    processors/other/spring_reverb/SpringReverbProcessor.cpp
    processors/other/krusher/Krusher.cpp

    processors/utility/CleanGain.cpp
    processors/utility/FreqBandSplitter.cpp
    processors/utility/InputProcessor.cpp
    processors/utility/Mixer.cpp
    processors/utility/Oscilloscope.cpp
    processors/utility/OutputProcessor.cpp
    processors/utility/StereoMerger.cpp
    processors/utility/StereoSplitter.cpp
    processors/utility/Tuner.cpp

    processors/netlist_helpers/CircuitQuantity.cpp
    processors/netlist_helpers/NetlistViewer.cpp
)

# Jai files
include(${CMAKE_SOURCE_DIR}/modules/cmake/FindJaiCompiler.cmake)
if (NOT(${JAI_COMPILER} STREQUAL "JAI_COMPILER-NOTFOUND"))
    message(STATUS "Configuring Jai compilation!")
    add_subdirectory(jai)
    target_compile_definitions(BYOD PRIVATE BYOD_BUILDING_JAI_MODULES=1)
endif()

# AVX/SSE files for accelerated neural nets and other DSP
make_lib_simd_runtime(dsp_accelerated
    SOURCES
        processors/drive/neural_utils/RNNAccelerated.cpp
        processors/other/poly_octave/PolyOctaveV2FilterBankImpl.cpp
)
foreach(target IN ITEMS dsp_accelerated_sse_or_arm dsp_accelerated_avx)
    target_link_libraries(${target}
        PRIVATE
            math_approx
            config_flags
            juce::juce_recommended_lto_flags
            warning_flags
    )
    target_include_directories(${target}
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../modules/RTNeural
            ${CMAKE_CURRENT_SOURCE_DIR}/../modules/RTNeural/modules/xsimd/include
    )
    target_compile_definitions(${target}
        PRIVATE
            RTNEURAL_USE_XSIMD=1
            _USE_MATH_DEFINES=1
    )
    set_target_properties(${target} PROPERTIES
        POSITION_INDEPENDENT_CODE TRUE
        VISIBILITY_INLINES_HIDDEN TRUE
        C_VISIBILITY_PRESET hidden
        CXX_VISIBILITY_PRESET hidden
    )
endforeach()
target_compile_definitions(dsp_accelerated_sse_or_arm PRIVATE RTNEURAL_DEFAULT_ALIGNMENT=16 RTNEURAL_NAMESPACE=RTNeural_sse_arm)
target_compile_definitions(dsp_accelerated_avx PRIVATE RTNEURAL_DEFAULT_ALIGNMENT=32 RTNEURAL_NAMESPACE=RTNeural_avx)
target_link_libraries(BYOD PRIVATE dsp_accelerated)

# special flags for MSVC
if (MSVC)
    target_compile_options(BYOD PRIVATE /bigobj)
endif ()

file(GLOB_RECURSE module_sources
    # JUCE module format is already unity building
    ${CMAKE_CURRENT_SOURCE_DIR}/../modules/JUCE/modules/juce_*/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../modules/JUCE/modules/juce_*/*.mm
    ${CMAKE_CURRENT_SOURCE_DIR}/../modules/JUCE/modules/chowdsp_utils/modules/*/chowdsp_*/*.cpp

    # files that include "byod_jai_lib.h" have issues with unity builds
    ${CMAKE_CURRENT_SOURCE_DIR}/jai/SharedJaiContext.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/other/krusher/Krusher.cpp

    # already a massive cpp file!
    ${CMAKE_CURRENT_SOURCE_DIR}/processors/ProcessorStore.cpp
)
set_source_files_properties(${module_sources}
    TARGET_DIRECTORY BYOD
    PROPERTIES SKIP_PRECOMPILE_HEADERS TRUE SKIP_UNITY_BUILD_INCLUSION TRUE)
set_target_properties(BYOD PROPERTIES UNITY_BUILD ON UNITY_BUILD_BATCH_SIZE 8)
target_precompile_headers(BYOD PRIVATE pch.h)
