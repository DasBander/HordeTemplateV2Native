

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameViewportClient.h"
#include "HordeViewportClient.generated.h"

/**
 * 
 */
UCLASS()
class HORDETEMPLATEV2NATIVE_API UHordeViewportClient : public UGameViewportClient
{
	GENERATED_BODY()

public:
	void AddViewportWidgetContent(TSharedRef<class SWidget> ViewportContent, const int32 ZOrder = 0) override;
	void RemoveViewportWidgetContent(TSharedRef<class SWidget> ViewportContent) override;

	virtual void BeginDestroy() override;
	virtual void DetachViewportClient() override;

	void ReleaseSlateResources();

protected:

	/** List of viewport content that the viewport is tracking */
	TArray<TSharedRef<class SWidget>>				ViewportContentStack;
	TArray<TSharedRef<class SWidget>>				HiddenViewportContentStack;

};
