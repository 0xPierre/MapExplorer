# Fichiers de test pour Dijkstra

## Entrée

- Ligne 1 : une chaîne de caractères (sans espace) donnant le chemin du graphe.
- Ligne 2 : une chaîne de caractères (sans espace) donnant le chemin du fichier des coordonnées GPS.
- Ligne 3 : deux entiers `start` et `end` séparés par un espace représentant les identifiants des points de départ et d'arrivée.

## Sortie

- Ligne 1 : la distance du plus court chemin entre `start` et `end` exprimée en mètres avec un chiffre après la virgule.
- Ligne 2 : le nombre `n` de points composant le chemin.
- Ligne 3 : `n` entiers séparés par des espaces représentant les identifiants des points composant le chemin.

## Affichage

Le fichier `outputX.geojson` représente la solution sous le format geojson.
Il peut être importé sur le site : https://umap.openstreetmap.fr/