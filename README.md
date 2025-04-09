# Simulation de Trajectoire d’une Voiture RC

Ce projet simule en 2D la trajectoire d’une voiture radiocommandée (RC) avec un **modèle physique réaliste**, en utilisant la bibliothèque **SFML** pour l’affichage et la gestion des entrées clavier. L’objectif est de modéliser la dynamique de la voiture, d’implémenter des forces physiques, et de suivre une trajectoire à l’aide d’un contrôleur PID.

## 🎮 Fonctionnalités

- Simulation physique réaliste (accélération progressive, freinage, frottements, etc.)
- Contrôle manuel ou autonome avec PID
- Circuit généré dynamiquement avec waypoints
- Affichage 2D avec textures SFML
- Chronométrage des tours
- Système de collision simple (zones d’herbe/délimitation)
- Structure modulaire orientée objet

## 🧱 Structure du projet

/assets # Textures utilisées (piste, voiture, herbe, etc.) 

/circuits # Données éventuelles pour d'autres circuits 

/SFML-2.6.2 # Dossier SFML (inclus dans le dépôt)

/src 
  ├── Affichage # Affichage SFML, map, textures 
  ├── Model # Forces physiques + dynamique de la voiture 
  ├── PID # Contrôleur PID (vitesse, direction) 
  ├── Simulation # Fichier principal main.cpp + Makefile /tests # Éventuels tests unitaires


## ⚙️ Compilation

### 1. Prérequis

- **SFML 2.6.2** (déjà fourni dans le dossier `SFML-2.6.2/`)
- **Compilateur C++17 compatible** (Clang, g++)
- Optionnel :
  - `nlohmann/json` (si besoin d'extension JSON plus tard)

#### Dépendances (Mac/Linux)

```bash
# Sur Mac
brew install sfml nlohmann-json boost

# Sur Ubuntu
sudo apt update
sudo apt install libsfml-dev nlohmann-json3-dev 

### 2. Compilation

```bash
cd src/Simulation
make
```

Cela génère un exécutable `rc_simulation` dans le même dossier.

### 3. Exécution

```bash
./rc_simulation
```

## Commandes

- `Espace` : accélérer (mode manuel)
- `Entrée` : freiner
- `←` / `→` : tourner
- `A` : activer/désactiver le mode autonome
- `↓` : frein à main

## Auteurs

- **Nadir** : Modélisation physique, forces, dynamique véhicule
- **David** : Contrôles utilisateur, PID
- **Talla** : Interface graphique SFML 