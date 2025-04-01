// BoidManager.cpp
#include "BoidManager.h"
#include "Kismet/GameplayStatics.h"

ABoidManager::ABoidManager()
{
    PrimaryActorTick.bCanEverTick = true;
}

void ABoidManager::BeginPlay()
{
    Super::BeginPlay();
    
    SpawnBoids();
}

void ABoidManager::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    if (bEnableSpatialPartitioning)
    {
        // Mettre à jour la grille spatiale
        UpdateSpatialGrid();
    }
    
    // Mettre à jour tous les boids
    UpdateBoids(DeltaTime);
}

void ABoidManager::SpawnBoids()
{
    if (!BoidClass)
    {
        UE_LOG(LogTemp, Error, TEXT("BoidClass non définie dans BoidManager!"));
        return;
    }
    
    UWorld* World = GetWorld();
    if (!World)
        return;
    
    // Vider tout boid précédemment créé
    for (ABoid* Boid : AllBoids)
    {
        if (Boid)
        {
            Boid->Destroy();
        }
    }
    AllBoids.Empty();
    
    // Créer les nouveaux boids
    FActorSpawnParameters SpawnParams;
    SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
    
    for (int32 i = 0; i < NumBoids; ++i)
    {
        // Position aléatoire dans le volume de spawn
        FVector SpawnLocation = GetActorLocation() + FVector(
            FMath::RandRange(-SpawnVolume.X/2, SpawnVolume.X/2),
            FMath::RandRange(-SpawnVolume.Y/2, SpawnVolume.Y/2),
            FMath::RandRange(-SpawnVolume.Z/2, SpawnVolume.Z/2)
        );
        
        // Rotation aléatoire
        FRotator SpawnRotation = FRotator(
            FMath::RandRange(0.0f, 360.0f),
            FMath::RandRange(0.0f, 360.0f),
            FMath::RandRange(0.0f, 360.0f)
        );
        
        // Spawn du boid
        ABoid* NewBoid = World->SpawnActor<ABoid>(BoidClass, SpawnLocation, SpawnRotation, SpawnParams);
        if (NewBoid)
        {
            AllBoids.Add(NewBoid);
        }
    }
    
    UE_LOG(LogTemp, Log, TEXT("BoidManager: %d boids spawned"), AllBoids.Num());
}

void ABoidManager::UpdateBoids(float DeltaTime)
{
    // Mettre à jour chaque boid
    for (ABoid* Boid : AllBoids)
    {
        if (!Boid)
            continue;
            
        // Obtenir les boids voisins (optimisé par la grille spatiale si activée)
        TArray<ABoid*> NearbyBoids = GetNearbyBoids(Boid);
        
        // Mettre à jour la vélocité du boid en fonction des règles
        Boid->UpdateVelocity(NearbyBoids, DeltaTime);
    }
}

FIntVector ABoidManager::GetGridCellForLocation(const FVector& Location) const
{
    return FIntVector(
        FMath::Floor(Location.X / GridCellSize),
        FMath::Floor(Location.Y / GridCellSize),
        FMath::Floor(Location.Z / GridCellSize)
    );
}

void ABoidManager::UpdateSpatialGrid()
{
    // Vider la grille
    SpatialGrid.Empty();
    
    // Placer chaque boid dans sa cellule appropriée
    for (ABoid* Boid : AllBoids)
    {
        if (!Boid)
            continue;
            
        FIntVector Cell = GetGridCellForLocation(Boid->GetActorLocation());
        
        if (!SpatialGrid.Contains(Cell))
        {
            SpatialGrid.Add(Cell, FBoidGrid());
        }
        
        SpatialGrid[Cell].Boids.Add(Boid);
    }
}

TArray<ABoid*> ABoidManager::GetNearbyBoids(ABoid* Boid)
{
    TArray<ABoid*> NearbyBoids;
    
    if (!Boid)
        return NearbyBoids;
        
    if (bEnableSpatialPartitioning)
    {
        // Obtenir la cellule du boid
        FIntVector BoidCell = GetGridCellForLocation(Boid->GetActorLocation());
        
        // Chercher dans la cellule actuelle et les cellules adjacentes
        for (int x = -1; x <= 1; ++x)
        {
            for (int y = -1; y <= 1; ++y)
            {
                for (int z = -1; z <= 1; ++z)
                {
                    FIntVector NeighborCell = BoidCell + FIntVector(x, y, z);
                    
                    if (SpatialGrid.Contains(NeighborCell))
                    {
                        NearbyBoids.Append(SpatialGrid[NeighborCell].Boids);
                    }
                }
            }
        }
    }
    else
    {
        // Si l'optimisation spatiale n'est pas activée, renvoyer tous les boids
        NearbyBoids = AllBoids;
    }
    
    return NearbyBoids;
}