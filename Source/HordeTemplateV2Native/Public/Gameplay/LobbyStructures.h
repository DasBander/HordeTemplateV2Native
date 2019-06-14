#pragma once
#include "Engine/DataTable.h"
#include "LobbyStructures.generated.h"

UENUM(BlueprintType)
enum class EGameStatus : uint8
{
	ELOBBY UMETA(DisplayName = "In-Lobby"),
	EINGAME UMETA(DisplayName = "In-Game"),
	EGAMEOVER UMETA(DisplayName = "Game Over"),
	ESERVERTRAVEL UMETA(DisplayName = "Server Travel")
};

UENUM(BlueprintType)
enum class EPointType : uint8
{
	EPointCasual UMETA(DisplayName = "Casual Kill"),
	EPointHeadShot UMETA(DisplayName = "Headshot")
};

USTRUCT(BlueprintType)
struct FChatMessage
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat")
		FString Sender = "SERVER";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
		FText Message;

	FChatMessage() {}

	FChatMessage(FString InSender, FText InMessage)
	{
		Sender = InSender;
		Message = InMessage;
	}

};

USTRUCT(BlueprintType)
struct FPlayerInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
		FString UserName = "None";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
		FString PlayerID = "None";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
		FName SelectedCharacter = "None";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Player Info")
		bool PlayerReady = false;

	FPlayerInfo() {}


};

USTRUCT(BlueprintType)
struct FLobbyAvailableCharacters : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Characters")
		FName CharacterID = "None";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Characters")
		FString PlayerID = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Characters")
		FString PlayerUsername = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Characters")
		bool CharacterTaken = false;

	FLobbyAvailableCharacters() {}


};

USTRUCT(BlueprintType)
struct FLobbyInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby Info")
		FString LobbyName = "None";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby Info")
		FString OwnerID = "None";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby Info")
		TArray<FName> LobbyMapRotation;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby Info")
		TArray<FLobbyAvailableCharacters> AvailableCharacters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby Info")
		int32 MinimumStartingPlayers = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Lobby Info")
		float DefaultLobbyTime = 300.f;

	FLobbyInfo() {}


};


USTRUCT(BlueprintType)
struct FPlayableLevel : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Playable Levels")
		 FText LevelName = FText();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Playable Levels")
		FName RawLevelName = "TestingMap";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Playable Levels")
		UTexture2D* LevelImage = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Playable Levels")
		UTexture2D* LobbyImage = nullptr;

	FPlayableLevel() {}


};
	
USTRUCT(BlueprintType)
struct FLobbyTrade
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Characters")
		FString Instigator = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Characters")
		FString Target = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Characters")
		float TimeLeft = 20;


	FLobbyTrade() {}

	FLobbyTrade(FString InInstigator, FString InTarget, float InTimeLeft) 
	{
		Instigator = InInstigator;
		Target = InTarget;
		TimeLeft = InTimeLeft;
	}

};

USTRUCT(BlueprintType)
struct FPlayerScore
{
	GENERATED_USTRUCT_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
		FString ScoreType = "None";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
		FString PlayerID = "";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
		int32 Score = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
		FName Character;

	FPlayerScore() {}

	FPlayerScore(FString InScoreType)
	{
		ScoreType = InScoreType;
	}

	FPlayerScore(FString InScoreType, FString InPlayerID, int32 InScore, FName InCharacter)
	{
		ScoreType = InScoreType;
		PlayerID = InPlayerID;
		Score = InScore;
		Character = InCharacter;
	}

	

};