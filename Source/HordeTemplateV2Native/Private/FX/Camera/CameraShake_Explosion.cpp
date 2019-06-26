

#include "CameraShake_Explosion.h"

UCameraShake_Explosion::UCameraShake_Explosion()
{
	bSingleInstance = true;
	OscillationDuration = 3.f;
	OscillationBlendInTime = .1f;
	OscillationBlendOutTime = 1.f;

	RotOscillation.Pitch.Amplitude = 5.f;
	RotOscillation.Pitch.Frequency = 30.f;
	RotOscillation.Pitch.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	RotOscillation.Pitch.Waveform = EOscillatorWaveform::SineWave;

	RotOscillation.Yaw.Amplitude = 5.f;
	RotOscillation.Yaw.Frequency = 30.f;
	RotOscillation.Yaw.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	RotOscillation.Yaw.Waveform = EOscillatorWaveform::SineWave;

	LocOscillation.X.Amplitude = 5.f;
	LocOscillation.X.Frequency = 10.f;
	LocOscillation.X.InitialOffset = EInitialOscillatorOffset::EOO_OffsetZero;
	LocOscillation.X.Waveform = EOscillatorWaveform::SineWave;

	LocOscillation.Y.Amplitude = 5.f;
	LocOscillation.Y.Frequency = 30.f;
	LocOscillation.Y.InitialOffset = EInitialOscillatorOffset::EOO_OffsetRandom;
	LocOscillation.Y.Waveform = EOscillatorWaveform::SineWave;

	AnimScale = 3.f;

}
