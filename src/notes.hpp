#pragma once

// Some values that can be useful

#define NO_ALL_SCALES_AND_ARPS 28 // All scales & arps

// Basic semi-tone and tone
const float SEMI_TONE = 1 / 12.f;
const float TONE = 1 / 6.f;

// Base Notes in Voltage (C4 or 0.0 V is considered the base note)

const float C4_NOTE = 0.f;
const float Db4_NOTE = 1 / 12.f;
const float D4_NOTE = 2 / 12.f;
const float Eb4_NOTE = 3 / 12.f;
const float E4_NOTE = 4 / 12.f;
const float F4_NOTE = 5 / 12.f;
const float Gb4_NOTE = 6 / 12.f;
const float G4_NOTE = 7 / 12.f;
const float Ab4_NOTE = 8 / 12.f;
const float A4_NOTE = 9 / 12.f;
const float Bb4_NOTE = 10 / 12.f;
const float B4_NOTE = 11 / 12.f;

// Intervals
const float MINOR_SECOND = 1 / 12.f;
const float MAJOR_SECOND = 2 / 12.f;
const float MINOR_THIRD = 3 / 12.f;
const float MAJOR_THIRD = 4 / 12.f;
const float PERFECT_FOURTH = 5 / 12.f;
const float AUG_FOURTH = 6 / 12.f; // Diminished Fifth, tritone
const float DIM_FIFTH = 6 / 12.f;  // Augmented fourth, tritone
const float PERFECT_FIFTH = 7 / 12.f;
const float AUG_FIFTH = 8 / 12.f;
const float MINOR_SIXTH = 8 / 12.f;
const float MAJOR_SIXTH = 9 / 12.f;
const float MINOR_SEVENTH = 10 / 12.f;
const float MAJOR_SEVENTH = 11 / 12.f;
const float MAJOR_NINETH = 14 / 12.f;

// Data Structure for all scales. A matrix of notes, -1.0 meaning note does not participate. Last value per row is # of notes in that scale
// so 1st index is the scale/arp (menu item #), 2nd index is the note (C..B menut item #), 3rd index is the note value in C4--B4, or -1.0 or # of notes as 13th value
const float ALL_SCALES_AND_ARPS[NO_ALL_SCALES_AND_ARPS][12][13] = {
	// Chromatic 12-note scales
	{
		// C
		{0.f, SEMI_TONE, 2 * SEMI_TONE, 3 * SEMI_TONE,
		 4 * SEMI_TONE, 5 * SEMI_TONE, 6 * SEMI_TONE,
		 7 * SEMI_TONE, 8 * SEMI_TONE, 9 * SEMI_TONE,
		 10 * SEMI_TONE, 11 * SEMI_TONE, 12.f},
		// C#/Db
		{SEMI_TONE, 2 * SEMI_TONE, 3 * SEMI_TONE,
		 4 * SEMI_TONE, 5 * SEMI_TONE, 6 * SEMI_TONE,
		 7 * SEMI_TONE, 8 * SEMI_TONE, 9 * SEMI_TONE,
		 10 * SEMI_TONE, 11 * SEMI_TONE, 12 * SEMI_TONE, 12.f},
		// D
		{2 * SEMI_TONE, 3 * SEMI_TONE, 4 * SEMI_TONE,
		 5 * SEMI_TONE, 6 * SEMI_TONE, 7 * SEMI_TONE,
		 8 * SEMI_TONE, 9 * SEMI_TONE, 10 * SEMI_TONE,
		 11 * SEMI_TONE, 12 * SEMI_TONE, 13 * SEMI_TONE, 12.f},
		// D#/Eb
		{3 * SEMI_TONE, 4 * SEMI_TONE, 5 * SEMI_TONE,
		 6 * SEMI_TONE, 7 * SEMI_TONE, 8 * SEMI_TONE,
		 9 * SEMI_TONE, 10 * SEMI_TONE, 11 * SEMI_TONE,
		 12 * SEMI_TONE, 13 * SEMI_TONE, 14 * SEMI_TONE, 12.f},
		// E
		{4 * SEMI_TONE, 5 * SEMI_TONE, 6 * SEMI_TONE,
		 7 * SEMI_TONE, 8 * SEMI_TONE, 9 * SEMI_TONE,
		 10 * SEMI_TONE, 11 * SEMI_TONE, 12 * SEMI_TONE,
		 13 * SEMI_TONE, 14 * SEMI_TONE, 15 * SEMI_TONE, 12.f},
		// F
		{5 * SEMI_TONE, 6 * SEMI_TONE, 7 * SEMI_TONE,
		 8 * SEMI_TONE, 9 * SEMI_TONE, 10 * SEMI_TONE,
		 11 * SEMI_TONE, 12 * SEMI_TONE, 13 * SEMI_TONE,
		 14 * SEMI_TONE, 15 * SEMI_TONE, 16 * SEMI_TONE, 12.f},
		// F#/Gb
		{6 * SEMI_TONE, 7 * SEMI_TONE, 8 * SEMI_TONE,
		 9 * SEMI_TONE, 10 * SEMI_TONE, 11 * SEMI_TONE,
		 12 * SEMI_TONE, 13 * SEMI_TONE, 14 * SEMI_TONE,
		 15 * SEMI_TONE, 16 * SEMI_TONE, 17 * SEMI_TONE, 12.f},
		// G
		{7 * SEMI_TONE, 8 * SEMI_TONE, 9 * SEMI_TONE,
		 10 * SEMI_TONE, 11 * SEMI_TONE, 12 * SEMI_TONE,
		 13 * SEMI_TONE, 14 * SEMI_TONE, 15 * SEMI_TONE,
		 16 * SEMI_TONE, 17 * SEMI_TONE, 18 * SEMI_TONE, 12.f},
		// G#/Ab
		{8 * SEMI_TONE, 9 * SEMI_TONE, 10 * SEMI_TONE,
		 11 * SEMI_TONE, 12 * SEMI_TONE, 13 * SEMI_TONE,
		 14 * SEMI_TONE, 15 * SEMI_TONE, 16 * SEMI_TONE,
		 17 * SEMI_TONE, 18 * SEMI_TONE, 19 * SEMI_TONE, 12.f},
		// A
		{9 * SEMI_TONE, 10 * SEMI_TONE, 11 * SEMI_TONE,
		 12 * SEMI_TONE, 13 * SEMI_TONE, 14 * SEMI_TONE,
		 15 * SEMI_TONE, 16 * SEMI_TONE, 17 * SEMI_TONE,
		 18 * SEMI_TONE, 19 * SEMI_TONE, 20 * SEMI_TONE, 12.f},
		// A#/Bb
		{10 * SEMI_TONE, 11 * SEMI_TONE, 12 * SEMI_TONE,
		 13 * SEMI_TONE, 14 * SEMI_TONE, 15 * SEMI_TONE,
		 16 * SEMI_TONE, 17 * SEMI_TONE, 18 * SEMI_TONE,
		 19 * SEMI_TONE, 20 * SEMI_TONE, 21 * SEMI_TONE, 12.f},
		// B
		{11 * SEMI_TONE, 12 * SEMI_TONE, 13 * SEMI_TONE,
		 14 * SEMI_TONE, 15 * SEMI_TONE, 16 * SEMI_TONE,
		 17 * SEMI_TONE, 18 * SEMI_TONE, 19 * SEMI_TONE,
		 20 * SEMI_TONE, 21 * SEMI_TONE, 22 * SEMI_TONE, 12.f}},
	// All the modes
	// Ionian / Major
	{
		// C
		{C4_NOTE, C4_NOTE + MAJOR_SECOND, C4_NOTE + MAJOR_THIRD, C4_NOTE + PERFECT_FOURTH, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MAJOR_SIXTH, C4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MAJOR_SECOND, Db4_NOTE + MAJOR_THIRD, Db4_NOTE + PERFECT_FOURTH, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MAJOR_SIXTH, Db4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// D
		{D4_NOTE, D4_NOTE + MAJOR_SECOND, D4_NOTE + MAJOR_THIRD, D4_NOTE + PERFECT_FOURTH, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MAJOR_SIXTH, D4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MAJOR_SECOND, Eb4_NOTE + MAJOR_THIRD, Eb4_NOTE + PERFECT_FOURTH, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MAJOR_SIXTH, Eb4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// E
		{E4_NOTE, E4_NOTE + MAJOR_SECOND, E4_NOTE + MAJOR_THIRD, E4_NOTE + PERFECT_FOURTH, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MAJOR_SIXTH, E4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// F
		{F4_NOTE, F4_NOTE + MAJOR_SECOND, F4_NOTE + MAJOR_THIRD, F4_NOTE + PERFECT_FOURTH, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MAJOR_SIXTH, F4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MAJOR_SECOND, Gb4_NOTE + MAJOR_THIRD, Gb4_NOTE + PERFECT_FOURTH, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MAJOR_SIXTH, Gb4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// G
		{G4_NOTE, G4_NOTE + MAJOR_SECOND, G4_NOTE + MAJOR_THIRD, G4_NOTE + PERFECT_FOURTH, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MAJOR_SIXTH, G4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MAJOR_SECOND, Ab4_NOTE + MAJOR_THIRD, Ab4_NOTE + PERFECT_FOURTH, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MAJOR_SIXTH, Ab4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// A
		{A4_NOTE, A4_NOTE + MAJOR_SECOND, A4_NOTE + MAJOR_THIRD, A4_NOTE + PERFECT_FOURTH, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MAJOR_SIXTH, A4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MAJOR_SECOND, Bb4_NOTE + MAJOR_THIRD, Bb4_NOTE + PERFECT_FOURTH, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MAJOR_SIXTH, Bb4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// B
		{B4_NOTE, B4_NOTE + MAJOR_SECOND, B4_NOTE + MAJOR_THIRD, B4_NOTE + PERFECT_FOURTH, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MAJOR_SIXTH, B4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f}},
	// Dorian
	{
		// C
		{C4_NOTE, C4_NOTE + MAJOR_SECOND, C4_NOTE + MINOR_THIRD, C4_NOTE + PERFECT_FOURTH, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MAJOR_SIXTH, C4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MAJOR_SECOND, Db4_NOTE + MINOR_THIRD, Db4_NOTE + PERFECT_FOURTH, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MAJOR_SIXTH, Db4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// D
		{D4_NOTE, D4_NOTE + MAJOR_SECOND, D4_NOTE + MINOR_THIRD, D4_NOTE + PERFECT_FOURTH, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MAJOR_SIXTH, D4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MAJOR_SECOND, Eb4_NOTE + MINOR_THIRD, Eb4_NOTE + PERFECT_FOURTH, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MAJOR_SIXTH, Eb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// E
		{E4_NOTE, E4_NOTE + MAJOR_SECOND, E4_NOTE + MINOR_THIRD, E4_NOTE + PERFECT_FOURTH, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MAJOR_SIXTH, E4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// F
		{F4_NOTE, F4_NOTE + MAJOR_SECOND, F4_NOTE + MINOR_THIRD, F4_NOTE + PERFECT_FOURTH, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MAJOR_SIXTH, F4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MAJOR_SECOND, Gb4_NOTE + MINOR_THIRD, Gb4_NOTE + PERFECT_FOURTH, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MAJOR_SIXTH, Gb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// G
		{G4_NOTE, G4_NOTE + MAJOR_SECOND, G4_NOTE + MINOR_THIRD, G4_NOTE + PERFECT_FOURTH, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MAJOR_SIXTH, G4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MAJOR_SECOND, Ab4_NOTE + MINOR_THIRD, Ab4_NOTE + PERFECT_FOURTH, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MAJOR_SIXTH, Ab4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// A
		{A4_NOTE, A4_NOTE + MAJOR_SECOND, A4_NOTE + MINOR_THIRD, A4_NOTE + PERFECT_FOURTH, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MAJOR_SIXTH, A4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MAJOR_SECOND, Bb4_NOTE + MINOR_THIRD, Bb4_NOTE + PERFECT_FOURTH, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MAJOR_SIXTH, Bb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// B
		{B4_NOTE, B4_NOTE + MAJOR_SECOND, B4_NOTE + MINOR_THIRD, B4_NOTE + PERFECT_FOURTH, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MAJOR_SIXTH, B4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f}},
	// Phrygian
	{
		// C
		{C4_NOTE, C4_NOTE + MINOR_SECOND, C4_NOTE + MINOR_THIRD, C4_NOTE + PERFECT_FOURTH, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MINOR_SIXTH, C4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MINOR_SECOND, Db4_NOTE + MINOR_THIRD, Db4_NOTE + PERFECT_FOURTH, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MINOR_SIXTH, Db4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// D
		{D4_NOTE, D4_NOTE + MINOR_SECOND, D4_NOTE + MINOR_THIRD, D4_NOTE + PERFECT_FOURTH, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MINOR_SIXTH, D4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MINOR_SECOND, Eb4_NOTE + MINOR_THIRD, Eb4_NOTE + PERFECT_FOURTH, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MINOR_SIXTH, Eb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// E
		{E4_NOTE, E4_NOTE + MINOR_SECOND, E4_NOTE + MINOR_THIRD, E4_NOTE + PERFECT_FOURTH, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MINOR_SIXTH, E4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// F
		{F4_NOTE, F4_NOTE + MINOR_SECOND, F4_NOTE + MINOR_THIRD, F4_NOTE + PERFECT_FOURTH, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MINOR_SIXTH, F4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MINOR_SECOND, Gb4_NOTE + MINOR_THIRD, Gb4_NOTE + PERFECT_FOURTH, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MINOR_SIXTH, Gb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// G
		{G4_NOTE, G4_NOTE + MINOR_SECOND, G4_NOTE + MINOR_THIRD, G4_NOTE + PERFECT_FOURTH, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MINOR_SIXTH, G4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MINOR_SECOND, Ab4_NOTE + MINOR_THIRD, Ab4_NOTE + PERFECT_FOURTH, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MINOR_SIXTH, Ab4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// A
		{A4_NOTE, A4_NOTE + MINOR_SECOND, A4_NOTE + MINOR_THIRD, A4_NOTE + PERFECT_FOURTH, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MINOR_SIXTH, A4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MINOR_SECOND, Bb4_NOTE + MINOR_THIRD, Bb4_NOTE + PERFECT_FOURTH, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MINOR_SIXTH, Bb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// B
		{B4_NOTE, B4_NOTE + MINOR_SECOND, B4_NOTE + MINOR_THIRD, B4_NOTE + PERFECT_FOURTH, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MINOR_SIXTH, B4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f}},
	// Lydian
	{
		// C
		{C4_NOTE, C4_NOTE + MAJOR_SECOND, C4_NOTE + MAJOR_THIRD, C4_NOTE + AUG_FOURTH, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MAJOR_SIXTH, C4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MAJOR_SECOND, Db4_NOTE + MAJOR_THIRD, Db4_NOTE + AUG_FOURTH, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MAJOR_SIXTH, Db4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// D
		{D4_NOTE, D4_NOTE + MAJOR_SECOND, D4_NOTE + MAJOR_THIRD, D4_NOTE + AUG_FOURTH, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MAJOR_SIXTH, D4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MAJOR_SECOND, Eb4_NOTE + MAJOR_THIRD, Eb4_NOTE + AUG_FOURTH, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MAJOR_SIXTH, Eb4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// E
		{E4_NOTE, E4_NOTE + MAJOR_SECOND, E4_NOTE + MAJOR_THIRD, E4_NOTE + AUG_FOURTH, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MAJOR_SIXTH, E4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// F
		{F4_NOTE, F4_NOTE + MAJOR_SECOND, F4_NOTE + MAJOR_THIRD, F4_NOTE + AUG_FOURTH, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MAJOR_SIXTH, F4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MAJOR_SECOND, Gb4_NOTE + MAJOR_THIRD, Gb4_NOTE + AUG_FOURTH, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MAJOR_SIXTH, Gb4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// G
		{G4_NOTE, G4_NOTE + MAJOR_SECOND, G4_NOTE + MAJOR_THIRD, G4_NOTE + AUG_FOURTH, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MAJOR_SIXTH, G4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MAJOR_SECOND, Ab4_NOTE + MAJOR_THIRD, Ab4_NOTE + AUG_FOURTH, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MAJOR_SIXTH, Ab4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// A
		{A4_NOTE, A4_NOTE + MAJOR_SECOND, A4_NOTE + MAJOR_THIRD, A4_NOTE + AUG_FOURTH, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MAJOR_SIXTH, A4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MAJOR_SECOND, Bb4_NOTE + MAJOR_THIRD, Bb4_NOTE + AUG_FOURTH, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MAJOR_SIXTH, Bb4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// B
		{B4_NOTE, B4_NOTE + MAJOR_SECOND, B4_NOTE + MAJOR_THIRD, B4_NOTE + AUG_FOURTH, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MAJOR_SIXTH, B4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f}},
	// Mixolydian
	{
		// C
		{C4_NOTE, C4_NOTE + MAJOR_SECOND, C4_NOTE + MAJOR_THIRD, C4_NOTE + PERFECT_FOURTH, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MAJOR_SIXTH, C4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MAJOR_SECOND, Db4_NOTE + MAJOR_THIRD, Db4_NOTE + PERFECT_FOURTH, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MAJOR_SIXTH, Db4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// D
		{D4_NOTE, D4_NOTE + MAJOR_SECOND, D4_NOTE + MAJOR_THIRD, D4_NOTE + PERFECT_FOURTH, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MAJOR_SIXTH, D4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MAJOR_SECOND, Eb4_NOTE + MAJOR_THIRD, Eb4_NOTE + PERFECT_FOURTH, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MAJOR_SIXTH, Eb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// E
		{E4_NOTE, E4_NOTE + MAJOR_SECOND, E4_NOTE + MAJOR_THIRD, E4_NOTE + PERFECT_FOURTH, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MAJOR_SIXTH, E4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// F
		{F4_NOTE, F4_NOTE + MAJOR_SECOND, F4_NOTE + MAJOR_THIRD, F4_NOTE + PERFECT_FOURTH, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MAJOR_SIXTH, F4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MAJOR_SECOND, Gb4_NOTE + MAJOR_THIRD, Gb4_NOTE + PERFECT_FOURTH, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MAJOR_SIXTH, Gb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// G
		{G4_NOTE, G4_NOTE + MAJOR_SECOND, G4_NOTE + MAJOR_THIRD, G4_NOTE + PERFECT_FOURTH, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MAJOR_SIXTH, G4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MAJOR_SECOND, Ab4_NOTE + MAJOR_THIRD, Ab4_NOTE + PERFECT_FOURTH, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MAJOR_SIXTH, Ab4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// A
		{A4_NOTE, A4_NOTE + MAJOR_SECOND, A4_NOTE + MAJOR_THIRD, A4_NOTE + PERFECT_FOURTH, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MAJOR_SIXTH, A4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MAJOR_SECOND, Bb4_NOTE + MAJOR_THIRD, Bb4_NOTE + PERFECT_FOURTH, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MAJOR_SIXTH, Bb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// B
		{B4_NOTE, B4_NOTE + MAJOR_SECOND, B4_NOTE + MAJOR_THIRD, B4_NOTE + PERFECT_FOURTH, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MAJOR_SIXTH, B4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f}},
	// Aeolian/Natural Minor
	{
		// C
		{C4_NOTE, C4_NOTE + MAJOR_SECOND, C4_NOTE + MINOR_THIRD, C4_NOTE + PERFECT_FOURTH, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MINOR_SIXTH, C4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MAJOR_SECOND, Db4_NOTE + MINOR_THIRD, Db4_NOTE + PERFECT_FOURTH, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MINOR_SIXTH, Db4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// D
		{D4_NOTE, D4_NOTE + MAJOR_SECOND, D4_NOTE + MINOR_THIRD, D4_NOTE + PERFECT_FOURTH, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MINOR_SIXTH, D4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MAJOR_SECOND, Eb4_NOTE + MINOR_THIRD, Eb4_NOTE + PERFECT_FOURTH, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MINOR_SIXTH, Eb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// E
		{E4_NOTE, E4_NOTE + MAJOR_SECOND, E4_NOTE + MINOR_THIRD, E4_NOTE + PERFECT_FOURTH, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MINOR_SIXTH, E4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// F
		{F4_NOTE, F4_NOTE + MAJOR_SECOND, F4_NOTE + MINOR_THIRD, F4_NOTE + PERFECT_FOURTH, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MINOR_SIXTH, F4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MAJOR_SECOND, Gb4_NOTE + MINOR_THIRD, Gb4_NOTE + PERFECT_FOURTH, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MINOR_SIXTH, Gb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// G
		{G4_NOTE, G4_NOTE + MAJOR_SECOND, G4_NOTE + MINOR_THIRD, G4_NOTE + PERFECT_FOURTH, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MINOR_SIXTH, G4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MAJOR_SECOND, Ab4_NOTE + MINOR_THIRD, Ab4_NOTE + PERFECT_FOURTH, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MINOR_SIXTH, Ab4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// A
		{A4_NOTE, A4_NOTE + MAJOR_SECOND, A4_NOTE + MINOR_THIRD, A4_NOTE + PERFECT_FOURTH, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MINOR_SIXTH, A4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MAJOR_SECOND, Bb4_NOTE + MINOR_THIRD, Bb4_NOTE + PERFECT_FOURTH, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MINOR_SIXTH, Bb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// B
		{B4_NOTE, B4_NOTE + MAJOR_SECOND, B4_NOTE + MINOR_THIRD, B4_NOTE + PERFECT_FOURTH, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MINOR_SIXTH, B4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f}},
	// Locrian
	{
		// C
		{C4_NOTE, C4_NOTE + MINOR_SECOND, C4_NOTE + MINOR_THIRD, C4_NOTE + PERFECT_FOURTH, C4_NOTE + DIM_FIFTH, C4_NOTE + MINOR_SIXTH, C4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MINOR_SECOND, Db4_NOTE + MINOR_THIRD, Db4_NOTE + PERFECT_FOURTH, Db4_NOTE + DIM_FIFTH, Db4_NOTE + MINOR_SIXTH, Db4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// D
		{D4_NOTE, D4_NOTE + MINOR_SECOND, D4_NOTE + MINOR_THIRD, D4_NOTE + PERFECT_FOURTH, D4_NOTE + DIM_FIFTH, D4_NOTE + MINOR_SIXTH, D4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MINOR_SECOND, Eb4_NOTE + MINOR_THIRD, Eb4_NOTE + PERFECT_FOURTH, Eb4_NOTE + DIM_FIFTH, Eb4_NOTE + MINOR_SIXTH, Eb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// E
		{E4_NOTE, E4_NOTE + MINOR_SECOND, E4_NOTE + MINOR_THIRD, E4_NOTE + PERFECT_FOURTH, E4_NOTE + DIM_FIFTH, E4_NOTE + MINOR_SIXTH, E4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// F
		{F4_NOTE, F4_NOTE + MINOR_SECOND, F4_NOTE + MINOR_THIRD, F4_NOTE + PERFECT_FOURTH, F4_NOTE + DIM_FIFTH, F4_NOTE + MINOR_SIXTH, F4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MINOR_SECOND, Gb4_NOTE + MINOR_THIRD, Gb4_NOTE + PERFECT_FOURTH, Gb4_NOTE + DIM_FIFTH, Gb4_NOTE + MINOR_SIXTH, Gb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// G
		{G4_NOTE, G4_NOTE + MINOR_SECOND, G4_NOTE + MINOR_THIRD, G4_NOTE + PERFECT_FOURTH, G4_NOTE + DIM_FIFTH, G4_NOTE + MINOR_SIXTH, G4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MINOR_SECOND, Ab4_NOTE + MINOR_THIRD, Ab4_NOTE + PERFECT_FOURTH, Ab4_NOTE + DIM_FIFTH, Ab4_NOTE + MINOR_SIXTH, Ab4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// A
		{A4_NOTE, A4_NOTE + MINOR_SECOND, A4_NOTE + MINOR_THIRD, A4_NOTE + PERFECT_FOURTH, A4_NOTE + DIM_FIFTH, A4_NOTE + MINOR_SIXTH, A4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MINOR_SECOND, Bb4_NOTE + MINOR_THIRD, Bb4_NOTE + PERFECT_FOURTH, Bb4_NOTE + DIM_FIFTH, Bb4_NOTE + MINOR_SIXTH, Bb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f},
		// B
		{B4_NOTE, B4_NOTE + MINOR_SECOND, B4_NOTE + MINOR_THIRD, B4_NOTE + PERFECT_FOURTH, B4_NOTE + DIM_FIFTH, B4_NOTE + MINOR_SIXTH, B4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, 7.f}},

	// Minor Pentatonic
	{
		// C
		{C4_NOTE, C4_NOTE + MINOR_THIRD, C4_NOTE + PERFECT_FOURTH, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MINOR_THIRD, Db4_NOTE + PERFECT_FOURTH, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// D
		{D4_NOTE, D4_NOTE + MINOR_THIRD, D4_NOTE + PERFECT_FOURTH, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MINOR_THIRD, Eb4_NOTE + PERFECT_FOURTH, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// E
		{E4_NOTE, E4_NOTE + MINOR_THIRD, E4_NOTE + PERFECT_FOURTH, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// F
		{F4_NOTE, F4_NOTE + MINOR_THIRD, F4_NOTE + PERFECT_FOURTH, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MINOR_THIRD, Gb4_NOTE + PERFECT_FOURTH, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// G
		{G4_NOTE, G4_NOTE + MINOR_THIRD, G4_NOTE + PERFECT_FOURTH, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MINOR_THIRD, Ab4_NOTE + PERFECT_FOURTH, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// A
		{A4_NOTE, A4_NOTE + MINOR_THIRD, A4_NOTE + PERFECT_FOURTH, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MINOR_THIRD, Bb4_NOTE + PERFECT_FOURTH, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// B
		{B4_NOTE, B4_NOTE + MINOR_THIRD, B4_NOTE + PERFECT_FOURTH, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f}},
	// Major Pentatonic
	{
		// C
		{C4_NOTE, C4_NOTE + MAJOR_SECOND, C4_NOTE + MAJOR_THIRD, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MAJOR_SECOND, Db4_NOTE + MAJOR_THIRD, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// D
		{D4_NOTE, D4_NOTE + MAJOR_SECOND, D4_NOTE + MAJOR_THIRD, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MAJOR_SECOND, Eb4_NOTE + MAJOR_THIRD, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// E
		{E4_NOTE, E4_NOTE + MAJOR_SECOND, E4_NOTE + MAJOR_THIRD, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// F
		{F4_NOTE, F4_NOTE + MAJOR_SECOND, F4_NOTE + MAJOR_THIRD, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MAJOR_SECOND, Gb4_NOTE + MAJOR_THIRD, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// G
		{G4_NOTE, G4_NOTE + MAJOR_SECOND, G4_NOTE + MAJOR_THIRD, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MAJOR_SECOND, Ab4_NOTE + MAJOR_THIRD, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// A
		{A4_NOTE, A4_NOTE + MAJOR_SECOND, A4_NOTE + MAJOR_THIRD, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MAJOR_SECOND, Bb4_NOTE + MAJOR_THIRD, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f},
		// B
		{B4_NOTE, B4_NOTE + MAJOR_SECOND, B4_NOTE + MAJOR_THIRD, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 5.f}},
	// Minor Blues
	{
		// C
		{C4_NOTE, C4_NOTE + MINOR_THIRD, C4_NOTE + PERFECT_FOURTH, C4_NOTE + DIM_FIFTH, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MINOR_THIRD, Db4_NOTE + PERFECT_FOURTH, Db4_NOTE + DIM_FIFTH, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// D
		{D4_NOTE, D4_NOTE + MINOR_THIRD, D4_NOTE + PERFECT_FOURTH, D4_NOTE + DIM_FIFTH, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MINOR_THIRD, Eb4_NOTE + PERFECT_FOURTH, Eb4_NOTE + DIM_FIFTH, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// E
		{E4_NOTE, E4_NOTE + MINOR_THIRD, E4_NOTE + PERFECT_FOURTH, E4_NOTE + DIM_FIFTH, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// F
		{F4_NOTE, F4_NOTE + MINOR_THIRD, F4_NOTE + PERFECT_FOURTH, F4_NOTE + DIM_FIFTH, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MINOR_THIRD, Gb4_NOTE + PERFECT_FOURTH, Gb4_NOTE + DIM_FIFTH, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// G
		{G4_NOTE, G4_NOTE + MINOR_THIRD, G4_NOTE + PERFECT_FOURTH, G4_NOTE + DIM_FIFTH, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MINOR_THIRD, Ab4_NOTE + PERFECT_FOURTH, Ab4_NOTE + DIM_FIFTH, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// A
		{A4_NOTE, A4_NOTE + MINOR_THIRD, A4_NOTE + PERFECT_FOURTH, A4_NOTE + DIM_FIFTH, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MINOR_THIRD, Bb4_NOTE + PERFECT_FOURTH, Bb4_NOTE + DIM_FIFTH, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// B
		{B4_NOTE, B4_NOTE + MINOR_THIRD, B4_NOTE + PERFECT_FOURTH, B4_NOTE + DIM_FIFTH, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f}},
	// Major Blues
	{
		// C
		{C4_NOTE, C4_NOTE + MAJOR_SECOND, C4_NOTE + MINOR_THIRD, C4_NOTE + MAJOR_THIRD, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MAJOR_SECOND, Db4_NOTE + MINOR_THIRD, Db4_NOTE + MAJOR_THIRD, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// D
		{D4_NOTE, D4_NOTE + MAJOR_SECOND, D4_NOTE + MINOR_THIRD, D4_NOTE + MAJOR_THIRD, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MAJOR_SECOND, Eb4_NOTE + MINOR_THIRD, Eb4_NOTE + MAJOR_THIRD, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// E
		{E4_NOTE, E4_NOTE + MAJOR_SECOND, E4_NOTE + MINOR_THIRD, E4_NOTE + MAJOR_THIRD, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// F
		{F4_NOTE, F4_NOTE + MAJOR_SECOND, F4_NOTE + MINOR_THIRD, F4_NOTE + MAJOR_THIRD, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MAJOR_SECOND, Gb4_NOTE + MINOR_THIRD, Gb4_NOTE + MAJOR_THIRD, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// G
		{G4_NOTE, G4_NOTE + MAJOR_SECOND, G4_NOTE + MINOR_THIRD, G4_NOTE + MAJOR_THIRD, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MAJOR_SECOND, Ab4_NOTE + MINOR_THIRD, Ab4_NOTE + MAJOR_THIRD, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// A
		{A4_NOTE, A4_NOTE + MAJOR_SECOND, A4_NOTE + MINOR_THIRD, A4_NOTE + MAJOR_THIRD, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MAJOR_SECOND, Bb4_NOTE + MINOR_THIRD, Bb4_NOTE + MAJOR_THIRD, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f},
		// B
		{B4_NOTE, B4_NOTE + MAJOR_SECOND, B4_NOTE + MINOR_THIRD, B4_NOTE + MAJOR_THIRD, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 6.f}},
	// Major triads
	{
		// C
		{C4_NOTE, C4_NOTE + MAJOR_THIRD, C4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MAJOR_THIRD, Db4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D
		{D4_NOTE, D4_NOTE + MAJOR_THIRD, D4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MAJOR_THIRD, Eb4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// E
		{E4_NOTE, E4_NOTE + MAJOR_THIRD, E4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F
		{F4_NOTE, F4_NOTE + MAJOR_THIRD, F4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MAJOR_THIRD, Gb4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G
		{G4_NOTE, G4_NOTE + MAJOR_THIRD, G4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MAJOR_THIRD, Ab4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A
		{A4_NOTE, A4_NOTE + MAJOR_THIRD, A4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MAJOR_THIRD, Bb4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// B
		{B4_NOTE, B4_NOTE + MAJOR_THIRD, B4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f}},
	// Minor triads
	{
		// C
		{C4_NOTE, C4_NOTE + MINOR_THIRD, C4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MINOR_THIRD, Db4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D
		{D4_NOTE, D4_NOTE + MINOR_THIRD, D4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MINOR_THIRD, Eb4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// E
		{E4_NOTE, E4_NOTE + MINOR_THIRD, E4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F
		{F4_NOTE, F4_NOTE + MINOR_THIRD, F4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MINOR_THIRD, Gb4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G
		{G4_NOTE, G4_NOTE + MINOR_THIRD, G4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MINOR_THIRD, Ab4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A
		{A4_NOTE, A4_NOTE + MINOR_THIRD, A4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MINOR_THIRD, Bb4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// B
		{B4_NOTE, B4_NOTE + MINOR_THIRD, B4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f}},
	// Diminished Triads
	{
		// C
		{C4_NOTE, C4_NOTE + MINOR_THIRD, C4_NOTE + DIM_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MINOR_THIRD, Db4_NOTE + DIM_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D
		{D4_NOTE, D4_NOTE + MINOR_THIRD, D4_NOTE + DIM_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MINOR_THIRD, Eb4_NOTE + DIM_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// E
		{E4_NOTE, E4_NOTE + MINOR_THIRD, E4_NOTE + DIM_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F
		{F4_NOTE, F4_NOTE + MINOR_THIRD, F4_NOTE + DIM_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MINOR_THIRD, Gb4_NOTE + DIM_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G
		{G4_NOTE, G4_NOTE + MINOR_THIRD, G4_NOTE + DIM_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MINOR_THIRD, Ab4_NOTE + DIM_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A
		{A4_NOTE, A4_NOTE + MINOR_THIRD, A4_NOTE + DIM_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MINOR_THIRD, Bb4_NOTE + DIM_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// B
		{B4_NOTE, B4_NOTE + MINOR_THIRD, B4_NOTE + DIM_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f}},
	// Augmented triads
	{
		// C
		{C4_NOTE, C4_NOTE + MAJOR_THIRD, C4_NOTE + AUG_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MAJOR_THIRD, Db4_NOTE + AUG_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D
		{D4_NOTE, D4_NOTE + MAJOR_THIRD, D4_NOTE + AUG_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MAJOR_THIRD, Eb4_NOTE + AUG_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// E
		{E4_NOTE, E4_NOTE + MAJOR_THIRD, E4_NOTE + AUG_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F
		{F4_NOTE, F4_NOTE + MAJOR_THIRD, F4_NOTE + AUG_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MAJOR_THIRD, Gb4_NOTE + AUG_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G
		{G4_NOTE, G4_NOTE + MAJOR_THIRD, G4_NOTE + AUG_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MAJOR_THIRD, Ab4_NOTE + AUG_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A
		{A4_NOTE, A4_NOTE + MAJOR_THIRD, A4_NOTE + AUG_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MAJOR_THIRD, Bb4_NOTE + AUG_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// B
		{B4_NOTE, B4_NOTE + MAJOR_THIRD, B4_NOTE + AUG_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f}},
	// Sus2 triads
	{
		// C
		{C4_NOTE, C4_NOTE + MAJOR_SECOND, C4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MAJOR_SECOND, Db4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D
		{D4_NOTE, D4_NOTE + MAJOR_SECOND, D4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MAJOR_SECOND, Eb4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// E
		{E4_NOTE, E4_NOTE + MAJOR_SECOND, E4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F
		{F4_NOTE, F4_NOTE + MAJOR_SECOND, F4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MAJOR_SECOND, Gb4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G
		{G4_NOTE, G4_NOTE + MAJOR_SECOND, G4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MAJOR_SECOND, Ab4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A
		{A4_NOTE, A4_NOTE + MAJOR_SECOND, A4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MAJOR_SECOND, Bb4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// B
		{B4_NOTE, B4_NOTE + MAJOR_SECOND, B4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f}},
	// Sus4 triads
	{
		// C
		{C4_NOTE, C4_NOTE + PERFECT_FOURTH, C4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + PERFECT_FOURTH, Db4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D
		{D4_NOTE, D4_NOTE + PERFECT_FOURTH, D4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + PERFECT_FOURTH, Eb4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// E
		{E4_NOTE, E4_NOTE + PERFECT_FOURTH, E4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F
		{F4_NOTE, F4_NOTE + PERFECT_FOURTH, F4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + PERFECT_FOURTH, Gb4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G
		{G4_NOTE, G4_NOTE + PERFECT_FOURTH, G4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + PERFECT_FOURTH, Ab4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A
		{A4_NOTE, A4_NOTE + PERFECT_FOURTH, A4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + PERFECT_FOURTH, Bb4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// B
		{B4_NOTE, B4_NOTE + PERFECT_FOURTH, B4_NOTE + PERFECT_FIFTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f}},
	// 7/Septime chords
	{
		// C
		{C4_NOTE, C4_NOTE + MAJOR_THIRD, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MAJOR_THIRD, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D
		{D4_NOTE, D4_NOTE + MAJOR_THIRD, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MAJOR_THIRD, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// E
		{E4_NOTE, E4_NOTE + MAJOR_THIRD, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F
		{F4_NOTE, F4_NOTE + MAJOR_THIRD, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MAJOR_THIRD, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G
		{G4_NOTE, G4_NOTE + MAJOR_THIRD, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MAJOR_THIRD, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A
		{A4_NOTE, A4_NOTE + MAJOR_THIRD, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MAJOR_THIRD, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// B
		{B4_NOTE, B4_NOTE + MAJOR_THIRD, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f}},
	// Maj7 chords
	{
		// C
		{C4_NOTE, C4_NOTE + MAJOR_THIRD, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MAJOR_THIRD, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D
		{D4_NOTE, D4_NOTE + MAJOR_THIRD, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MAJOR_THIRD, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// E
		{E4_NOTE, E4_NOTE + MAJOR_THIRD, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F
		{F4_NOTE, F4_NOTE + MAJOR_THIRD, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MAJOR_THIRD, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G
		{G4_NOTE, G4_NOTE + MAJOR_THIRD, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MAJOR_THIRD, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A
		{A4_NOTE, A4_NOTE + MAJOR_THIRD, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MAJOR_THIRD, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// B
		{B4_NOTE, B4_NOTE + MAJOR_THIRD, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MAJOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f}},
	// Min7 chords
	{
		// C
		{C4_NOTE, C4_NOTE + MINOR_THIRD, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MINOR_THIRD, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D
		{D4_NOTE, D4_NOTE + MINOR_THIRD, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MINOR_THIRD, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// E
		{E4_NOTE, E4_NOTE + MINOR_THIRD, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F
		{F4_NOTE, F4_NOTE + MINOR_THIRD, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MINOR_THIRD, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G
		{G4_NOTE, G4_NOTE + MINOR_THIRD, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MINOR_THIRD, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A
		{A4_NOTE, A4_NOTE + MINOR_THIRD, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MINOR_THIRD, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// B
		{B4_NOTE, B4_NOTE + MINOR_THIRD, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f}},
	// Major 6 chords
	{
		// C
		{C4_NOTE, C4_NOTE + MAJOR_THIRD, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MAJOR_THIRD, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D
		{D4_NOTE, D4_NOTE + MAJOR_THIRD, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MAJOR_THIRD, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// E
		{E4_NOTE, E4_NOTE + MAJOR_THIRD, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F
		{F4_NOTE, F4_NOTE + MAJOR_THIRD, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MAJOR_THIRD, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G
		{G4_NOTE, G4_NOTE + MAJOR_THIRD, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MAJOR_THIRD, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A
		{A4_NOTE, A4_NOTE + MAJOR_THIRD, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MAJOR_THIRD, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// B
		{B4_NOTE, B4_NOTE + MAJOR_THIRD, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f}},
	// Minor 6 chords
	{
		// C
		{C4_NOTE, C4_NOTE + MINOR_THIRD, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MINOR_THIRD, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D
		{D4_NOTE, D4_NOTE + MINOR_THIRD, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MINOR_THIRD, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// E
		{E4_NOTE, E4_NOTE + MINOR_THIRD, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F
		{F4_NOTE, F4_NOTE + MINOR_THIRD, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MINOR_THIRD, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G
		{G4_NOTE, G4_NOTE + MINOR_THIRD, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MINOR_THIRD, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A
		{A4_NOTE, A4_NOTE + MINOR_THIRD, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MINOR_THIRD, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// B
		{B4_NOTE, B4_NOTE + MINOR_THIRD, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MAJOR_SIXTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f}},
	// Major add9 chords
	{
		// C
		{C4_NOTE, C4_NOTE + MAJOR_THIRD, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MAJOR_THIRD, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D
		{D4_NOTE, D4_NOTE + MAJOR_THIRD, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MAJOR_THIRD, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// E
		{E4_NOTE, E4_NOTE + MAJOR_THIRD, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F
		{F4_NOTE, F4_NOTE + MAJOR_THIRD, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MAJOR_THIRD, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G
		{G4_NOTE, G4_NOTE + MAJOR_THIRD, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MAJOR_THIRD, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A
		{A4_NOTE, A4_NOTE + MAJOR_THIRD, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MAJOR_THIRD, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// B
		{B4_NOTE, B4_NOTE + MAJOR_THIRD, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f}},
	// Minor add9 chords
	{
		// C
		{C4_NOTE, C4_NOTE + MINOR_THIRD, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MINOR_THIRD, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D
		{D4_NOTE, D4_NOTE + MINOR_THIRD, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MINOR_THIRD, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// E
		{E4_NOTE, E4_NOTE + MINOR_THIRD, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F
		{F4_NOTE, F4_NOTE + MINOR_THIRD, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MINOR_THIRD, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G
		{G4_NOTE, G4_NOTE + MINOR_THIRD, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MINOR_THIRD, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A
		{A4_NOTE, A4_NOTE + MINOR_THIRD, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MINOR_THIRD, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// B
		{B4_NOTE, B4_NOTE + MINOR_THIRD, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f}},
	// Min7b5 chords
	{
		// C
		{C4_NOTE, C4_NOTE + MINOR_THIRD, C4_NOTE + DIM_FIFTH, C4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MINOR_THIRD, Db4_NOTE + DIM_FIFTH, Db4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D
		{D4_NOTE, D4_NOTE + MINOR_THIRD, D4_NOTE + DIM_FIFTH, D4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MINOR_THIRD, Eb4_NOTE + DIM_FIFTH, Eb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// E
		{E4_NOTE, E4_NOTE + MINOR_THIRD, E4_NOTE + DIM_FIFTH, E4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F
		{F4_NOTE, F4_NOTE + MINOR_THIRD, F4_NOTE + DIM_FIFTH, F4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MINOR_THIRD, Gb4_NOTE + DIM_FIFTH, Gb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G
		{G4_NOTE, G4_NOTE + MINOR_THIRD, G4_NOTE + DIM_FIFTH, G4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MINOR_THIRD, Ab4_NOTE + DIM_FIFTH, Ab4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A
		{A4_NOTE, A4_NOTE + MINOR_THIRD, A4_NOTE + DIM_FIFTH, A4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MINOR_THIRD, Bb4_NOTE + DIM_FIFTH, Bb4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// B
		{B4_NOTE, B4_NOTE + MINOR_THIRD, B4_NOTE + DIM_FIFTH, B4_NOTE + MINOR_SEVENTH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f}},
	// Major 9 chords
	{
		// C
		{C4_NOTE, C4_NOTE + MAJOR_THIRD, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MAJOR_THIRD, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D
		{D4_NOTE, D4_NOTE + MAJOR_THIRD, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MAJOR_THIRD, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// E
		{E4_NOTE, E4_NOTE + MAJOR_THIRD, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F
		{F4_NOTE, F4_NOTE + MAJOR_THIRD, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MAJOR_THIRD, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G
		{G4_NOTE, G4_NOTE + MAJOR_THIRD, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MAJOR_THIRD, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A
		{A4_NOTE, A4_NOTE + MAJOR_THIRD, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MAJOR_THIRD, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// B
		{B4_NOTE, B4_NOTE + MAJOR_THIRD, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f}},
	// Minor 9 chords
	{
		// C
		{C4_NOTE, C4_NOTE + MINOR_THIRD, C4_NOTE + PERFECT_FIFTH, C4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// C#/Db
		{Db4_NOTE, Db4_NOTE + MINOR_THIRD, Db4_NOTE + PERFECT_FIFTH, Db4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D
		{D4_NOTE, D4_NOTE + MINOR_THIRD, D4_NOTE + PERFECT_FIFTH, D4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// D#/Eb
		{Eb4_NOTE, Eb4_NOTE + MINOR_THIRD, Eb4_NOTE + PERFECT_FIFTH, Eb4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// E
		{E4_NOTE, E4_NOTE + MINOR_THIRD, E4_NOTE + PERFECT_FIFTH, E4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F
		{F4_NOTE, F4_NOTE + MINOR_THIRD, F4_NOTE + PERFECT_FIFTH, F4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// F#/Gb
		{Gb4_NOTE, Gb4_NOTE + MINOR_THIRD, Gb4_NOTE + PERFECT_FIFTH, Gb4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G
		{G4_NOTE, G4_NOTE + MINOR_THIRD, G4_NOTE + PERFECT_FIFTH, G4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// G#/Ab
		{Ab4_NOTE, Ab4_NOTE + MINOR_THIRD, Ab4_NOTE + PERFECT_FIFTH, Ab4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A
		{A4_NOTE, A4_NOTE + MINOR_THIRD, A4_NOTE + PERFECT_FIFTH, A4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// A#/Bb
		{Bb4_NOTE, Bb4_NOTE + MINOR_THIRD, Bb4_NOTE + PERFECT_FIFTH, Bb4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f},
		// B
		{B4_NOTE, B4_NOTE + MINOR_THIRD, B4_NOTE + PERFECT_FIFTH, B4_NOTE + MINOR_SEVENTH, C4_NOTE + MAJOR_NINETH, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, 3.f}}};
