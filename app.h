/*
 * Copyright (c) 2026 Marcel Licence
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
 * @file app.h
 * @author Marcel Licence
 * @date 08.03.2026
 *
 * @brief Declarations of the app
 */


#ifndef APP_H_
#define APP_H_


#include "config.h"

#include <stdint.h>


void App_Setup(void);
void App_Loop(void);

void App_Setup1(void);
void App_Loop1(void);


void App_NoteOn(uint8_t ch, uint8_t note, float vel);
void App_NoteOff(uint8_t ch, uint8_t note);
void App_Sustain(uint8_t unused __attribute__((unused)), float val);
void App_PitchBend(uint8_t unused __attribute__((unused)), float val);
void App_ModWheel(uint8_t unused __attribute__((unused)), float val);
void App_ModSpeed(uint8_t unused __attribute__((unused)), float val);
void App_SetVolume(uint8_t unused, float val);
void App_ModParam(uint8_t param, float val);
void App_ModSwitch(uint8_t param, float val);
#ifdef MAX_DELAY
void App_DelayMode(uint8_t mode, float value);
#endif

#define PARAM_QUICK_DAMP_VALUE  0
#define PARAM_QUICK_DAMP_THRES  1
#define PARAM_MODULATION_DEPTH  2
#define PARAM_TREMOLO_DEPTH  3
#define PARAM_TREMOLO_SHIFT  4
#define PARAM_TREMOLO_SPEED  5
#define PARAM_SOUND_C1  6
#define PARAM_SOUND_C2  7

#define PIANO_SWITCH_SUB_BASE 4


#endif /* APP_H_ */
