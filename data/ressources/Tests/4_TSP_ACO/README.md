# Fichiers de test pour le TSP avec l'agorithme ACO

## Entrée

- Ligne 1 : une chaîne de caractères (sans espace) donnant le chemin du graphe.
- Ligne 2 : une chaîne de caractères (sans espace) donnant le chemin du fichier des coordonnées GPS.
- Ligne 3 : un entier `n` représentant le nombre de points.
- Ligne 4 : `n` entiers séparés par des espaces représentant les identifiants des points de la tournée.

## Sortie

- Ligne 1 : les éléments suivants, séparés par des espaces :
  - un réel représentant la distance de la tournée,
  - un entier donnant le nombre de point de la tournée (égal à `n` + 1)
- Ligne 2 : `n` entiers représentant les identifiants des points composant la tournée (dans l'ordre). Les premier et dernier identifiants sont obligatoirement égaux à 0.

La tournée est calculée avec les paramètres suivants :
- station = 0
- iterationCount = 1000
- antCount = 100
- alpha = 2.0f
- beta = 3.0f
- rho = 0.1f
- q = 2.0f

# Affichage

Le fichier `outputX.geojson` représente la solution sous le format geojson.
Il peut être importé sur le site : https://umap.openstreetmap.fr/