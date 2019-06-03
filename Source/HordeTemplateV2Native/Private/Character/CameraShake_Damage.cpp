

#include "CameraShake_Damage.h"

UCameraShake_Damage::UCameraShake_Damage()
{
	bSingleInstance = true;
	OscillationDuration = .1f;
	OscillationBlendInTime = .1f;
	OscillationBlendOutTime = .2f;

	RotOscillation.Pitch.Amplitude = 5.f;
	RotOscillation.Pitch.Frequency = 10.f;
	RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	RotOscillation.Pitch.Waveform = EOscillatorWaveform::SineWave;

	RotOscillation.Yaw.Amplitude = 5.f;
	RotOscillation.Yaw.Frequency = 10.f;
	RotOscillation.Yaw.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	RotOscillation.Yaw.Waveform = EOscillatorWaveform::SineWave;

	LocOscillation.X.Amplitude = 5.f;
	LocOscillation.X.Frequency = 1.f;
	LocOscillation.X.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
	LocOscillation.X.Waveform = EOscillatorWaveform::SineWave;

	LocOscillation.Y.Amplitude = 5.f;
	LocOscillation.Y.Frequency = 15.f;
	LocOscillation.Y.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	LocOscillation.Y.Waveform = EOscillatorWaveform::SineWave;
}
