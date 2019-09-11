

#include "BloodImpact.h"
#include "ConstructorHelpers.h"

/*
	FUNCTION: Constructor for ABloodImpact
	PARAM: None
	RETURN: None
	DESC:
	Constructor for ABloodImpact. Overriding Impact Sound and Particle.
*/
ABloodImpact::ABloodImpact()
{
	const ConstructorHelpers::FObjectFinder<USoundCue> BloodImpactSound(TEXT("SoundCue'/Game/HordeTemplateBP/Assets/Sounds/A_FleshImpact_Cue.A_FleshImpact_Cue'"));
	if (BloodImpactSound.Succeeded())
	{
		ImpactSound->SetSound(BloodImpactSound.Object);
	}

	const ConstructorHelpers::FObjectFinder<UParticleSystem> BloodParticleAsset(TEXT("ParticleSystem'/Game/HordeTemplateBP/Assets/Effects/ParticleSystems/Gameplay/Player/P_body_bullet_impact.P_body_bullet_impact'"));
	if (BloodParticleAsset.Succeeded())
	{
		ImpactFX->SetTemplate(BloodParticleAsset.Object);
	}
}
