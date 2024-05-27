# Fichiers de test pour la matrice des chemins

## Entrée

- Ligne 1 : une chaîne de caractères (sans espace) donnant le chemin du graphe.
- Ligne 2 : une chaîne de caractères (sans espace) donnant le chemin du fichier des coordonnées GPS.
- Ligne 3 : un entier `n` représentant le nombre de points.
- Ligne 4 : `n` entiers séparés par des espaces représentant les identifiants des points de la tournée.

## Sortie simplifiée

- Ligne 1 à `n`x`n` : les éléments suivants, séparés par des espaces :
  - un entier `i` donnant l'indice de la ligne de la matrice,
  - un entier `j` donnant l'indice de la colonne de la matrice,
  - un réel représentant la distance du plus court chemin entre les points d'identifiants d'indices `i` et `j` dans le fichier d'entrée.
  
Les coefficients de la matrice sont donnés dans l'ordre naturel. Par exemple, si `n` = 3, le fichier de sortie contiendra les 9 lignes "0 0", "0 1", "0 2", "1 0", "1 1", "1 2", "2 0", "2 1" et "2 2".

## Sortie complète

- Ligne 1 à `n`x`n` : les éléments suivants, séparés par des espaces :
  - un entier `i` donnant l'indice de la ligne de la matrice,
  - un entier `j` donnant l'indice de la colonne de la matrice,
  - un réel représentant la distance du plus court chemin entre les points d'identifiants d'indices `i` et `j` dans le fichier d'entrée,
  - un entier `m` représentant le nombre de points composant le chemin,
  - `m` entiers représentant les identifiants des points composant le chemin (dans l'ordre).
  
# Affichage

Le fichier `outputX.geojson` représente la solution sous le format geojson.
Il peut être importé sur le site : https://umap.openstreetmap.fr/