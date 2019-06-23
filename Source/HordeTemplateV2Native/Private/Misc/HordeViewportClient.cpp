

#include "HordeViewportClient.h"

void UHordeViewportClient::AddViewportWidgetContent(TSharedRef<class SWidget> ViewportContent, const int32 ZOrder /*= 0*/)
{

	if (ViewportContentStack.Contains(ViewportContent))
	{
		return;
	}

	ViewportContentStack.AddUnique(ViewportContent);

	Super::AddViewportWidgetContent(ViewportContent, 0);
}

void UHordeViewportClient::RemoveViewportWidgetContent(TSharedRef<class SWidget> ViewportContent)
{
	ViewportContentStack.Remove(ViewportContent);
	HiddenViewportContentStack.Remove(ViewportContent);

	Super::RemoveViewportWidgetContent(ViewportContent);
}

void UHordeViewportClient::BeginDestroy()
{
	ReleaseSlateRessources();

	Super::BeginDestroy();
}

void UHordeViewportClient::DetachViewportClient()
{
	Super::DetachViewportClient();

	ReleaseSlateRessources();
}

void UHordeViewportClient::ReleaseSlateRessources()
{
	ViewportContentStack.Empty();
	HiddenViewportContentStack.Empty();
}
