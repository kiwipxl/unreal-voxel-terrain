// Copyright 1998-2014 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "BlankProject.h"
#include "Components/MeshComponent.h"
#include "GeneratedMeshComponent.generated.h"

USTRUCT(BlueprintType)
struct FGeneratedMeshTriangle
{
	GENERATED_USTRUCT_BODY()

	FGeneratedMeshTriangle() { }
	
	FGeneratedMeshTriangle(FVector v1, FVector v2, FVector v3) : Vertex0(v1), Vertex1(v2), Vertex2(v3) { }

	void set_vertex(FVector v, int index) {
		if (index == 0) Vertex0 = v;
		else if (index == 1) Vertex1 = v;
		else if (index == 2) Vertex2 = v;
	}

	UPROPERTY(EditAnywhere, Category = Triangle)
	FVector Vertex0;

	UPROPERTY(EditAnywhere, Category = Triangle)
	FVector Vertex1;

	UPROPERTY(EditAnywhere, Category = Triangle)
	FVector Vertex2;
};

/** Component that allows you to specify generated triangle mesh geometry */
UCLASS(hidecategories = (Object, LOD, Physics, Collision), editinlinenew, meta = (BlueprintSpawnableComponent), ClassGroup = Rendering)
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