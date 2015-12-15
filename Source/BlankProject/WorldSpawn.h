// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include "GameFramework/Actor.h"

#include "WorldSpawn.generated.h"

extern UMaterialInterface* texture_tiles_mat;
extern AActor* world_spawn;

UCLASS()
class BLANKPROJECT_API AWorldSpawn : public AActor
{
	GENERATED_BODY()
	
public:
	// Sets default values for this actor's properties
	AWorldSpawn();

    // Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
    virtual void Tick(float dt) override;
};
