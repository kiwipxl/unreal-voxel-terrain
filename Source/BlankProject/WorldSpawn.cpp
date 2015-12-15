// Fill out your copyright notice in the Description page of Project Settings.

#include "BlankProject.h"
#include "WorldSpawn.h"

#include "VoxelMesh.h"

#include "StaticMeshResources.h"

UMaterialInterface* texture_tiles_mat;
AActor* world_spawn;

// Sets default values
AWorldSpawn::AWorldSpawn() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UMaterial> texture_tiles_mat_class(TEXT("Material'/Game/texture_tiles_mat.texture_tiles_mat'"));
	if (texture_tiles_mat_class.Object) {
		texture_tiles_mat = (UMaterial*)texture_tiles_mat_class.Object;
	}
	UE_LOG(LogTemp, Warning, TEXT("ayy lmao: %p"), texture_tiles_mat);
}

// Called when the game starts or when spawned
void AWorldSpawn::BeginPlay() {
    Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BEGIN PLAY"));

	world_spawn = this;

	UVoxelMesh* mesh = NewObject<UVoxelMesh>();
	mesh->gen(10, 10, 10);
}

// Called every frame
void AWorldSpawn::Tick(float dt) {
    Super::Tick(dt);
}

