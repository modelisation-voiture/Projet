# Projet de simulation dynamique d'une voiture RC en C++

## 📅 Objectif
Simuler le comportement dynamique d'une voiture RC dans un environnement 2D, en prenant en compte :
- Les forces physiques réalistes (motrice, freinage, frottements, virages)
- Le glissement des pneus et la perte d’adhérence
- Une visualisation différenciée des phases de conduite

Le tout est conçu en C++ avec une architecture orientée objet claire et modulable.



## 📄 Architecture du projet

### 1. `voiture.hpp / voiture.cpp`
Classe `Voiture`
- Stocke l’état dynamique de la voiture : position, vitesses, angle, masse, empattement, etc.
- Méthodes :
  - `appliquerForce()` : applique une force externe
  - `updatePositionRK4()` : intègre les équations différentielles
  - Getters pour accéder à la vitesse, angle, etc.

### 2. `force.hpp`
Contient toutes les forces sous forme de classes dérivées d'une classe abstraite `Force` :
- `ForceMotrice` : propulsion constante
- `ForceFrottement` : frottements proportionnels à la vitesse
- `ForceFreinage` : freinage opposé à la vitesse, activé dynamiquement
- `ForceVirage` : modélisation de l’adhérence latérale avec saturation du pneu (glissement)

### 3. `map.hpp / map.cpp`
Classe `Map`
- Stocke les positions (x,y) de la voiture
- Permet de les sauvegarder dans un fichier texte `trajectoire.txt` utilisé pour la visualisation Python

### 4. `main.cpp`
Contient le scénario temporel complet de la simulation, découpé en 7 phases logiques :

| Phase | Durée      | Action                                        |
|-||--|
| 1     | 0 - 10s    | Accélération en ligne droite                |
| 2     | 10 - 15s   | Virage sinusoïdal (ellipse) sous accélération |
| 3     | 15 - 17s   | Roulage libre                                 |
| 4     | 17 - 19s   | Freinage en ligne droite                      |
| 5     | 19 - 29s   | Réaccélération                               |
| 6     | 29 - 34s   | Virage fort sous freinage (glissement test)  |
| 7     | 34 - 36s   | Roulage final                                 |

Chaque phase applique les forces correspondantes, met à jour la position avec la méthode `updatePositionRK4()`, et exporte l’état dans un fichier texte.



## 📈 Visualisation Python : `trajectoire.py`
- Lecture du fichier `trajectoire.txt`
- Affichage de la trajectoire en 2D avec couleurs selon le type de conduite :
  - 🔵 Bleu : accélération
  - 🔴 Rouge : freinage
  - ⚫ Noir : roulage libre

Permet de valider visuellement les comportements dynamiques, virages, glissements, etc.



## 📆 Fonctionnalités modélisées
- Système de forces modulaire (orienté objet)
- Virages dynamiques avec modèle de glissement des pneus (angle de dérive + saturation)
- Visualisation des phases dynamiques de la voiture
- Architecture orientée objet extensible (facile à enrichir)



## Extensions possibles
- Ajout d’une résistance de l’air (force proportionnelle à `v^2`)
- Limitation du couple moteur en fonction de la vitesse
- Modèle 4 roues / suspensions
- Contrôleur PID / IA de pilotage


