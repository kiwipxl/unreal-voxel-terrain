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

UVoxelMesh* mesh;

// Called when the game starts or when spawned
void AWorldSpawn::BeginPlay() {
    Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BEGIN PLAY"));

	world_spawn = this;

	mesh = NewObject<UVoxelMesh>();
	mesh->gen(32, 32, 32);
}

bool same_side(FVector p1, FVector p2, FVector a, FVector b) {
	FVector cp1 = FVector::CrossProduct(b - a, p1 - a);
	FVector cp2 = FVector::CrossProduct(b - a, p2 - a);
	return FVector::DotProduct(cp1, cp2) >= 0;
}

bool point_in_triangle(FVector p, FVector a, FVector b, FVector c) {
	if (same_side(p, a, b, c) && same_side(p, b, a, c) && same_side(p, c, a, b)) {
		FVector vc1 = FVector::CrossProduct(a - b, a - c);
		if (abs(FVector::DotProduct(a - p, vc1)) <= .01f) return true;
	}
	return false;
}

float timer2 = 0;

// Called every frame
void AWorldSpawn::Tick(float dt) {
	Super::Tick(dt);

	mesh->update(dt);

	timer2 += dt;
	if (timer2 >= 1) {
		timer2 = 0;

		FCollisionQueryParams trace_params;
		//default player controller
		trace_params.AddIgnoredActor(GetWorld()->GetFirstPlayerController());
		//default pawn
		trace_params.AddIgnoredActor(GetWorld()->GetPawnIterator()->Get());

		FVector start_trace;
		FRotator cam_rota;
		GetWorld()->GetFirstPlayerController()->GetPlayerViewPoint(start_trace, cam_rota);

		FVector dir = cam_rota.Vector();
		FVector len_dir = dir * 1000.0f;
		FVector end_trace = start_trace + len_dir;

		FHitResult hit;

		GetWorld()->LineTraceSingle(hit, start_trace, end_trace, trace_params, FCollisionObjectQueryParams::DefaultObjectQueryParam);

		if (hit.bBlockingHit) {
			float closest_dist = 99999;
			FNode* closest_node = NULL;

			for (int z = 0; z < mesh->get_sizez(); ++z) {
				for (int y = 0; y < mesh->get_sizey(); ++y) {
					for (int x = 0; x < mesh->get_sizex(); ++x) {
						FNode* node = mesh->get_node(x, y, z);
						if (!node || !node->tri) continue;

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
				closest_node->type = BLOCK_TYPE_SELECTED;
			}
		}
		//DrawDebugLine(GetWorld(), start_trace, end_trace, FColor(0, 255, 0), true, -1.0f, 0, 4);
	}
}

