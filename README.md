# Projet

Voici le contenu du **README.md** structuré et prêt à être copié-collé dans votre projet GitHub.  

```md
# Simulation de Trajectoire d’une Voiture RC

## Objectif du projet

Ce projet vise à modéliser et simuler la trajectoire d’une voiture RC sur un circuit avec une **interface graphique 2D en SFML**. L'objectif est d'intégrer un **modèle physique réaliste** basé sur des équations différentielles et d'optimiser la trajectoire pour un circuit donné.  

Dans un second temps, une simulation d’un **capteur LiDAR** sera ajoutée afin d’évaluer dynamiquement l’environnement et ajuster la trajectoire en conséquence.

## Équipe

- **Nadir** : Modélisation physique et simulation de la dynamique du véhicule.  
- **David** : Développement de l’interface graphique avec SFML.  
- **Talla** : Gestion des interactions utilisateur et intégration des algorithmes de contrôle.  

## Organisation du projet

```
/Projet-RC
│── /docs               # Documentation (formules mathématiques, modèles physiques)
│── /src                # Code source principal en C++
│   │── main.cpp        # Point d'entrée principal du programme
│   │── Simulation/     # Moteur physique
│   │   │── Modeles/    # Modèles de dynamique et trajectoire
│   │   │── Solver/     # Résolution des équations différentielles
│   │── Affichage/      # Interface graphique SFML
│   │── Controle/       # Algorithmes d’optimisation
│   │── Donnees/        # Fichiers de circuits
│── /tests              # Tests unitaires
│── /circuits           # Données des circuits en JSON
│── /assets             # Images et textures
│── README.md           # Présentation du projet
│── Makefile            # Compilation avec SFML
```

## Technologies utilisées

- **C++** : Langage principal du projet.  
- **SFML** : Bibliothèque pour l’affichage et la gestion des entrées utilisateur.  
- **Makefile** : Automatisation de la compilation et de l’exécution.  

---

## Plan de travail

### Semaine 1 : Analyse et préparation (14-21 février)

**Objectifs**  
- Étudier les **modèles mathématiques** pour la dynamique d’une voiture RC.  
- Définir la **structure du circuit** dans un repère cartésien.  
- Préparer l’environnement de développement et initialiser **le projet GitHub**.  

**Répartition des tâches**  
- **Nadir** : Choisir et implémenter les **équations du modèle dynamique**.  
- **David** : Installer et tester **SFML**, préparer une **fenêtre 2D** affichant un circuit statique.  
- **Talla** : Structurer le projet sur **GitHub**, préparer le **Makefile** pour SFML.  

---

### Semaine 2-3 : Implémentation de la simulation physique (22 février - 6 mars)

**Objectifs**  
- Développer le **moteur de simulation** basé sur la dynamique d’une voiture RC.  
- Intégrer les **équations différentielles** pour mettre à jour la position du véhicule.  
- Afficher une **première version de la trajectoire** sur l’interface SFML.  

**Formules utilisées**  
- **Position** :  
  ```
  X_dot = Vx * cos(ψ) - Vy * sin(ψ)
  Y_dot = Vx * sin(ψ) + Vy * cos(ψ)
  ```
- **Vitesse latérale** :  
  ```
  Vy_dot = -Vx * ψ_dot + (2 * Cf / m) * (δf - (Vy + a * ψ_dot) / Vx) + (2 * Cr / m) * (- (Vy - b * ψ_dot) / Vx)
  ```
- **Angle de lacet** :  
  ```
  ψ_dot = (2 * a * Cf / Iz) * (δf - (Vy + a * ψ_dot) / Vx) - (2 * b * Cr / Iz) * ((Vy - b * ψ_dot) / Vx)
  ```

**Répartition des tâches**  
- **Nadir** : Implémentation des **équations de dynamique** et mise à jour de la position du véhicule.  
- **David** : Gestion des **affichages graphiques** (circuit, véhicule en mouvement, tracé de la trajectoire).  
- **Talla** : Interaction **clavier** (accélération, freinage) et affichage des données de simulation.  

---

### Semaine 4 : Gestion de collision et optimisation du modèle (7-14 mars)

**Objectifs**  
- Ajouter **des bordures de circuit** et détecter les sorties de route.  
- Améliorer la gestion **des virages et des trajectoires optimales**.  
- Implémenter un **système de correction de trajectoire** en fonction des collisions détectées.  

**Méthode de détection des collisions**  
- Définir le circuit comme une **matrice de pixels** ou une **liste de segments (x,y)**.  
- Vérifier en temps réel si **les coordonnées de la voiture sont valides**.  

**Répartition des tâches**  
- **Nadir** : Implémentation de la **détection des bordures et des collisions**.  
- **David** : Affichage **graphique des zones de collision** et amélioration de la gestion des virages.  
- **Talla** : Optimisation du **moteur physique** pour rendre la simulation plus fluide.  

---

### Semaine 5 : Ajout de la simulation LiDAR (15-21 mars)

**Objectifs**  
- Simuler un **capteur LiDAR** qui détecte les bordures du circuit.  
- Modifier l’algorithme de contrôle pour que le véhicule **utilise ces données** afin d’ajuster sa trajectoire.  

**Méthode pour simuler le LiDAR**  
- Émettre des **rayons virtuels** autour de la voiture.  
- Déterminer les **intersections** avec les bords du circuit.  
- Calculer la **distance aux obstacles** et ajuster l’angle de braquage.  

**Répartition des tâches**  
- **Nadir** : Calcul des **intersections des rayons LiDAR** avec le circuit.  
- **David** : Affichage **des points détectés** par le LiDAR.  
- **Talla** : Ajustement **de la trajectoire en fonction des distances mesurées**.  

---

### Semaine 6 : Tests, finitions et rapport final (22-28 mars)

**Objectifs**  
- Effectuer des **tests de robustesse** (différentes vitesses, types de virages).  
- Ajouter une **animation des trajectoires optimales**.  
- Finaliser la **documentation et le rapport technique**.  

**Répartition des tâches**  
- **Nadir** : Tests finaux et analyse **des performances** du moteur physique.  
- **David** : **Amélioration graphique** et corrections de bugs.  
- **Talla** : Rédaction du **rapport final** et production d’une vidéo de démonstration.  

---

### Derniers jours (29-31 mars)

**Objectifs**  
- Préparer **la présentation** du projet.  
- Réaliser une **vidéo de démonstration**.  
- Finaliser la **documentation** et les supports de présentation.  

---

## Conclusion

Le projet sera centré sur la **modélisation physique et la simulation** d’une voiture RC sur un circuit en SFML.  
La priorité sera d’intégrer des **équations différentielles précises** pour obtenir un comportement réaliste et de structurer le code de manière modulaire.  
La **gestion des collisions et de la trajectoire sera développée en premier**, avant d’intégrer progressivement la simulation du LiDAR.  

L’approche retenue permet de maximiser la fidélité physique tout en assurant une interface graphique interactive et performante.  
