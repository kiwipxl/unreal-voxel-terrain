// Fill out your copyright notice in the Description page of Project Settings.

#include "BlankProject.h"
#include "WorldSpawn.h"

#include "FractalNoise.h"
#include "lodepng.h"
#include "GeneratedMeshComponent.h"
#include "StaticMeshResources.h"

TSubclassOf<AActor> hex_asset;
UMaterialInterface* grass_mat_asset;
UMaterialInterface* water_mat_asset;
float timer = 0;
FractalNoise fnoise;
TArray<FGeneratedMeshTriangle> chunk_triangles;
FPositionVertexBuffer* vertex_buffer;
FIndexArrayView index_buffer;
UGeneratedMeshComponent* custom_mesh;

// Sets default values
AWorldSpawn::AWorldSpawn() {
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UBlueprint> hex_asset_class(TEXT("Blueprint'/Game/Hexagon_Blueprint.Hexagon_Blueprint'"));
    if (hex_asset_class.Object) {
        hex_asset = (UClass*)hex_asset_class.Object->GeneratedClass;
	}

    static ConstructorHelpers::FObjectFinder<UMaterial> grass_mat_asset_class(TEXT("Material'/Game/hex_grass_material.hex_grass_material'"));
    if (grass_mat_asset_class.Object) {
        grass_mat_asset = (UMaterial*)grass_mat_asset_class.Object;
    }

    static ConstructorHelpers::FObjectFinder<UMaterial> water_mat_asset_class(TEXT("Material'/Game/hex_water_material.hex_water_material'"));
    if (water_mat_asset_class.Object) {
        water_mat_asset = (UMaterial*)water_mat_asset_class.Object;
    }
}

// Called when the game starts or when spawned
void AWorldSpawn::BeginPlay() {
    Super::BeginPlay();

	UE_LOG(LogTemp, Warning, TEXT("BEGIN PLAY"));

	hex_list.clear();
	chunk_triangles.Empty();

	AActor* new_hex_obj = GetWorld()->SpawnActor<AActor>(hex_asset, FVector::ZeroVector, FRotator(0, -90, 0));
	UStaticMeshComponent* mesh_component = Cast<UStaticMeshComponent>(new_hex_obj->GetComponentByClass(UStaticMeshComponent::StaticClass()));
	vertex_buffer = &mesh_component->StaticMesh->RenderData->LODResources[0].PositionVertexBuffer;
	index_buffer = mesh_component->StaticMesh->RenderData->LODResources[0].IndexBuffer.GetArrayView();
	new_hex_obj->Destroy();

	SetActorRotation(FRotator(0, -90, 0));

	UE_LOG(LogTemp, Warning, TEXT("num_vertices: %d"), vertex_buffer->GetNumVertices());

    fnoise.init();
	gen_chunk();
}

Hex* AWorldSpawn::get_hex(int x, int y, int z) {
    int index = ((SIZEX * SIZEY) * z) + (y * SIZEY) + x;
    if (index < 0 || index >= hex_list.size()) return NULL;
    return hex_list[index];
}

float AWorldSpawn::get_height(int x, int y, int z) {
	float t_height = fnoise.noise(x / (float)SIZEX, y / (float)SIZEY, z / (float)SIZEZ);
	t_height -= z / (float)SIZEZ;
	return t_height;
}

BlockType AWorldSpawn::get_type(int x, int y, int z) {
	float t_height = get_height(x, y, z);
	BlockType type = BLOCK_TYPE_AIR;
	if (t_height > 0 || z == 0) type = BLOCK_TYPE_DIRT;
	return type;
}

void AWorldSpawn::gen_hex(int x, int y, int z) {
    Hex* hex = get_hex(x, y, z);
	if (hex) hex->type = get_type(x, y, z);
}

FVector corner_points[] = { FVector(1, 0, 1), FVector(1, 1, 1), FVector(0, 1, 1), FVector(0, 0, 1), 
							FVector(1, 0, 0), FVector(1, 1, 0), FVector(0, 1, 0), FVector(0, 0, 0), };

/*
Linearly interpolate the position where an isosurface cuts
an edge between two vertices, each with their own scalar value
*/
FVector vertex_interp(FVector p1, FVector p2, bool valp1, bool valp2)
{
	if (!valp1) return p1;
	if (!valp2) return p2;
	return p1;
}

void AWorldSpawn::gen_chunk() {
    if (hex_asset) {
        for (int z = 0; z < SIZEZ; ++z) {
            for (int y = 0; y < SIZEY; ++y) {
                for (int x = 0; x < SIZEX; ++x) {
                    Hex* hex = new Hex();
                    hex->x = x;
                    hex->y = y;
                    hex->z = z;
                    hex_list.push_back(hex);

                    gen_hex(x, y, z);
                }
            }
        }

		for (int z = -1; z < SIZEZ; ++z) {
			for (int y = -1; y < SIZEY; ++y) {
				for (int x = -1; x < SIZEX; ++x) {
					bool corner0 = (get_hex(x + 1, y, z + 1) != NULL && get_hex(x + 1, y, z + 1)->type != BLOCK_TYPE_AIR);
					bool corner1 = (get_hex(x + 1, y + 1, z + 1) != NULL && get_hex(x + 1, y + 1, z + 1)->type != BLOCK_TYPE_AIR);
					bool corner2 = (get_hex(x, y + 1, z + 1) != NULL && get_hex(x, y + 1, z + 1)->type != BLOCK_TYPE_AIR);
					bool corner3 = (get_hex(x, y, z + 1) != NULL && get_hex(x, y, z + 1)->type != BLOCK_TYPE_AIR);

					bool corner4 = (get_hex(x + 1, y, z) != NULL && get_hex(x + 1, y, z)->type != BLOCK_TYPE_AIR);
					bool corner5 = (get_hex(x + 1, y + 1, z) != NULL && get_hex(x + 1, y + 1, z)->type != BLOCK_TYPE_AIR);
					bool corner6 = (get_hex(x, y + 1, z) != NULL && get_hex(x, y + 1, z)->type != BLOCK_TYPE_AIR);
					bool corner7 = (get_hex(x, y, z) != NULL && get_hex(x, y, z)->type != BLOCK_TYPE_AIR);

					//UE_LOG(LogTemp, Warning, TEXT("(%d, %d, %d): %d, %d, %d, %d, %d, %d, %d, %d"), x, y, z, corner0, corner1, corner2, corner3, corner4, corner5, corner6, corner7);

					FVector vertlist[12];
					//bool isolevel = corner0 && corner1 && corner2 && corner3 && corner4 && corner5 && corner6 && corner7;
					//isolevel = false;
					float isolevel = 1;

					int cubeindex = 0;
					if (corner0) cubeindex |= 1;
					if (corner1) cubeindex |= 2;
					if (corner2) cubeindex |= 4;
					if (corner3) cubeindex |= 8;
					if (corner4) cubeindex |= 16;
					if (corner5) cubeindex |= 32;
					if (corner6) cubeindex |= 64;
					if (corner7) cubeindex |= 128;

					//UE_LOG(LogTemp, Warning, TEXT("%d, %d"), cubeindex, isolevel);

					/* Cube is entirely in/out of the surface */
					if (edgeTable[cubeindex] == 0) continue;

					/* Find the vertices where the surface intersects the cube */
					if (edgeTable[cubeindex] & 1)
						vertlist[0] = vertex_interp(corner_points[0], corner_points[1], corner0, corner1);
					if (edgeTable[cubeindex] & 2)
						vertlist[1] = vertex_interp(corner_points[1], corner_points[2], corner1, corner2);
					if (edgeTable[cubeindex] & 4)
						vertlist[2] = vertex_interp(corner_points[2], corner_points[3], corner2, corner3);
					if (edgeTable[cubeindex] & 8)
						vertlist[3] = vertex_interp(corner_points[3], corner_points[0], corner3, corner0);
					if (edgeTable[cubeindex] & 16)
						vertlist[4] = vertex_interp(corner_points[4], corner_points[5], corner4, corner5);
					if (edgeTable[cubeindex] & 32)
						vertlist[5] = vertex_interp(corner_points[5], corner_points[6], corner5, corner6);
					if (edgeTable[cubeindex] & 64)
						vertlist[6] = vertex_interp(corner_points[6], corner_points[7], corner6, corner7);
					if (edgeTable[cubeindex] & 128)
						vertlist[7] = vertex_interp(corner_points[7], corner_points[4], corner7, corner4);
					if (edgeTable[cubeindex] & 256)
						vertlist[8] = vertex_interp(corner_points[0], corner_points[4], corner0, corner4);
					if (edgeTable[cubeindex] & 512)
						vertlist[9] = vertex_interp(corner_points[1], corner_points[5], corner1, corner5);
					if (edgeTable[cubeindex] & 1024)
						vertlist[10] = vertex_interp(corner_points[2], corner_points[6], corner2, corner6);
					if (edgeTable[cubeindex] & 2048)
						vertlist[11] = vertex_interp(corner_points[3], corner_points[7], corner3, corner7);

					/* Create the triangle */
					int ntriang = 0;
					FGeneratedMeshTriangle tri;
					for (int i = 0; triTable[cubeindex][i] != -1; i += 3) {
						tri.set_vertex(vertlist[triTable[cubeindex][i]] * 100.0f, 2);
						tri.set_vertex(vertlist[triTable[cubeindex][i + 1]] * 100.0f, 1);
						tri.set_vertex(vertlist[triTable[cubeindex][i + 2]] * 100.0f, 0);

						tri.Vertex0.X += x * 100.0f;
						tri.Vertex1.X += x * 100.0f;
						tri.Vertex2.X += x * 100.0f;

						tri.Vertex0.Y += y * 100.0f;
						tri.Vertex1.Y += y * 100.0f;
						tri.Vertex2.Y += y * 100.0f;

						tri.Vertex0.Z += z * 100.0f;
						tri.Vertex1.Z += z * 100.0f;
						tri.Vertex2.Z += z * 100.0f;

						chunk_triangles.Add(tri);

						//UE_LOG(LogTemp, Warning, TEXT("--tri--"));
						//UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), tri.Vertex0.X, tri.Vertex0.Y, tri.Vertex0.Z);
						//UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), tri.Vertex1.X, tri.Vertex1.Y, tri.Vertex1.Z);
						//UE_LOG(LogTemp, Warning, TEXT("%f, %f, %f"), tri.Vertex2.X, tri.Vertex2.Y, tri.Vertex2.Z);

						ntriang++;
					}

				}
			}
		}

		custom_mesh = NewObject<UGeneratedMeshComponent>(this);
		custom_mesh->RegisterComponent();

		custom_mesh->SetGeneratedMeshTriangles(chunk_triangles);
		custom_mesh->SetMaterial(0, water_mat_asset);

        UE_LOG(LogTemp, Warning, TEXT("spawned"));
    }else {
        UE_LOG(LogTemp, Warning, TEXT("Could not find hexagon asset"));
    }
}

// Called every frame
void AWorldSpawn::Tick(float dt) {
    Super::Tick(dt);

    timer += dt;

    if (timer >= 2) {
		timer = -10000;
    }
}

