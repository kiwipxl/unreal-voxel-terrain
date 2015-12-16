// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>

#include "UObject.h"
#include "GeneratedMeshComponent.h"

#include "VoxelMesh.generated.h"

enum BlockType {

	BLOCK_TYPE_AIR, 
	BLOCK_TYPE_GRASS,
	BLOCK_TYPE_DIRT, 
	BLOCK_TYPE_WATER
};

struct FNode {

    int x;
	int y;
	int z;
	BlockType type = BLOCK_TYPE_AIR;
	FGeneratedMeshTriangle* tri;
};

UCLASS()
class UVoxelMesh : public UObject
{
	GENERATED_BODY()
	
public:
	void gen(int sizex_, int sizey_, int sizez_);

	int get_sizex() { return sizex; }
	int get_sizey() { return sizey; }
	int get_sizez() { return sizez; }

	FNode* get_node(int x, int y, int z);

	void update(float dt);

private:
	std::vector<FNode*> nodes;
	TArray<FGeneratedMeshTriangle> tris;

	float sizex;
	float sizey;
	float sizez;

	BlockType get_type(int x, int y, int z);
	float get_height(int x, int y, int z);

    void gen_node(int x, int y, int z);
	void del_node(int x, int y, int z);
};
