/*
 * Copyright (c) 2022 Marcel Licence
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
 * @file ml_epiano_example.ino
 * @author Marcel Licence
 * @date 02.11.2022
 *
 * @brief   This is the main project file to test the ML_SynthLibrary (epiano module)
 *          It should be compatible with ESP32 (more is coming soon might be compatible with Teensy and DaisySeed)
 *
 * @see     https://youtu.be/m12w1Xtm5Ts
 */


#ifdef __CDT_PARSER__
#include <cdt.h>
#endif


#include "config.h"


#include <Arduino.h>

/*
 * Library can be found on https://github.com/marcel-licence/ML_SynthTools
 */
#include <ml_epiano.h>
#include <ml_tremolo.h>

#ifdef USE_DAISY_SP /* not supported yet */
#include "reverb_sc.h"
#endif

#if 0 /* coming in future? */
#include <ml_overdrive.h>
#endif

#ifdef REVERB_ENABLED
#include <ml_reverb.h>
#endif
#ifdef MAX_DELAY
#include <ml_delay.h>
#endif
#ifdef OLED_OSC_DISP_ENABLED
#include <ml_scope.h>
#endif

#if (defined ARDUINO_GENERIC_F407VGTX) || (defined ARDUINO_DISCO_F407VG)
#include <Wire.h> /* todo remove, just for scanning */
#endif


ML_EPiano myRhodes;
ML_EPiano *rhodes = &myRhodes;

ML_Tremolo myTremolo(SAMPLE_RATE);
ML_Tremolo *tremolo = &myTremolo;


#ifdef USE_DAISY_SP
ReverbSc myVerb;
ReverbSc *verb = &myVerb;

Overdrive myDrive;
Overdrive *drive = &myDrive;
float driveInv = 1.0f;
#endif


char shortName[] = "ML_Piano";


void setup()
{
    /*
     * this code runs once
     */
#ifdef MIDI_USB_ENABLED
    Midi_Usb_Setup();
#endif

#ifdef BLINK_LED_PIN
    Blink_Setup();
    Blink_Fast(1);
#endif

#ifdef ARDUINO_DAISY_SEED
    DaisySeed_Setup();
#endif

    delay(500);

#ifdef SWAP_SERIAL
    /* only one hw serial use this for ESP */
    Serial.begin(115200);
    delay(500);
#else
    Serial.begin(SERIAL_BAUDRATE);
#endif

    Serial.println();


    Serial.printf("Loading data\n");

    Serial.printf("Firmware started successfully\n");


#ifdef ESP8266
    Midi_Setup();
#endif

    Serial.printf("Initialize Audio Interface\n");
    Audio_Setup();

#ifdef TEENSYDUINO
    Teensy_Setup();
#else

#ifdef ARDUINO_SEEED_XIAO_M0
    pinMode(7, INPUT_PULLUP);
    Midi_Setup();
    pinMode(LED_BUILTIN, OUTPUT);
#else

#ifndef ESP8266 /* otherwise it will break audio output */
    Midi_Setup();
#endif
#endif

#endif

#ifdef USE_DAISY_SP
    float *verbBuf = (float *)malloc(DSY_REVERBSC_MAX_SIZE * sizeof(float));
    verb->SetBuffer(verbBuf);

    Serial.println("verb->Init");
    verb->Init(44100);

    Serial.println("verb->SetFeedback");
    verb->SetFeedback(0.75f);

    Serial.println("verb->SetLpFreq");
    verb->SetLpFreq(18000.0f);
#endif

#ifdef REVERB_ENABLED
    /*
     * Initialize reverb
     * The buffer shall be static to ensure that
     * the memory will be exclusive available for the reverb module
     */
    //static float revBuffer[REV_BUFF_SIZE];
    static float *revBuffer = (float *)malloc(sizeof(float) * REV_BUFF_SIZE);
    Reverb_Setup(revBuffer);
#endif

#ifdef MAX_DELAY
    /*
     * Prepare a buffer which can be used for the delay
     */
    if (psramInit())
    {
        //static int16_t delBuffer1[MAX_DELAY];
        //static int16_t delBuffer2[MAX_DELAY];
        static int16_t *delBuffer1 = (int16_t *)ps_malloc(sizeof(int16_t) * MAX_DELAY);
        static int16_t *delBuffer2 = (int16_t *)ps_malloc(sizeof(int16_t) * MAX_DELAY);
        Delay_Init2(delBuffer1, delBuffer2, MAX_DELAY);

        Delay_SetInputLevel(0, 0.75f);
        Delay_SetOutputLevel(0, 0.75f);
        Delay_SetFeedback(0, 0.25f);
        Delay_SetLength(0, 0.5f);
    }
    else
    {
        Serial.printf("PSRAM required for Delay effect, software will crash!\n");
    }
#endif

#ifdef MIDI_BLE_ENABLED
    midi_ble_setup();
#endif

#ifdef USB_HOST_ENABLED
    Usb_Host_Midi_setup();
#endif

#ifdef ESP32
    Serial.printf("ESP.getFreeHeap() %d\n", ESP.getFreeHeap());
    Serial.printf("ESP.getMinFreeHeap() %d\n", ESP.getMinFreeHeap());
    Serial.printf("ESP.getHeapSize() %d\n", ESP.getHeapSize());
    Serial.printf("ESP.getMaxAllocHeap() %d\n", ESP.getMaxAllocHeap());

    /* PSRAM will be fully used by the looper */
    Serial.printf("Total PSRAM: %d\n", ESP.getPsramSize());
    Serial.printf("Free PSRAM: %d\n", ESP.getFreePsram());
#endif

    Serial.printf("Firmware started successfully\n");

#if (defined OLED_OSC_DISP_ENABLED) && (defined TEENSYDUINO)
    // Setup for Master mode, pins 18/19, external pullups, 400kHz, 10ms default timeout
    //Wire.begin(I2C_MASTER, 0x00, I2C_PINS_18_19, I2C_PULLUP_EXT, 1000000);

    //Wire.setClock(1000000) ;
    ScopeOled_Setup();
#endif

#ifdef TEENSYDUINO
    memtest();
#endif

#ifdef USB_HOST_ENABLED
    Usb_Host_Midi_setup();
#endif

#ifdef NOTE_ON_AFTER_SETUP
#ifdef USE_ML_SYNTH_PRO
    OrganPro_NoteOn(0, 60, 127);
    OrganPro_SetLeslCtrl(127);
#ifndef SOC_CPU_HAS_FPU
    Serial.printf("Synth might not work because CPU does not have a FPU (floating point unit)");
#endif
#else
    Organ_NoteOn(0, 60, 127);
    Organ_SetLeslCtrl(127);
    Organ_PercussionSet(CTRL_ROTARY_ACTIVE);
    Organ_PercussionSet(CTRL_ROTARY_ACTIVE);
#endif
#endif

#ifdef USE_DAISY_SP
    drive->Init();
    drive->SetDrive(0.28);
#endif

#ifdef MIDI_STREAM_PLAYER_ENABLED
    MidiStreamPlayer_Init();

    char midiFile[] = "/demo.mid";
    MidiStreamPlayer_PlayMidiFile_fromLittleFS(midiFile, 1);
#endif

#if (defined MIDI_VIA_USB_ENABLED) || (defined OLED_OSC_DISP_ENABLED)
#ifdef ESP32
    Core0TaskInit();
#else
    //#error only supported by ESP32 platform
#endif
#endif
}

#ifdef ESP32
/*
 * Core 0
 */
/* this is used to add a task to core 0 */
TaskHandle_t Core0TaskHnd;

inline
void Core0TaskInit()
{
    /* we need a second task for the terminal output */
    xTaskCreatePinnedToCore(Core0Task, "CoreTask0", 8000, NULL, 999, &Core0TaskHnd, 0);
}

void Core0TaskSetup()
{
    /*
     * init your stuff for core0 here
     */

#ifdef OLED_OSC_DISP_ENABLED
    ScopeOled_Setup();
#endif
#ifdef MIDI_VIA_USB_ENABLED
    UsbMidi_Setup();
#endif
}

void Core0TaskLoop()
{
    /*
     * put your loop stuff for core0 here
     */
#ifdef MIDI_VIA_USB_ENABLED
    UsbMidi_Loop();
#endif

#ifdef OLED_OSC_DISP_ENABLED
    ScopeOled_Process();
#endif
}

void Core0Task(void *parameter)
{
    Core0TaskSetup();

    while (true)
    {
        Core0TaskLoop();

        /* this seems necessary to trigger the watchdog */
        delay(1);
        yield();
    }
}
#endif /* ESP32 */

void loop_1Hz()
{
#ifdef CYCLE_MODULE_ENABLED
    CyclePrint();
#endif
#ifdef BLINK_LED_PIN
    Blink_Process();
#endif
}

void loop()
{
    static int loop_cnt_1hz = 0; /*!< counter to allow 1Hz loop cycle */

#ifdef SAMPLE_BUFFER_SIZE
    loop_cnt_1hz += SAMPLE_BUFFER_SIZE;
#else
    loop_cnt_1hz += 1; /* in case only one sample will be processed per loop cycle */
#endif
    if (loop_cnt_1hz >= SAMPLE_RATE)
    {
        loop_cnt_1hz -= SAMPLE_RATE;
        loop_1Hz();
    }

    /*
     * MIDI processing
     */
    Midi_Process();
#ifdef MIDI_VIA_USB_ENABLED
    UsbMidi_ProcessSync();
#endif
#ifdef MIDI_STREAM_PLAYER_ENABLED
    MidiStreamPlayer_Tick(SAMPLE_BUFFER_SIZE);
#endif

#ifdef MIDI_BLE_ENABLED
    midi_ble_loop();
#endif

#ifdef USB_HOST_ENABLED
    Usb_Host_Midi_loop();
#endif

#ifdef MIDI_USB_ENABLED
    Midi_Usb_Loop();
#endif

    /*
     * And finally the audio stuff
     */
#if 1
#if  (defined ARDUINO_RASPBERRY_PI_PICO) || (defined ARDUINO_GENERIC_RP2040)

    int16_t mono[SAMPLE_BUFFER_SIZE];
    rhodes->Process(mono, SAMPLE_BUFFER_SIZE);
    Audio_Output(mono, mono);

#elif (defined ESP8266) || (defined ARDUINO_SEEED_XIAO_M0)
#error Configuration is not supported
#else

    float mono[SAMPLE_BUFFER_SIZE], left[SAMPLE_BUFFER_SIZE], right[SAMPLE_BUFFER_SIZE];

    memset(left, 0, sizeof(left));
    memset(right, 0, sizeof(right));

#ifdef AUDIO_PASS_THROUGH
    Audio_Input(left, right);
#endif

    rhodes->Process(mono, SAMPLE_BUFFER_SIZE);

    /* reduce gain to avoid clipping */
    for (int n = 0; n < SAMPLE_BUFFER_SIZE; n++)
    {
        mono[n] *= 0.5f;
    }

#ifdef AUDIO_PASS_THROUGH
    for (int n = 0; n < SAMPLE_BUFFER_SIZE; n++)
    {
        mono[n] += (left[n] + right[n]) * 0.5f * 32.0f;
    }
#endif

#if 0 /* maybe coming soon? */
    Overdrive_Process_fl(mono, SAMPLE_BUFFER_SIZE);
#endif

#ifdef INPUT_TO_MIX
    Audio_Input(left, right);
    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        mono[i] += (left[i] * 0.5f + right[i] * 0.5f) * 32.0f;
    }
#endif

#ifdef USE_DAISY_SP
    drive->Process(mono, mono, SAMPLE_BUFFER_SIZE);
#endif

#ifdef REVERB_ENABLED
    Reverb_Process(mono, SAMPLE_BUFFER_SIZE);
#endif

#ifdef MAX_DELAY
    /*
     * post process delay
     */
    Delay_Process_Buff(mono, left, right, SAMPLE_BUFFER_SIZE);
#endif

#ifdef USE_DAISY_SP
    verb->Process(mono, mono, left, right, SAMPLE_BUFFER_SIZE);

    /*
     * mix mono back to stereo mix (bypass otherwise completely missing
     */
    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        left[i] = mono[i] + left[i];
        right[i] = mono[i] + right[i];
    }
#else
    /* mono to left and right channel */
    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        left[i] += mono[i];
        right[i] += mono[i];
    }
#endif

    tremolo->process(left, right, SAMPLE_BUFFER_SIZE);


    /* ~21dB margin required to allow playing all notes at the same time -> overdrive would help */
#ifdef USE_DAISY_SP
    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        left[i] *= driveInv;
        right[i] *= driveInv;
    }
#endif

    /*
     * Output the audio
     */
    Audio_Output(left, right);

#ifdef OLED_OSC_DISP_ENABLED
    ScopeOled_AddSamples(left, right, SAMPLE_BUFFER_SIZE);
#ifdef TEENSYDUINO
    static uint8_t x = 0;
    x++;
    if (x == 0)
    {
        ScopeOled_Process();
    }
#endif
#endif
#endif
#else
    int32_t mono[SAMPLE_BUFFER_SIZE];
    Organ_Process_Buf(mono, SAMPLE_BUFFER_SIZE);
#ifdef REVERB_ENABLED
    float mono_f[SAMPLE_BUFFER_SIZE];
    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        mono_f[i] = mono[i];
    }
    Reverb_Process(mono_f, SAMPLE_BUFFER_SIZE); /* post reverb */
    for (int i = 0; i < SAMPLE_BUFFER_SIZE; i++)
    {
        mono[i] = mono_f[i];
    }
#endif
    Audio_OutputMono(mono);
#endif
}

/*
 * MIDI via USB Host Module
 */
#ifdef MIDI_VIA_USB_ENABLED
void App_UsbMidiShortMsgReceived(uint8_t *msg)
{
#ifdef MIDI_TX2_PIN
    Midi_SendShortMessage(msg);
#endif
    Midi_HandleShortMsg(msg, 8);
}
#endif

/*
 * MIDI callbacks
 */
bool subBase = false;

void App_NoteOn(uint8_t ch, uint8_t note, float vel)
{
    rhodes->NoteOn(ch, note, vel);
    if (subBase && note <= 60)
    {
        rhodes->NoteOn(ch, note + 12, vel);
    }
}

void App_NoteOff(uint8_t ch, uint8_t note)
{
    rhodes->NoteOff(ch, note);
    if (subBase && note <= 60)
    {
        rhodes->NoteOff(ch, note + 12);
    }
}

void App_Sustain(uint8_t userdata, float val)
{
    rhodes->Sustain(val);
}

void App_PitchBend(uint8_t userdata, float val)
{
    rhodes->PitchBend(val);
}

void App_ModWheel(uint8_t userdata, float val)
{
    Serial.printf("tremolo depth: %0.3f\n", val);
    tremolo->setDepth(val);
}

void App_ModSpeed(uint8_t userdata, float val)
{
    Serial.printf("tremolo speed: %0.3f\n", val);
    tremolo->setSpeed(0.5 + val * 15);
}

#define PARAM_QUICK_DAMP_VALUE  0
#define PARAM_QUICK_DAMP_THRES  1
#define PARAM_MODULATION_DEPTH  2
#define PARAM_TREMOLO_DEPTH  3
#define PARAM_TREMOLO_SHIFT  4
#define PARAM_SOUND_C1  5
#define PARAM_SOUND_C2  6

void App_ModParam(uint8_t param, float val)
{
    switch (param)
    {
    case PARAM_QUICK_DAMP_VALUE:
        rhodes->SetQuickDamping(val);
        break;
    case PARAM_QUICK_DAMP_THRES:
        rhodes->SetQuickDampingThr(val);
        break;
    case PARAM_MODULATION_DEPTH:
        rhodes->SetModulationDepth(val);
        break;
    case PARAM_TREMOLO_SHIFT:
        tremolo->setPhaseShift(val);
        break;
    case PARAM_TREMOLO_DEPTH:
        tremolo->setDepth(val);
        break;
    case PARAM_SOUND_C1:
        rhodes->SetCurve(val);
        break;
    case PARAM_SOUND_C2:
        rhodes->SetCurve2(val);
        break;
    }
}

#define PIANO_SWITCH_SUB_BASE 4

void App_ModSwitch(uint8_t param, float val)
{
    if (val > 0)
    {
        switch (param)
        {
        case 0:
            rhodes->SetCenterTuneA(220);
            break;
        case 1:
            rhodes->SetCenterTuneA(440);
            break;
        case 2:
            rhodes->SetCenterTuneA(444);
            break;
        case 3:
            rhodes->SetCenterTuneA(880);
            break;
        case PIANO_SWITCH_SUB_BASE:
            subBase = !subBase;
            break;
        case 5:
            rhodes->CalcCurvePreset1();
            break;
        case 6:
            rhodes->SetCurve(0);
            rhodes->SetCurve2(0);
            break;
        case 7:
            rhodes->SetCurve(0);
            rhodes->SetCurve2(0.5f);
            break;
        }
    }
}

inline void Overdrive_SetDrive(uint8_t unused __attribute__((unused)), float value)
{
#ifdef USE_DAISY_SP
    value *= 0.4;
    value += 0.30;
    drive->SetDrive(value);
    driveInv = 1.3 - value * 2;
#endif
}

inline void Delay_SetOutputLevelInt(uint8_t unused __attribute__((unused)), uint8_t value)
{
#ifdef MAX_DELAY
    float val = value;
    val /= 127.0f;
    Delay_SetOutputLevel(unused, val);
#endif
}

inline void Delay_SetFeedbackInt(uint8_t unused __attribute__((unused)), uint8_t value)
{
#ifdef MAX_DELAY
    float val = value;
    val /= 127.0f;
    Delay_SetFeedback(unused, val);
#endif
}

#ifdef MAX_DELAY
void App_DelayMode(uint8_t mode, float value)
{
    if (value > 0.0f)
    {
        switch (mode)
        {
        case 0:
            Delay_SetShift(0, 0.25);
            break;
        case 1:
            Delay_SetShift(0, 0.5);
            break;
        case 2:
            Delay_SetShift(0, 2.0f / 3.0f);
            break;
        case 3:
            Delay_SetShift(0, 0.75);
            break;
        }
    }
}
#endif

#if defined(I2C_SCL) && defined (I2C_SDA)
void  ScanI2C(void)
{
#ifdef ARDUINO_GENERIC_F407VGTX
    Wire.setSDA(I2C_SDA);
    Wire.setSCL(I2C_SCL);
    Wire.begin();//I2C_SDA, I2C_SCL);
#else
    Wire.begin();
#endif

    byte address;
    int nDevices;

    Serial.println("Scanning...");

    nDevices = 0;
    for (address = 1; address < 127; address++)
    {
        byte r_error;
        // The i2c_scanner uses the return value of
        // the Write.endTransmisstion to see if
        // a device did acknowledge to the address.
        Wire.beginTransmission(address);
        r_error = Wire.endTransmission();

        if (r_error == 0)
        {
            Serial.print("I2C device found at address 0x");
            if (address < 16)
            {
                Serial.print("0");
            }
            Serial.print(address, HEX);
            Serial.println("  !");

            nDevices++;
        }
        else if (r_error == 4)
        {
            Serial.print("Unknown error at address 0x");
            if (address < 16)
            {
                Serial.print("0");
            }
            Serial.println(address, HEX);
        }
    }
    if (nDevices == 0)
    {
        Serial.println("No I2C devices found\n");
    }
    else
    {
        Serial.println("done\n");
    }
}
#endif /* (defined ARDUINO_GENERIC_F407VGTX) || (defined ARDUINO_DISCO_F407VG) */

