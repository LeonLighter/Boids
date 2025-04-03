# TPBoids
 
Explication technique du système de Boids dans Unreal Engine
Classe ABoid (Étapes 1-4)
Fonctionnement général
Chaque boid est un acteur individuel qui maintient sa propre position et vélocité. Il évalue constamment son environnement pour ajuster sa trajectoire selon les trois règles fondamentales (séparation, alignement, cohésion) et l'évitement d'obstacles.
Principales fonctions

Constructeur : Initialise le boid avec un mesh et une vélocité aléatoire
BeginPlay : Configure les paramètres initiaux
Tick : Met à jour la position et la rotation du boid selon sa vélocité
UpdateVelocity : Méthode centrale qui calcule et applique toutes les forces agissant sur le boid
Separation : Calcule la force qui éloigne le boid de ses voisins trop proches
Alignment : Calcule la force qui aligne la direction du boid avec celle de ses voisins
Cohesion : Calcule la force qui attire le boid vers le centre de masse de ses voisins
ObstacleAvoidance : Détecte et évite les obstacles via des raycasts
LimitVector : Utilitaire pour limiter la magnitude d'un vecteur
IsInFieldOfView : Détermine si un autre boid est dans le champ de vision

Variables clés

Vélocité : Direction et vitesse actuelles
Paramètres de comportement : MaxSpeed, MaxForce, poids des différentes forces
Paramètres de perception : Rayon de voisinage, angle de vision

Classe ABoidManager (Étapes 1-4)
Fonctionnement général
Le gestionnaire crée et supervise tous les boids. Il implémente des optimisations comme la partition spatiale pour réduire la complexité des calculs quand de nombreux boids sont présents.
Principales fonctions

Constructeur : Initialise les paramètres du système
BeginPlay : Crée tous les boids
Tick : Met à jour la grille spatiale et tous les boids
SpawnBoids : Crée le nombre spécifié de boids à des positions aléatoires
UpdateBoids : Parcourt tous les boids et met à jour leur vélocité
GetGridCellForLocation : Convertit une position 3D en coordonnées de cellule de grille
UpdateSpatialGrid : Répartit tous les boids dans leurs cellules respectives
GetNearbyBoids : Récupère les boids voisins d'un boid donné en utilisant la grille spatiale

Variables clés

Configuration du système : Nombre de boids, volume d'apparition
Optimisation : Taille des cellules de la grille, activation de la partition spatiale
Références : Liste de tous les boids, grille spatiale

Flux de traitement à chaque frame

Le BoidManager met à jour sa grille spatiale en répartissant tous les boids dans leurs cellules respectives
Pour chaque boid:

Le manager obtient les boids voisins en consultant uniquement les cellules pertinentes de la grille
Le boid calcule les forces de séparation, alignement et cohésion basées sur ses voisins
Le boid détecte les obstacles potentiels via des raycasts
Toutes les forces sont combinées, pondérées et appliquées pour mettre à jour la vélocité
Le boid met à jour sa position et sa rotation selon sa nouvelle vélocité



Optimisation spatiale (Étape 4)
La partition spatiale divise l'espace en cellules. Plutôt que de comparer chaque boid avec tous les autres (complexité O(n²)), chaque boid examine uniquement ceux situés dans les cellules adjacentes. Ceci réduit considérablement la complexité de calcul, surtout quand le nombre de boids est élevé.
La taille des cellules de la grille est un paramètre crucial : trop petites, elles n'incluront pas assez de voisins pertinents ; trop grandes, elles perdent leur avantage d'optimisation. Idéalement, la taille de cellule devrait être légèrement supérieure au rayon de perception des boids.
Cette architecture fournit un système de boids performant et réaliste, où des comportements complexes émergent de règles simples appliquées localement.