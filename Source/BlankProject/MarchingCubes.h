// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include <vector>
#include "GeneratedMeshComponent.h"

class UVoxelMesh;

extern void gen_marching_cubes(UVoxelMesh* vmesh, TArray<FGeneratedMeshTriangle>* tris);