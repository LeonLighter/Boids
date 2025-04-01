// Boid.cpp
#include "Boid.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

ABoid::ABoid()
{
    PrimaryActorTick.bCanEverTick = true;
    
    BoidMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BoidMesh"));
    RootComponent = BoidMesh;
    
    // Initialiser la vitesse aléatoirement
    Velocity = FVector(FMath::RandRange(-1.0f, 1.0f), 
                      FMath::RandRange(-1.0f, 1.0f), 
                      FMath::RandRange(-1.0f, 1.0f)).GetSafeNormal() * MaxSpeed;
}

void ABoid::BeginPlay()
{
    Super::BeginPlay();
}

void ABoid::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
    
    // La mise à jour sera gérée par le BoidManager, mais nous pouvons déplacer le boid ici
    if (!Velocity.IsNearlyZero())
    {
        // Mettre à jour la position
        AddActorWorldOffset(Velocity * DeltaTime, true);
        
        // Mettre à jour la rotation pour faire face à la direction du mouvement
        FRotator NewRotation = Velocity.Rotation();
        SetActorRotation(NewRotation);
    }
}

void ABoid::UpdateVelocity(TArray<ABoid*>& NearbyBoids, float DeltaTime)
{
    // Calculer les forces de comportement
    FVector SeparationForce = Separation(NearbyBoids) * SeparationWeight;
    FVector AlignmentForce = Alignment(NearbyBoids) * AlignmentWeight;
    FVector CohesionForce = Cohesion(NearbyBoids) * CohesionWeight;
    FVector AvoidanceForce = ObstacleAvoidance();
    
    // Appliquer les forces
    FVector Acceleration = SeparationForce + AlignmentForce + CohesionForce + AvoidanceForce;
    Acceleration = LimitVector(Acceleration, MaxForce);
    
    // Mettre à jour la vitesse
    Velocity += Acceleration * DeltaTime;
    Velocity = LimitVector(Velocity, MaxSpeed);
}

FVector ABoid::Separation(TArray<ABoid*>& NearbyBoids)
{
    FVector SteeringForce = FVector::ZeroVector;
    int Count = 0;
    
    for (ABoid* OtherBoid : NearbyBoids)
    {
        if (OtherBoid != this && IsInFieldOfView(OtherBoid))
        {
            FVector Direction = GetActorLocation() - OtherBoid->GetActorLocation();
            float Distance = Direction.Size();
            
            // Éviter la division par zéro et normaliser avec un poids inversement proportionnel à la distance
            if (Distance > 0.0f && Distance < NeighborRadius)
            {
                // Méthode 2 (progressive) comme décrite dans le document
                float Ratio = Distance / NeighborRadius;
                Direction.Normalize();
                SteeringForce += Direction * (1.0f - Ratio);
                Count++;
            }
        }
    }
    
    if (Count > 0)
    {
        SteeringForce /= Count;
        if (SteeringForce.Size() > 0)
        {
            SteeringForce.Normalize();
            SteeringForce *= MaxSpeed;
            SteeringForce -= Velocity;
            SteeringForce = LimitVector(SteeringForce, MaxForce);
        }
    }
    
    return SteeringForce;
}

FVector ABoid::Alignment(TArray<ABoid*>& NearbyBoids)
{
    FVector AverageVelocity = FVector::ZeroVector;
    int Count = 0;
    
    for (ABoid* OtherBoid : NearbyBoids)
    {
        if (OtherBoid != this && IsInFieldOfView(OtherBoid))
        {
            float Distance = FVector::Dist(GetActorLocation(), OtherBoid->GetActorLocation());
            if (Distance < NeighborRadius)
            {
                AverageVelocity += OtherBoid->GetVelocity();
                Count++;
            }
        }
    }
    
    if (Count > 0)
    {
        AverageVelocity /= Count;
        AverageVelocity.Normalize();
        AverageVelocity *= MaxSpeed;
        
        FVector SteeringForce = AverageVelocity - Velocity;
        return LimitVector(SteeringForce, MaxForce);
    }
    
    return FVector::ZeroVector;
}

FVector ABoid::Cohesion(TArray<ABoid*>& NearbyBoids)
{
    FVector CenterOfMass = FVector::ZeroVector;
    int Count = 0;
    
    for (ABoid* OtherBoid : NearbyBoids)
    {
        if (OtherBoid != this && IsInFieldOfView(OtherBoid))
        {
            float Distance = FVector::Dist(GetActorLocation(), OtherBoid->GetActorLocation());
            if (Distance < NeighborRadius)
            {
                CenterOfMass += OtherBoid->GetActorLocation();
                Count++;
            }
        }
    }
    
    if (Count > 0)
    {
        CenterOfMass /= Count;
        FVector DesiredVelocity = CenterOfMass - GetActorLocation();
        DesiredVelocity.Normalize();
        DesiredVelocity *= MaxSpeed;
        
        FVector SteeringForce = DesiredVelocity - Velocity;
        return LimitVector(SteeringForce, MaxForce);
    }
    
    return FVector::ZeroVector;
}

FVector ABoid::ObstacleAvoidance()
{
    // Nombre de points de raycast sur la sphère
    const int NumRaycastPoints = 8;
    const float RaycastDistance = 200.0f;
    
    FVector SteeringForce = FVector::ZeroVector;
    int HitCount = 0;
    
    // Générer des points sur une sphère avec le golden ratio
    const float GoldenRatio = 1.618033988749895f;
    
    for (int i = 0; i < NumRaycastPoints; ++i)
    {
        // Calculer les angles avec le golden ratio pour une répartition uniforme
        float Theta = FMath::Acos(1.0f - 2.0f * (float)i / NumRaycastPoints);
        float Phi = 2.0f * PI * GoldenRatio * i;
        
        // Convertir en coordonnées cartésiennes
        FVector RayDirection(
            FMath::Sin(Theta) * FMath::Cos(Phi),
            FMath::Sin(Theta) * FMath::Sin(Phi),
            FMath::Cos(Theta)
        );
        
        // Transformer la direction selon l'orientation du boid
        RayDirection = GetActorRotation().RotateVector(RayDirection);
        
        // Tracer un rayon
        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(this);
        
        // Utiliser le canal de collision des objets qui devraient être évités
        if (GetWorld()->LineTraceSingleByChannel(
                HitResult, 
                GetActorLocation(), 
                GetActorLocation() + RayDirection * RaycastDistance,
                ECC_WorldStatic, 
                QueryParams))
        {
            // Si on a une collision, ajouter une force d'évitement
            SteeringForce -= RayDirection * (RaycastDistance - HitResult.Distance) / RaycastDistance;
            HitCount++;
            
            // Debug - afficher les rayons qui ont touché des obstacles
            DrawDebugLine(GetWorld(), GetActorLocation(), HitResult.Location, FColor::Red, false, 0.0f, 0, 1.0f);
        }
        else
        {
            // Debug - afficher les rayons qui n'ont pas touché d'obstacles
            DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + RayDirection * RaycastDistance, FColor::Green, false, 0.0f, 0, 1.0f);
        }
    }
    
    if (HitCount > 0)
    {
        SteeringForce.Normalize();
        SteeringForce *= MaxSpeed;
        SteeringForce -= Velocity;
        return LimitVector(SteeringForce, MaxForce * 2.0f); // Force d'évitement plus forte
    }
    
    return FVector::ZeroVector;
}

FVector ABoid::LimitVector(const FVector& Vector, float Max)
{
    if (Vector.SizeSquared() > Max * Max)
    {
        return Vector.GetSafeNormal() * Max;
    }
    return Vector;
}

bool ABoid::IsInFieldOfView(const ABoid* OtherBoid) const
{
    if (!OtherBoid)
        return false;
        
    FVector DirectionToOther = OtherBoid->GetActorLocation() - GetActorLocation();
    DirectionToOther.Normalize();
    
    // Direction vers laquelle le boid regarde
    FVector ForwardDirection = GetActorForwardVector();
    
    // Calculer l'angle entre les deux vecteurs
    float DotProduct = FVector::DotProduct(ForwardDirection, DirectionToOther);
    float AngleRadians = FMath::Acos(DotProduct);
    float AngleDegrees = FMath::RadiansToDegrees(AngleRadians);
    
    // Si l'angle est inférieur à la moitié du champ de vision, l'autre boid est dans le FOV
    return AngleDegrees <= (ViewAngle / 2.0f);
}