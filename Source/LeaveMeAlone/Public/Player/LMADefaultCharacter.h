// LeaveMeAlone Game by Netologiya. All RightsReserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "LMADefaultCharacter.generated.h"

class UCameraComponent;

class USpringArmComponent;

class ULMAHealthComponent;

class UAnimMontage;

UCLASS()
class LEAVEMEALONE_API ALMADefaultCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALMADefaultCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UCameraComponent* CameraComponent;

	UPROPERTY()
	UDecalComponent* CurrentCursor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	UMaterialInterface* CursorMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cursor")
	FVector CursorSize = FVector(20.0f, 40.0f, 40.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (ClampMin = "100.0", ClampMax = "300.0"))
	float ZoomSpeed = 150.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (ClampMin = "300.0", ClampMax = "600.0"))
	float MinArmLength = 500.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera", meta = (ClampMin = "600.0", ClampMax = "3000.0"))
	float MaxArmLength = 2000.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components|Health")
	ULMAHealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, Category = "Animation")
	UAnimMontage* DeathMontage;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
	float YRotation = -75.0f;

	float ArmLength = 1400.0f;

	float FOV = 55.0f;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void CameraMove(float Value);

	void OnDeath();

	void RotationPlayerOnCursor();
};
