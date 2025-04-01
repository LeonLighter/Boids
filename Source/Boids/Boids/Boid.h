// Boid.h
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Boid.generated.h"

UCLASS()
class YOUR_PROJECT_API ABoid : public AActor
{
	GENERATED_BODY()
    
public:    
	ABoid();
    
	virtual void Tick(float DeltaTime) override;
    
	void UpdateVelocity(TArray<ABoid*>& NearbyBoids, float DeltaTime);
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Parameters")
	float MaxSpeed = 100.0f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Parameters")
	float MaxForce = 50.0f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Parameters")
	float NeighborRadius = 150.0f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Parameters")
	float SeparationWeight = 1.5f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Parameters")
	float AlignmentWeight = 1.0f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Parameters")
	float CohesionWeight = 1.0f;
    
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Boid Parameters")
	float ViewAngle = 120.0f; // Champ de vision en degrés
    
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* BoidMesh;
    
	// Getters
	FVector GetVelocity() const { return Velocity; }
    
protected:
	virtual void BeginPlay() override;
    
private:
	FVector Velocity;
    
	FVector Separation(TArray<ABoid*>& NearbyBoids);
	FVector Alignment(TArray<ABoid*>& NearbyBoids);
	FVector Cohesion(TArray<ABoid*>& NearbyBoids);
	FVector ObstacleAvoidance();
    
	FVector LimitVector(const FVector& Vector, float Max);
	bool IsInFieldOfView(const ABoid* OtherBoid) const;
};