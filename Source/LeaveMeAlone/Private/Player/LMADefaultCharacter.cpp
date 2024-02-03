// LeaveMeAlone Game by Netologiya. All RightsReserved.

#include "Player/LMADefaultCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/DecalComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/LMAHealthComponent.h"
#include "Components/LMAWeaponComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/Engine.h"

// Sets default values
ALMADefaultCharacter::ALMADefaultCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArmComponent->SetupAttachment(GetRootComponent());
	SpringArmComponent->SetUsingAbsoluteRotation(true);
	SpringArmComponent->TargetArmLength = ArmLength;
	SpringArmComponent->SetRelativeRotation(FRotator(YRotation, 0.0f, 0.0f));
	SpringArmComponent->bDoCollisionTest = false;
	SpringArmComponent->bEnableCameraLag = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>("CameraComponent");
	CameraComponent->SetupAttachment(SpringArmComponent);
	CameraComponent->SetFieldOfView(FOV);
	CameraComponent->bUsePawnControlRotation = false;

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");

	// Спринт
	bIsSprinting = false;

	SprintSpeed = 600;

	DefaultWalkSpeed = 300;

	MaxStamina = 100;

	CurrentStamina = MaxStamina;

	StaminaCostPerSecond = 10;

	StaminaRestorePerSecond = 10;

	// Оружие
	WeaponComponent = CreateDefaultSubobject<ULMAWeaponComponent>("Weapon");

	IsDead = false;
}

// Called when the game starts or when spawned
void ALMADefaultCharacter::BeginPlay()
{
	Super::BeginPlay();
	if (CursorMaterial)
	{
		CurrentCursor = UGameplayStatics::SpawnDecalAtLocation(GetWorld(), CursorMaterial, CursorSize, FVector(0));
	}

	OnHealthChanged(HealthComponent->GetHealth());
	HealthComponent->OnDeath.AddUObject(this, &ALMADefaultCharacter::OnDeath);
	HealthComponent->OnHealthChanged.AddUObject(this, &ALMADefaultCharacter::OnHealthChanged);
}

// Called every frame
void ALMADefaultCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		FHitResult ResultHit;
		PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
		if (CurrentCursor)
		{
			CurrentCursor->SetWorldLocation(ResultHit.Location);
		} /*
		if (bIsSprinting)
		{
			ConsumeStamina(StaminaCostPerSecond * DeltaTime);
		}
		else
		{
			// Вызов метода для восстановления выносливости после окончания спринта
			RestoreStamina(StaminaRestorePerSecond * DeltaTime);
		}*/
	}
}

// Called to bind functionality to input
void ALMADefaultCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ALMADefaultCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ALMADefaultCharacter::MoveRight);
	PlayerInputComponent->BindAxis("CameraMove", this, &ALMADefaultCharacter::CameraMove);

	// Приявяжем спринт
	PlayerInputComponent->BindAction("Sprint", IE_Pressed, this, &ALMADefaultCharacter::StartSprinting);
	PlayerInputComponent->BindAction("Sprint", IE_Released, this, &ALMADefaultCharacter::StopSprinting);

	// Привяжем оружие
	PlayerInputComponent->BindAction("Fire", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::StartFire); // Нажатие - начало стрельбы
	PlayerInputComponent->BindAction("Fire", IE_Released, WeaponComponent, &ULMAWeaponComponent::StopFire); // ОТпускание - завершение стрельббы
	PlayerInputComponent->BindAction("Reload", IE_Pressed, WeaponComponent, &ULMAWeaponComponent::Reload);
}

void ALMADefaultCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector(), Value);
}

void ALMADefaultCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector(), Value);
	// UE_LOG(LogTemp, Display, TEXT("MouseWheel: %f"), Value);
}

void ALMADefaultCharacter::CameraMove(float Value)
{

	float NewArmLength = SpringArmComponent->TargetArmLength - (Value * ZoomSpeed);
	NewArmLength = FMath::Clamp(NewArmLength, MinArmLength, MaxArmLength);

	SpringArmComponent->TargetArmLength = NewArmLength;
	// UE_LOG(LogTemp, Display, TEXT("MouseWheel: %f"), Value);
}

void ALMADefaultCharacter::OnDeath()
{
	IsDead = true;
	CurrentCursor->DestroyRenderState_Concurrent();

	PlayAnimMontage(DeathMontage);

	GetCharacterMovement()->DisableMovement();

	SetLifeSpan(5.0f);

	float		 DelayTime = 0.9f; // Задержка в секундах
	FTimerHandle SpectatingTimerHandle;

	GetWorldTimerManager().SetTimer(
		SpectatingTimerHandle, [this]() {
			if (Controller)
			{
				Controller->ChangeState(NAME_Spectating);
			}
		},
		DelayTime, false);

	/*
	if (Controller)
	{
		Controller->ChangeState(NAME_Spectating);
	}
	*/
}

void ALMADefaultCharacter::OnHealthChanged(float NewHealth)
{

	//GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Health = %f"), NewHealth));
}

void ALMADefaultCharacter::RotationPlayerOnCursor()
{
	APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
	if (PC)
	{
		FHitResult ResultHit;
		PC->GetHitResultUnderCursor(ECC_GameTraceChannel1, true, ResultHit);
		float FindRotatorResultYaw = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), ResultHit.Location).Yaw;
		SetActorRotation(FQuat(FRotator(0.0f, FindRotatorResultYaw, 0.0f)));
		if (CurrentCursor)
		{
			CurrentCursor->SetWorldLocation(ResultHit.Location);
		}
	}
}

void ALMADefaultCharacter::StartSprinting()
{
	if (CurrentStamina > StaminaCostPerSecond)
	{
		bIsSprinting = true;
		GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;
		GetWorldTimerManager().SetTimer(StaminaDecriaseTimer, this, &ALMADefaultCharacter::ConsumeStamina, 1.0f, true);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Sprint = %d"), bIsSprinting));
	}
	else
	{
		StopSprinting();
	}
}

void ALMADefaultCharacter::StopSprinting()
{
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
	GetWorldTimerManager().SetTimer(StaminaIncreaseTimer, this, &ALMADefaultCharacter::RestoreStamina, 1.0f, true);
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Sprint = %d"), bIsSprinting));
	GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, FString::Printf(TEXT("Speed = %f"), GetCharacterMovement()->MaxWalkSpeed));
}

void ALMADefaultCharacter::ConsumeStamina()
{
	if (bIsSprinting && CurrentStamina > 0)
	{
		CurrentStamina -= StaminaCostPerSecond;
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Yellow, FString::Printf(TEXT("Stamina = %f"), CurrentStamina));
	}
	else
	{
		StopSprinting();
		GetWorldTimerManager().ClearTimer(StaminaDecriaseTimer);
	}
}

void ALMADefaultCharacter::RestoreStamina()
{
	if (!bIsSprinting && CurrentStamina < MaxStamina)
	{
		CurrentStamina = FMath::Min(CurrentStamina + StaminaCostPerSecond, MaxStamina);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, FString::Printf(TEXT("Stamina = %f"), CurrentStamina));
	}
	else
	{
		GetWorldTimerManager().ClearTimer(StaminaIncreaseTimer);
	}
}