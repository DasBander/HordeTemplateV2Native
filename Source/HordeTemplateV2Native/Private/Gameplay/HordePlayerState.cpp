

#include "HordePlayerState.h"
#include "HordeGameState.h"
#include "TimerManager.h"
#include "HUD/HordeBaseHUD.h"
#include "Gameplay/HordeBaseController.h"
#include "Character/HordeBaseCharacter.h"
#include "Gameplay/HordeGameSession.h"
#include "Runtime/Core/Public/Math/NumericLimits.h"
#include "Misc/HordeTrader.h"
#include "HUD/HordeBaseHUD.h"

void AHordePlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AHordePlayerState, Player);
	DOREPLIFETIME(AHordePlayerState, ZedKills);
	DOREPLIFETIME(AHordePlayerState, Points);
	DOREPLIFETIME(AHordePlayerState, HeadShots);
	DOREPLIFETIME(AHordePlayerState, PlayerMoney);
	DOREPLIFETIME(AHordePlayerState, bIsDead);
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

void AHordePlayerState::OnMessageReceived_Implementation(FHordeChatMessage Msg)
{
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	AHordeBaseController* PC = Cast<AHordeBaseController>(GetOwner());
	if (GS && PC)
	{
		switch (GS->GameStatus) {
		case EGameStatus::ELOBBY:
			PC->OnLobbyMessageReceivedDelegate.Broadcast(Msg);
			break;

		case EGameStatus::EINGAME:
			PC->OnMessageReceivedDelegate.Broadcast(Msg);
			break;
		}
	}
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
		if (GetWorld())
		{
			AHordeGameSession* GSS = Cast<AHordeGameSession>(GetWorld()->GetAuthGameMode()->GameSession);
			if (GSS)
			{
				UE_LOG(LogTemp, Warning, TEXT("Ending Session."));
				GSS->EndGameSession();
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("Unable to get Game Session."));
			}
			PC->ConsoleCommand("disconnect?message=disconnectedfromgame", false);
		}
	}
}

void AHordePlayerState::SubmitMessage_Implementation(const FText& Message)
{
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if(GS && Message.ToString() != "")
	{
		if (Message.ToString()[0] == *"/")
		{
			GS->ParseChatCommand(Player.PlayerID, Message.ToString());
		}
		else {
			GS->PopMessage(FHordeChatMessage(Player.UserName, Message));
		}

	}
}

bool AHordePlayerState::SubmitMessage_Validate(const FText& Message)
{
	return true;
}

void AHordePlayerState::RequestPlayerKick_Implementation(FPlayerInfo InPlayer)
{
	AHordeGameState* GS = Cast<AHordeGameState>(GetWorld()->GetGameState());
	if (GS)
	{
		if (GS->LobbyInformation.OwnerID == InPlayer.PlayerID)
		{
			GS->KickPlayer(InPlayer.PlayerID);
		}
	}
}

bool AHordePlayerState::RequestPlayerKick_Validate(FPlayerInfo InPlayer)
{
	return true;
}

void AHordePlayerState::ModifyMoney_Implementation(int32 Modifier)
{
	PlayerMoney = FMath::Clamp<int32>(PlayerMoney + Modifier, 0, MAX_int32);
}

bool AHordePlayerState::ModifyMoney_Validate(int32 Modifier)
{
	return true;
}

int32 AHordePlayerState::GetPlayerMoney()
{
	return PlayerMoney;
}

void AHordePlayerState::BuyItem_Implementation(FName SellItemID)
{
	UDataTable* TraderSellData = Cast<UDataTable>(StaticLoadObject(UDataTable::StaticClass(), nullptr, ECONOMY_DATATABLE_PATH));

	if (TraderSellData) {
		FTraderSellItem* ItemFromRow = TraderSellData->FindRow<FTraderSellItem>(SellItemID, "PlayerState - Buy Item", false);
		if (ItemFromRow && PlayerMoney >= PlayerMoney - ItemFromRow->ItemPrice)
		{
			PlayerMoney = FMath::Clamp<int32>(PlayerMoney - ItemFromRow->ItemPrice, 0, MAX_int32);
			AHordeBaseController* PC = Cast<AHordeBaseController>(GetOwner());
			if (PC)
			{
				AHordeBaseCharacter* PLY = Cast<AHordeBaseCharacter>(PC->GetPawn());
				if (PLY)
				{
					PLY->Inventory->ServerAddItem(ItemFromRow->ItemID.ToString(), false, FItem());
				}
			}
		}
	}

}

bool AHordePlayerState::BuyItem_Validate(FName SellItemID)
{
	return true;
}

void AHordePlayerState::AddPoints(int32 InPoints, EPointType PointsType)
{
	switch (PointsType)
	{
	case  EPointType::EPointCasual:
		ZedKills++;
		PlayerMoney = PlayerMoney + 150;
	break;

	case EPointType::EPointHeadShot:
		ZedKills++;
		HeadShots++;
		PlayerMoney = PlayerMoney + 250;
	break;

	default:
	break;
	}
	Points = InPoints + Points;

	ClientNotifyPoints(PointsType, InPoints);
}

void AHordePlayerState::ClientNotifyPoints_Implementation(EPointType PointType, int32 OutPoints)
{
	AHordeBaseController* PC = Cast<AHordeBaseController>(GetOwner());
	if (PC)
	{
		AHordeBaseHUD* HUD = Cast<AHordeBaseHUD>(PC->GetHUD());
		if (HUD)
		{
			HUD->OnPlayerPointsReceivedDelegate.Broadcast(PointType, OutPoints);
		}
	}
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
				GS->PopMessage(FHordeChatMessage("SERVER", FText::FromString(GetPlayerName() + " has joined.")));

			}
		}
	});

	GetWorld()->GetTimerManager().SetTimer(DelayedBeginPlay, DelayedBeginPlayDelegate, 2.f, false);
}
