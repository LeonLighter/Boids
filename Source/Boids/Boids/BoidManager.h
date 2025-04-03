// BoidManager.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boid.h"
#include "BoidManager.generated.h"

// Structure pour représenter une cellule dans notre grille spatiale
USTRUCT()
struct FBoidGrid
{
	GENERATED_BODY()
    
	TArray<ABoid*> Boids;
};

UCLASS()
class BOIDS_API ABoidManager : public AActor
{
	GENERATED_BODY()
    
public:    
	ABoidManager();
    
	virtual void Tick(float DeltaTime) override;
    
	UPROPERTY(EditAnywhere, Category = "Boid Simulation")
	TSubclassOf<ABoid> BoidClass;
    
	UPROPERTY(EditAnywhere, Category = "Boid Simulation")
	int32 NumBoids = 100;
    
	UPROPERTY(EditAnywhere, Category = "Boid Simulation")
	FVector SpawnVolume = FVector(1000.0f, 1000.0f, 500.0f);
    
	UPROPERTY(EditAnywhere, Category = "Boid Simulation")
	bool bEnableSpatialPartitioning = true;
    
	UPROPERTY(EditAnywhere, Category = "Boid Simulation", meta = (EditCondition = "bEnableSpatialPartitioning"))
	float GridCellSize = 200.0f;
    
protected:
	virtual void BeginPlay() override;
    
private:
	TArray<ABoid*> AllBoids;
    
	// Grille spatiale pour l'optimisation
	TMap<FIntVector, FBoidGrid> SpatialGrid;
    
	// Méthodes
	void SpawnBoids();
	void UpdateBoids(float DeltaTime);
    
	// Méthodes d'optimisation spatiale
	FIntVector GetGridCellForLocation(const FVector& Location) const;
	void UpdateSpatialGrid();
	TArray<ABoid*> GetNearbyBoids(ABoid* Boid);
};