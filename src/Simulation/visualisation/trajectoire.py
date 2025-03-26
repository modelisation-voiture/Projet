import matplotlib.pyplot as plt
import numpy as np
import mplcursors

# Paramètres de simulation
dt = 0.01  # intervalle de temps entre chaque point dans le C++
sample_interval = 300  # intervalle d'affichage des échantillons

x, y = [], []

# Chargement des données
with open("trajectoire.txt", "r") as file:
    for line in file:
        xi, yi = map(float, line.strip().split(','))
        x.append(xi)
        y.append(yi)

# Conversion en numpy arrays
x = np.array(x)
y = np.array(y)

# Points d'échantillonnage
x_sampled = x[::sample_interval]
y_sampled = y[::sample_interval]

# Calcul des distances et vitesses entre chaque échantillon affiché
distances_sampled = np.sqrt(np.diff(x_sampled)**2 + np.diff(y_sampled)**2)
temps_entre_echantillons = dt * sample_interval
vitesses_sampled = distances_sampled / temps_entre_echantillons

# Création graphique
plt.figure(figsize=(12, 9), facecolor='black')

# Trajectoire complète
plt.plot(x, y, color='black', linewidth=1, linestyle='--')

# Affichage des points d'échantillonnage
sc = plt.scatter(x_sampled, y_sampled, color='black', s=60, zorder=4)

# Départ et arrivée
plt.scatter(x[0], y[0], color='green', label="Départ", s=120, zorder=5)
plt.scatter(x[-1], y[-1], color='red', label="Arrivée", s=120, zorder=5)

# Annotation interactive avec mplcursors
cursor = mplcursors.cursor(sc, hover=True)
@cursor.connect("add")
def on_add(sel):
    index = sel.index
    if index == 0:
        annotation = f"Point {index}"
    else:
        t_val = index * sample_interval * dt
        v_val = vitesses_sampled[index-1]
        annotation = f"Point {index}\n\nt = {t_val:.2f} s\nv ≈ {v_val:.2f} m/s"
    sel.annotation.set_text(annotation)
    sel.annotation.get_bbox_patch().set(fc="grey", alpha=0.6, ec="white")

# Calcul et affichage de la distance totale
distance_totale = np.sum(np.sqrt(np.diff(x)**2 + np.diff(y)**2))
plt.text(0.05, 0.95, f"Distance totale : {distance_totale:.2f} m",
         transform=plt.gca().transAxes,
         fontsize=14, color='white', verticalalignment='top',
         bbox=dict(facecolor='black', alpha=0.5))

# Configuration des axes et du titre
plt.xlabel('X (m)', color='white', fontsize=12)
plt.ylabel('Y (m)', color='white', fontsize=12)
plt.tick_params(colors='white')
plt.title('Trajectoire Voiture RC avec détails échantillons', color='white', fontsize=14)
plt.legend()
plt.axis('equal')
plt.grid(True, color='gray', linestyle=':', linewidth=0.5)

plt.show()
