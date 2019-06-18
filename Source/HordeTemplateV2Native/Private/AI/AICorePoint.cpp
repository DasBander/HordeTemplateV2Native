

#include "AICorePoint.h"
#include "HordeTemplateV2Native.h"
#include "ConstructorHelpers.h"

AAICorePoint::AAICorePoint()
{
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Scene Root"));
	Icon = CreateDefaultSubobject<UBillboardComponent>(TEXT("Icon"));
	Icon->SetupAttachment(RootComponent);
	const ConstructorHelpers::FObjectFinder<UTexture2D> IconAsset(TEXT("Texture2D'/Engine/EditorMaterials/TargetIcon.TargetIcon'"));
	if (IconAsset.Succeeded())
	{
		Icon->SetSprite(IconAsset.Object);
	}


}




