/*
 * Copyright (c) 2023 Marcel Licence
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Dieses Programm ist Freie Software: Sie können es unter den Bedingungen
 * der GNU General Public License, wie von der Free Software Foundation,
 * Version 3 der Lizenz oder (nach Ihrer Wahl) jeder neueren
 * veröffentlichten Version, weiter verteilen und/oder modifizieren.
 *
 * Dieses Programm wird in der Hoffnung bereitgestellt, dass es nützlich sein wird, jedoch
 * OHNE JEDE GEWÄHR,; sogar ohne die implizite
 * Gewähr der MARKTFÄHIGKEIT oder EIGNUNG FÜR EINEN BESTIMMTEN ZWECK.
 * Siehe die GNU General Public License für weitere Einzelheiten.
 *
 * Sie sollten eine Kopie der GNU General Public License zusammen mit diesem
 * Programm erhalten haben. Wenn nicht, siehe <https://www.gnu.org/licenses/>.
 */

/**
 * @file z_config.ino
 * @author Marcel Licence
 * @date 02.11.2022
 *
 * @brief This file contains the mapping configuration
 * Put all your project configuration here (no defines etc)
 * This file will be included at the and can access all
 * declarations and type definitions
 *
 * @see ESP32 Arduino DIY Synthesizer Projects - Little startup guide to get your MIDI synth working - https://youtu.be/ZNxGCB-d68g
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#ifdef AUDIO_KIT_BUTTON_ANALOG
audioKitButtonCb audioKitButtonCallback = NULL;
#endif

/*
 * this mapping is used for the edirol pcr-800
 * this should be changed when using another controller
 */
struct midiControllerMapping edirolMapping[] =
{
    /* general MIDI */
    { 0x0, 0x40, "sustain", NULL, App_Sustain, 0},

    /* transport buttons */
    { 0x8, 0x52, "back", NULL, NULL, 0},
#ifdef MIDI_STREAM_PLAYER_ENABLED
    { 0xD, 0x52, "stop", NULL, MidiStreamPlayer_StopPlayback, 0},
#endif
    { 0xe, 0x52, "start", NULL, NULL, 0},
    { 0xe, 0x52, "start", NULL, NULL, 0},
    { 0xa, 0x52, "rec", NULL, NULL, 0},

    /* lower row of buttons */
    { 0x0, 0x51, "B1", NULL, Midi_SetMidiMapByIndex, 0},
    { 0x1, 0x51, "B2", NULL, Midi_SetMidiMapByIndex, 1},
    { 0x2, 0x51, "B3", NULL, Midi_SetMidiMapByIndex, 2},
    { 0x3, 0x51, "B4", NULL, Midi_SetMidiMapByIndex, 3},

    { 0x4, 0x51, "B5", NULL, Midi_SetMidiMapByIndex, 4},
    { 0x5, 0x51, "B6", NULL, Midi_SetMidiMapByIndex, 5},
    { 0x6, 0x51, "B7", NULL, Midi_SetMidiMapByIndex, 6},
    { 0x7, 0x51, "B8", NULL, Midi_SetMidiMapByIndex, 7},

    { 0x1, 0x53, "B9", NULL, Midi_SetMidiMapByIndex, 8},

    /* rotary */
#ifdef REVERB_ENABLED
    { 0x0, 0x12, "R9", NULL, Reverb_SetLevel, 8},
#endif

    /* Central slider */
    { 0x0, 0x13, "H1", NULL, NULL, 0},

    /* MIDI defaults */
    { 0x0, 7, "Volume", NULL, App_SetVolume, 0},
#ifdef REVERB_ENABLED
    { 0x0, 91, "Reverb", NULL, Reverb_SetLevel, 0},
#endif
#ifdef ML_CHORUS_ENABLED
#if (defined ARDUINO_RASPBERRY_PI_PICO) || (defined ARDUINO_GENERIC_RP2040)
    { 0x0, 93, "Chorus", NULL, ChorusQ_SetOutputLevel, 0},
#else
    { 0x0, 93, "Chorus", NULL, Chorus_SetOutputLevel, 0},
#endif
#endif
    { 0x0, 74, "Cuttoff", NULL, NULL, 0},
    { 0x0, 71, "Resonance", NULL, NULL, 0},
};

/*
 * the other mappings can be selected during runtime
 *
 * using the PCR-800 as a MIDI controller following controls are static (map above):
 * - transport buttons
 * - general MIDI
 * - lower row of buttons B1 - B9
 * - rotary R5 - R9
 * - central slider H1
 *
 * the following controls are flexible and changed by the active map:
 * - upper row of buttons A1 - A9
 * - rotary R1 - R4
 * - slider S1 - S9
 */
#ifdef MAX_DELAY
struct midiControllerMapping edirolMapping_delay[] =
{
    /* upper row of buttons */
    { 0x0, 0x50, "A1", NULL, App_DelayMode, 0},
    { 0x1, 0x50, "A2", NULL, App_DelayMode, 1},
    { 0x2, 0x50, "A3", NULL, App_DelayMode, 2},
    { 0x3, 0x50, "A4", NULL, App_DelayMode, 3},

    /* slider */
    { 0x0, 0x11, "S1", NULL, Delay_SetInputLevel, 0},
    { 0x1, 0x11, "S2", NULL, Delay_SetFeedback, 0},
    { 0x2, 0x11, "S3", NULL, Delay_SetShift, 0},
    { 0x3, 0x11, "S4", NULL, Delay_SetLength, 0},

    { 0x4, 0x11, "S5", NULL, NULL, 4},
    { 0x5, 0x11, "S6", NULL, NULL, 5},
    { 0x6, 0x11, "S7", NULL, NULL, 6},
    { 0x7, 0x11, "S8", NULL, NULL, 7},

    { 0x1, 0x12, "S9", NULL, Delay_SetOutputLevel, 8},
};
#endif

#ifdef ML_CHORUS_ENABLED
#if (defined ARDUINO_RASPBERRY_PI_PICO) || (defined ARDUINO_GENERIC_RP2040)
struct midiControllerMapping edirolMapping_chorus[] =
{
    /* slider */
    { 0x0, 0x11, "S1", NULL, ChorusQ_SetInputLevel, 0},
    { 0x1, 0x11, "S2", NULL, ChorusQ_SetDelay, 1},
    { 0x2, 0x11, "S3", NULL, ChorusQ_SetDepth, 2},
    { 0x3, 0x11, "S4", NULL, ChorusQ_SetSpeed, 3},

    { 0x4, 0x11, "S5", NULL, ChorusQ_SetThrough, 4},
    { 0x5, 0x11, "S6", NULL, ChorusQ_SetPhaseShift, 5},
    { 0x6, 0x11, "S7", NULL, NULL, 6},
    { 0x7, 0x11, "S8", NULL, NULL, 7},

    { 0x1, 0x12, "S9", NULL, ChorusQ_SetOutputLevel, 8},
};
#else
struct midiControllerMapping edirolMapping_chorus[] =
{
    /* slider */
    { 0x0, 0x11, "S1", NULL, Chorus_SetInputLevel, 0},
    { 0x1, 0x11, "S2", NULL, Chorus_SetDelay, 1},
    { 0x2, 0x11, "S3", NULL, Chorus_SetDepth, 2},
    { 0x3, 0x11, "S4", NULL, Chorus_SetSpeed, 3},

    { 0x4, 0x11, "S5", NULL, Chorus_SetThrough, 4},
    { 0x5, 0x11, "S6", NULL, Chorus_SetPhaseShift, 5},
    { 0x6, 0x11, "S7", NULL, NULL, 6},
    { 0x7, 0x11, "S8", NULL, NULL, 7},

    { 0x1, 0x12, "S9", NULL, Chorus_SetOutputLevel, 8},
};
#endif
#endif

struct midiControllerMapping edirolMapping_tremolo[] =
{
    /* slider */
    { 0x0, 0x11, "S1", NULL, App_ModParam, PARAM_TREMOLO_DEPTH},
    { 0x1, 0x11, "S2", NULL, App_ModParam, PARAM_TREMOLO_SHIFT},
    { 0x2, 0x11, "S3", NULL, App_ModParam, PARAM_TREMOLO_SPEED},
    { 0x3, 0x11, "S4", NULL, NULL, 3},

    { 0x4, 0x11, "S5", NULL, NULL, 4},
    { 0x5, 0x11, "S6", NULL, NULL, 5},
    { 0x6, 0x11, "S7", NULL, NULL, 6},
    { 0x7, 0x11, "S8", NULL, NULL, 7},

    { 0x1, 0x12, "S9", NULL, NULL, 8},
};

struct midiControllerMapping edirolMapping_piano[] =
{
    /* slider */
    { 0x0, 0x11, "S1", NULL, App_ModParam, PARAM_QUICK_DAMP_VALUE},
    { 0x1, 0x11, "S2", NULL, App_ModParam, PARAM_QUICK_DAMP_THRES},
    { 0x2, 0x11, "S3", NULL, App_ModParam, PARAM_MODULATION_DEPTH},
    { 0x3, 0x11, "S4", NULL, NULL, 3},

    { 0x4, 0x11, "S5", NULL, App_ModParam, PARAM_SOUND_C1},
    { 0x5, 0x11, "S6", NULL, App_ModParam, PARAM_SOUND_C2},
    { 0x6, 0x11, "S7", NULL, NULL, 6},
    { 0x7, 0x11, "S8", NULL, NULL, 7},

    { 0x1, 0x12, "S9", NULL, NULL, 8},

    /* upper row of buttons */
    { 0x0, 0x50, "A1", NULL, App_ModSwitch, 0},
    { 0x1, 0x50, "A2", NULL, App_ModSwitch, 1},
    { 0x2, 0x50, "A3", NULL, App_ModSwitch, 2},
    { 0x3, 0x50, "A4", NULL, App_ModSwitch, 3},

    { 0x4, 0x50, "A5", NULL, App_ModSwitch, PIANO_SWITCH_SUB_BASE},
    { 0x5, 0x50, "A6", NULL, App_ModSwitch, 5},
    { 0x6, 0x50, "A7", NULL, App_ModSwitch, 6},
    { 0x7, 0x50, "A8", NULL, App_ModSwitch, 7},

    { 0x0, 0x53, "A9", NULL, App_ModSwitch, 8},
};

#ifdef MIDI_STREAM_PLAYER_ENABLED
struct midiControllerMapping edirolMapping_midi_stream_player[] =
{
    /* rotary */
    { 0x0, 0x10, "R1", NULL, MidiStreamPlayerTempo, 0},

    /* slider */
    { 0x0, 0x11, "S1", NULL, NULL, 0},
    { 0x1, 0x11, "S2", NULL, NULL, 1},
    { 0x2, 0x11, "S3", NULL, NULL, 2},
    { 0x3, 0x11, "S4", NULL, NULL, 3},

    { 0x4, 0x11, "S5", NULL, NULL, 4},
    { 0x5, 0x11, "S6", NULL, NULL, 5},
    { 0x6, 0x11, "S7", NULL, NULL, 6},
    { 0x7, 0x11, "S8", NULL, NULL, 7},

    { 0x1, 0x12, "S9", NULL, NULL, 8},

    /* upper row of buttons */
    { 0x0, 0x50, "A1", NULL, NULL, 0},
    { 0x1, 0x50, "A2", NULL, NULL, 1},
    { 0x2, 0x50, "A3", NULL, NULL, 2},
    { 0x3, 0x50, "A4", NULL, NULL, 3},

    { 0x4, 0x50, "A5", NULL, MidiStreamPlayerCtrl, MIDI_STREAM_PLAYER_CTRL_PAUSE},
    { 0x5, 0x50, "A6", NULL, MidiStreamPlayerCtrl, MIDI_STREAM_PLAYER_CTRL_STOP},
    { 0x6, 0x50, "A7", NULL, MidiStreamPlayerCtrl, MIDI_STREAM_PLAYER_CTRL_PLAY},
    { 0x7, 0x50, "A8", NULL, MidiStreamPlayerCtrl, MIDI_STREAM_PLAYER_CTRL_SKIP},

    { 0x0, 0x53, "A9", NULL, NULL, 8},
};
#endif

struct midiControllerMapping edirolMapping_rest[] =
{
    { 0x4, 0x50, "A5", NULL, NULL, 0},
    { 0x5, 0x50, "A6", NULL, NULL, 1},
    { 0x6, 0x50, "A7", NULL, NULL, 2},
    { 0x7, 0x50, "A8", NULL, NULL, 0},

    { 0x0, 0x53, "A9", NULL, NULL, 0},

#if 0
    /* lower row of buttons */
    { 0x0, 0x51, "B1", NULL, App_ModSwitch, 0},
    { 0x1, 0x51, "B2", NULL, App_ModSwitch, 1},
    { 0x2, 0x51, "B3", NULL, App_ModSwitch, 2},
    { 0x3, 0x51, "B4", NULL, App_ModSwitch, 3},

    { 0x4, 0x51, "B5", NULL, App_ModSwitch, PIANO_SWITCH_SUB_BASE},
    { 0x5, 0x51, "B6", NULL, App_ModSwitch, 5},
    { 0x6, 0x51, "B7", NULL, App_ModSwitch, 6},
    { 0x7, 0x51, "B8", NULL, App_ModSwitch, 7},

    { 0x1, 0x53, "B9", NULL, App_ModSwitch, 0},
#endif

    /* pedal */
    { 0x0, 0x0b, "VolumePedal", NULL, NULL, 0},

    /* rotary */
#ifdef MIDI_STREAM_PLAYER_ENABLED
    { 0x0, 0x10, "R1", NULL, MidiStreamPlayerTempo, 0},
#else
    { 0x0, 0x10, "R1", NULL, App_ModSpeed, 0},
#endif
    { 0x1, 0x10, "R2", NULL, App_ModParam, PARAM_QUICK_DAMP_VALUE},
    { 0x2, 0x10, "R3", NULL, App_ModParam, PARAM_QUICK_DAMP_THRES},
    { 0x3, 0x10, "R4", NULL, App_ModParam, PARAM_MODULATION_DEPTH},

    { 0x4, 0x10, "R5", NULL, App_ModParam, PARAM_TREMOLO_DEPTH},
    { 0x5, 0x10, "R6", NULL, App_ModParam, PARAM_TREMOLO_SHIFT},
    { 0x6, 0x10, "R7", NULL, App_ModParam, PARAM_SOUND_C2},
#ifdef OVERDRIVE_ENABLED
    { 0x7, 0x10, "R8", NULL, Overdrive_SetDrive, 7},
#endif
#ifdef REVERB_ENABLED
    { 0x0, 0x12, "R9", NULL, Reverb_SetLevel, 8},
#endif
};

#define ARRAY_LENGTH(array) (sizeof(array)/sizeof(array[0]))

/*
 * this lookup table shall contain all selectable MIDI maps
 */
struct midiMapLookUpEntry midiMapLookUp[] =
{
    {"Piano", edirolMapping_piano, ARRAY_LENGTH(edirolMapping_piano)},
#ifdef MAX_DELAY
    {"Delay", edirolMapping_delay, ARRAY_LENGTH(edirolMapping_delay)},
#endif
#ifdef ML_CHORUS_ENABLED
    {"Chorus", edirolMapping_chorus, ARRAY_LENGTH(edirolMapping_chorus)},
#endif
    {"Tremolo", edirolMapping_tremolo, ARRAY_LENGTH(edirolMapping_tremolo)},
#ifdef MIDI_STREAM_PLAYER_ENABLED
    {"MIDI Player", edirolMapping_midi_stream_player, ARRAY_LENGTH(edirolMapping_midi_stream_player)},
#endif
    {"Rest", edirolMapping_rest, ARRAY_LENGTH(edirolMapping_rest)},
};

#define MIDI_CONTROL_MAP_INIT_ID    0

int midiMapLookUpCnt = ARRAY_LENGTH(midiMapLookUp);

struct midiMapping_s midiMapping =
{
    NULL,
    App_NoteOn,
    App_NoteOff,
    App_PitchBend,
    App_ModWheel,
    NULL, /* assign program change callback here! */
    NULL,
    NULL,
    edirolMapping,
    ARRAY_LENGTH(edirolMapping),
    midiMapLookUp[MIDI_CONTROL_MAP_INIT_ID].controlMap,
    midiMapLookUp[MIDI_CONTROL_MAP_INIT_ID].controlMapSize,
};

#ifdef MIDI_VIA_USB_ENABLED
struct usbMidiMappingEntry_s usbMidiMappingEntries[] =
{
    {
        NULL,
        App_UsbMidiShortMsgReceived,
        NULL,
        NULL,
        0xFF,
    },
};

struct usbMidiMapping_s usbMidiMapping =
{
    NULL,
    NULL,
    usbMidiMappingEntries,
    sizeof(usbMidiMappingEntries) / sizeof(usbMidiMappingEntries[0]),
};
#endif /* MIDI_VIA_USB_ENABLED */
