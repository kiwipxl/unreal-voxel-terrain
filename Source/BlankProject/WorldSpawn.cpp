// Fill out your copyright notice in the Description page of Project Settings.

#include "BlankProject.h"
#include "WorldSpawn.h"

#include "FractalNoise.h"
#include "lodepng.h"
#include "GeneratedMeshComponent.h"
#include "StaticMeshResources.h"

UMaterialInterface* texture_tiles_mat;
FractalNoise fnoise;
TArray<FGeneratedMeshTriangle> chunk_triangles;
UGeneratedMeshComponent* custom_mesh;

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

	hex_list.clear();
	chunk_triangles.Empty();

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

	//fill all boundaries of chunk with air
	if (x <= 0 || y <= 0 || z <= 0 || x >= SIZEX - 1 || y >= SIZEY - 1 || z >= SIZEZ - 1) return type;

	if (t_height > 0 || z == 0) type = BLOCK_TYPE_DIRT;
	if (z <= 10) type = BLOCK_TYPE_GRASS;

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
	return p1 + (p2 - p1) / 2.0f;
}

FVector2D uv_interp(FVector v1, FVector v2, FVector v3, FVector p) {
	FVector f1 = v1 - p;
	FVector f2 = v2 - p;
	FVector f3 = v3 - p;

	float a = ((v1 - v2) ^ (v1 - v3)).Size();
	float a1 = (f2 ^ f3).Size() / a;
	float a2 = (f3 ^ f1).Size() / a;
	float a3 = (f1 ^ f2).Size() / a;

	return FVector2D(0, 0) * a1 + FVector2D(1.0f, 0) * a2 + FVector2D(0.0f, 1.0f) * a3;
}

void AWorldSpawn::gen_chunk() {
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
				FGeneratedMeshTriangle tri;
				FVector world_offset(x * 100.0f, y * 100.0f, z * 100.0f);

				for (int i = 0; triTable[cubeindex][i] != -1; i += 3) {
					/*const int NUM_TILESX = 2;
					const int NUM_TILESY = 1;
					const float TILE_SIZEX = 1.0f / NUM_TILESX;
					const float TILE_SIZEY = 1.0f / NUM_TILESY;

					float offsetx = 0, offsety = 0;
					Hex* hex = get_hex(x, y, z);
					if (!hex) continue;

					BlockType type = hex->type;
					if (type == BLOCK_TYPE_DIRT) offsetx = 0;
					else if (type == BLOCK_TYPE_GRASS) offsetx = TILE_SIZEX;*/

					FVector v0 = vertlist[triTable[cubeindex][i]];
					FVector v1 = vertlist[triTable[cubeindex][i + 1]];
					FVector v2 = vertlist[triTable[cubeindex][i + 2]];

					tri.v2.pos = (v0 * 100.0f) + world_offset;
					tri.v1.pos = (v1 * 100.0f) + world_offset;
					tri.v0.pos = (v2 * 100.0f) + world_offset;

					chunk_triangles.Add(tri);
				}

			}
		}
	}

	custom_mesh = NewObject<UGeneratedMeshComponent>(this);
	custom_mesh->RegisterComponent();

	custom_mesh->SetGeneratedMeshTriangles(chunk_triangles);
	custom_mesh->SetMaterial(0, texture_tiles_mat);

    UE_LOG(LogTemp, Warning, TEXT("spawned"));
}

// Called every frame
void AWorldSpawn::Tick(float dt) {
    Super::Tick(dt);
}

