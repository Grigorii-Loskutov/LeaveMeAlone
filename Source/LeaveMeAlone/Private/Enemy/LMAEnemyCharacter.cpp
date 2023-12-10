// LeaveMeAlone Game by Netologiya. All RightsReserved.


#include "Enemy/LMAEnemyCharacter.h"
#include "Components/LMAHealthComponent.h"

// Sets default values
ALMAEnemyCharacter::ALMAEnemyCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	HealthComponent = CreateDefaultSubobject<ULMAHealthComponent>("HealthComponent");
	IsRunning = false;
}

// Called when the game starts or when spawned
void ALMAEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	//this->TakeDamage(50.0f, FDamageEvent(), Controller, this);
	
}

// Called every frame
void ALMAEnemyCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
