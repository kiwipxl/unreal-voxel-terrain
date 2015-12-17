// Fill out your copyright notice in the Description page of Project Settings.

#include "BlankProject.h"
#include "VoxelMesh.h"

#include "FractalNoise.h"
#include "lodepng.h"
#include "MarchingCubes.h"
#include "WorldSpawn.h"

#include "StaticMeshResources.h"

FractalNoise fnoise;
UGeneratedMeshComponent* custom_mesh;

void UVoxelMesh::gen(int sizex_, int sizey_, int sizez_) {
	sizex = sizex_;
	sizey = sizey_;
	sizez = sizez_;

	nodes.clear();
	fnoise.init();

	for (int z = 0; z < sizez; ++z) {
		for (int y = 0; y < sizey; ++y) {
			for (int x = 0; x < sizex; ++x) {
				FNode* node = new FNode();
				node->x = x;
				node->y = y;
				node->z = z;
				nodes.push_back(node);

				float height = fnoise.noise(x / (float)sizex, y / (float)sizey, z / (float)sizez);
				if (height > 0) node->type = BLOCK_TYPE_GRID;
				else node->type = BLOCK_TYPE_AIR;

				//node->type = BLOCK_TYPE_GRID;
				if (x <= 0 || y <= 0 || z <= 0 || x >= sizex - 1 || y >= sizey - 1 || z >= sizez - 1) node->type = BLOCK_TYPE_AIR;
			}
		}
	}

	gen_marching_cubes(this, &tris);

	custom_mesh = NewObject<UGeneratedMeshComponent>(world_spawn);
	custom_mesh->RegisterComponent();
	world_spawn->SetActorEnableCollision(true);

	custom_mesh->SetGeneratedMeshTriangles(tris);
	custom_mesh->SetMaterial(0, texture_tiles_mat);

	UE_LOG(LogTemp, Warning, TEXT("spawned"));
}

FNode* UVoxelMesh::get_node(int x, int y, int z) {
    int index = ((sizex * sizey) * z) + (y * sizey) + x;
	if (index < 0 || index >= nodes.size()) return NULL;
    return nodes[index];
}

float timer = 0;
float offsetx = 0;
float total_time = 0;

void UVoxelMesh::update(float dt) {
	timer += dt;
	total_time += dt;

	if (timer >= 1) {
		timer = 0;
	}

	gen_marching_cubes(this, &tris);
	custom_mesh->SetGeneratedMeshTriangles(tris);
}