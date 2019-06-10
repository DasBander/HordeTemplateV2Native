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
		UTexture2D* LevelImage;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Playable Levels")
		UTexture2D* LobbyImage;

	FPlayableLevel() {}


};
	
