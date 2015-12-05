// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "BlankProject.h"
#include "Components/MeshComponent.h"
#include "GeneratedMeshComponent.generated.h"

USTRUCT()
struct FGeneratedVertexAttribs
{
	GENERATED_USTRUCT_BODY()

	FGeneratedVertexAttribs() { }

	UPROPERTY()
	FVector pos;
	UPROPERTY()
	FVector2D uv;
	UPROPERTY()
	FColor colour;
};

USTRUCT()
struct FGeneratedMeshTriangle
{
	GENERATED_USTRUCT_BODY()

	FGeneratedMeshTriangle() { }

	UPROPERTY()
	FGeneratedVertexAttribs v0 = { };
	UPROPERTY()
	FGeneratedVertexAttribs v1 = { };
	UPROPERTY()
	FGeneratedVertexAttribs v2 = { };
};

/** Component that allows you to specify generated triangle mesh geometry */
UCLASS()
class UGeneratedMeshComponent : public UMeshComponent
{
	GENERATED_UCLASS_BODY()

public:

	///** Set the geometry to use on this triangle mesh */
	UFUNCTION(BlueprintCallable, Category = "Components|GeneratedMesh")
	bool SetGeneratedMeshTriangles(const TArray<FGeneratedMeshTriangle>& Triangles);

	///** Add to the geometry to use on this triangle mesh.  This may cause an allocation.  Use SetGeneratedMeshTriangles() instead when possible to reduce allocations. */
	UFUNCTION(BlueprintCallable, Category = "Components|GeneratedMesh")
	void AddGeneratedMeshTriangles(const TArray<FGeneratedMeshTriangle>& Triangles);

	///** Removes all geometry from this triangle mesh.  Does not deallocate memory, allowing new geometry to reuse the existing allocation. */
	UFUNCTION(BlueprintCallable, Category = "Components|GeneratedMesh")
	void ClearGeneratedMeshTriangles();

private:

	// Begin UPrimitiveComponent interface.
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
	// End UPrimitiveComponent interface.

	// Begin UMeshComponent interface.
	virtual int32 GetNumMaterials() const override;
	// End UMeshComponent interface.

	// Begin USceneComponent interface.
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;
	// Begin USceneComponent interface.

	/** */
	TArray<FGeneratedMeshTriangle> GeneratedMeshTris;

	friend class FGeneratedMeshSceneProxy;
};