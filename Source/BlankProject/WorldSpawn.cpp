// Fill out your copyright notice in the Description page of Project Settings.

#include "BlankProject.h"
#include "WorldSpawn.h"

#include "VoxelMesh.h"

#include "StaticMeshResources.h"

UMaterialInterface* texture_tiles_mat;
AActor* world_spawn;
FNode* current_node = NULL;

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

UVoxelMesh* mesh;

// Called when the game starts or when spawned
void AWorldSpawn::BeginPlay() {
    Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BEGIN PLAY"));

	world_spawn = this;

	mesh = NewObject<UVoxelMesh>();
	mesh->gen(32, 32, 32);
}

float timer2 = 0;

// Called every frame
void AWorldSpawn::Tick(float dt) {
	Super::Tick(dt);

	mesh->update(dt);

	return;

	FCollisionQueryParams trace_params;
	//default player controller
	trace_params.AddIgnoredActor(GetWorld()->GetFirstPlayerController());
	//default pawn
	trace_params.AddIgnoredActor(GetWorld()->GetPawnIterator()->Get());

	FVector start_trace;
	FRotator cam_rota;
	GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(start_trace, cam_rota);

	FVector dir = cam_rota.Vector();
	FVector end_trace = start_trace + (dir * 2500.0f);

	FHitResult hit;

	GetWorld()->LineTraceSingle(hit, start_trace, end_trace, trace_params, FCollisionObjectQueryParams::DefaultObjectQueryParam);

	if (hit.Distance > 0) {
		float closest_dist = 99999;
		FNode* closest_node = NULL;

		for (int z = 0; z < mesh->get_sizez(); ++z) {
			for (int y = 0; y < mesh->get_sizey(); ++y) {
				for (int x = 0; x < mesh->get_sizex(); ++x) {
					FNode* node = mesh->get_node(x, y, z);
					if (!node || !node->tri || node->type == BLOCK_TYPE_AIR) continue;

					FVector center = (node->tri->v0.pos + node->tri->v1.pos + node->tri->v2.pos) / 3;
					FVector a = center - (dir * hit.Distance);
					float dist = FVector::Dist(a, start_trace);
					if (dist < closest_dist) {
						closest_dist = dist;
						closest_node = node;
					}
				}
			}
		}

		if (closest_node) {
			if (current_node && current_node->type != BLOCK_TYPE_AIR) current_node->type = BLOCK_TYPE_GRID;

			closest_node->type = BLOCK_TYPE_SELECTED;

			current_node = closest_node;
		}
	}
	if (current_node && GetWorld()->GetFirstPlayerController()->IsInputKeyDown(EKeys::LeftMouseButton)) {
		current_node->type = BLOCK_TYPE_AIR;
	}
}

