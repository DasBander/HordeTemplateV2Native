

#include "HordePlayerState.h"
#include "HordeGameState.h"
#include "HUD/HordeBaseHUD.h"
#include "HordeTemplateV2Native.h"

void AHordePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHordePlayerState, Player);
}

void AHordePlayerState::ClientUpdateGameStatus_Implementation(EGameStatus GameStatus)
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (PC)
	{
		AHordeBaseHUD* HUD = Cast<AHordeBaseHUD>(PC->GetHUD());
		if (HUD)
		{
			HUD->OnGameStatusChanged.Broadcast((uint8)GameStatus);
		}
	}
}

void AHordePlayerState::OnMessageReceived_Implementation(FChatMessage Msg)
{
	UE_LOG(LogTemp, Warning, TEXT("Message Received: %s"), *Msg.Message.ToString());
}

void AHordePlayerState::UpdateLobbyPlayerList_Implementation(const TArray<FPlayerInfo>& Players)
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (PC)
	{
		AHordeBaseHUD* HUD = Cast<AHordeBaseHUD>(PC->GetHUD());
		if (HUD)
		{
			HUD->GetLobbyWidget()->OnLobbyPlayersUpdateDelegate.Broadcast(Players);
		}
	}
}



void AHordePlayerState::ToggleReadyStatus_Implementation()
{
	Player.PlayerReady = (Player.PlayerReady) ? false : true;
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->UpdatePlayerLobby();
	}
}

bool AHordePlayerState::ToggleReadyStatus_Validate()
{
	return true;
}

void AHordePlayerState::RequestCharacterTrade_Implementation(const FString& InstigatorPlayer, const FString& TargetPlayer)
{
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		GS->StartCharacterTrade(InstigatorPlayer, TargetPlayer);
	}
}

bool AHordePlayerState::RequestCharacterTrade_Validate(const FString& InstigatorPlayer, const FString& TargetPlayer)
{
	return true;
}

void AHordePlayerState::CancelCharacterTrade_Implementation()
{
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		if (GS->TradeProgress.Target == Player.PlayerID)
		{
			GS->AbortLobbyTrade();
		}
	}
}

bool AHordePlayerState::CancelCharacterTrade_Validate()
{
	return true;
}

void AHordePlayerState::AcceptCharacterTrade_Implementation()
{
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		if (GS->TradeProgress.Target == Player.PlayerID)
		{
			GS->AcceptCharacterTrade();
		}
	}
}

bool AHordePlayerState::AcceptCharacterTrade_Validate()
{
	return true;
}

void AHordePlayerState::GettingKicked_Implementation()
{
	APlayerController* PC = Cast<APlayerController>(GetOwner());
	if (PC)
	{
		GetWorld()->Exec(GetWorld(), TEXT("disconnect?message=kicked"));
	}
}

void AHordePlayerState::SubmitMessage_Implementation(const FText& Message)
{
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if(GS)
	{
		GS->PopMessage(FChatMessage(Player.UserName, Message));
	}
}

bool AHordePlayerState::SubmitMessage_Validate(const FText& Message)
{
	return true;
}

void AHordePlayerState::RequestPlayerKick_Implementation(FPlayerInfo Player)
{
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		if (GS->LobbyInformation.OwnerID == Player.PlayerID)
		{
			GS->KickPlayer(Player.PlayerID);
		}
	}
}

bool AHordePlayerState::RequestPlayerKick_Validate(FPlayerInfo Player)
{
	return true;
}

void AHordePlayerState::BeginPlay()
{
	Super::BeginPlay();

	FTimerHandle DelayedBeginPlay;
	FTimerDelegate DelayedBeginPlayDelegate;

	DelayedBeginPlayDelegate.BindLambda([=] {
		if (HasAuthority())
		{
			AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
			if (GS)
			{
				ClientUpdateGameStatus(GS->GameStatus);
				Player.SelectedCharacter = GS->GetFreeCharacter();
				Player.PlayerID = UniqueId->ToString();
				Player.UserName = GetPlayerName();

				GS->TakePlayer(Player);
				GS->UpdatePlayerLobby();
				GS->PopMessage(FChatMessage("SERVER", FText::FromString(GetPlayerName() + " has joined.")));

			}
		}
	});

	GetWorld()->GetTimerManager().SetTimer(DelayedBeginPlay, DelayedBeginPlayDelegate, 2.f, false);
}
