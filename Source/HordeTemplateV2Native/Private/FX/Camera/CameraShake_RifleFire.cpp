

#include "CameraShake_RifleFire.h"

/*
	FUNCTION: Constructor for UCameraShare_RifleFire
	PARAM: None
	RETURN: None
	DESC:
	Constructor for UCameraShake_RifleFire. Sets up default variables for the camera shake.
*/
UCameraShake_RifleFire::UCameraShake_RifleFire()
{
	bSingleInstance = true;
	OscillationDuration = .1f;
	OscillationBlendInTime = .1f;
	OscillationBlendOutTime = .2f;

	RotOscillation.Pitch.Amplitude = 2.5f;
	RotOscillation.Pitch.Frequency = 5.f;
	RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	RotOscillation.Pitch.Waveform = EOscillatorWaveform::SineWave;

	RotOscillation.Yaw.Amplitude = 2.5f;
	RotOscillation.Yaw.Frequency = 5.f;
	RotOscillation.Yaw.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	RotOscillation.Yaw.Waveform = EOscillatorWaveform::SineWave;

	LocOscillation.X.Amplitude = 2.5f;
	LocOscillation.X.Frequency = 0.5f;
	LocOscillation.X.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
	LocOscillation.X.Waveform = EOscillatorWaveform::SineWave;

	LocOscillation.Y.Amplitude = 2.5f;
	LocOscillation.Y.Frequency = 7.5f;
	LocOscillation.Y.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	LocOscillation.Y.Waveform = EOscillatorWaveform::SineWave;
}
