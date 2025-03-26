#include "voiture.hpp"
#include <utility>

Voiture::Voiture(double x_init, double y_init, double angle_deg, double masse, double empattement)
    : x(x_init), y(y_init), angle(angle_deg), vx(0), vy(0), ax(0), ay(0), masse(masse), empattement(empattement) {}

void Voiture::appliquerForce(double fx, double fy) {
    ax = fx / masse; // Rajouter les forces de frottement.
    ay = fy / masse;
}


void Voiture::updatePositionRK4(double dt, double fx, double fy, double coeff_frottement, double angle_braquage_deg) {
    // Calcul de l'accélération linéaire (sans force latérale, seulement moteur et frottement)
    double vitesse = sqrt(vx*vx + vy*vy);
    double fx_frott = vitesse > 0 ? -coeff_frottement * vitesse * vitesse * masse * 9.81 * (vx / vitesse) : 0;
    double fy_frott = vitesse > 0 ? -coeff_frottement * vitesse * vitesse * masse * 9.81 * (vy / vitesse) : 0;

    double ax = (fx + fx_frott) / masse;
    double ay = (fy + fy_frott) / masse;

    // Mise à jour simple vitesse linéaire
    vx += ax * dt;
    vy += ay * dt;

    // Vitesse angulaire (rotation progressive voiture)
    double R = calculerRayonCourbure(angle_braquage_deg);
    double omega = vitesse / R; // vitesse angulaire = vitesse linéaire / rayon de courbure

    // Rotation progressive de la direction (angle)
    angle += omega * dt;
    angle = fmod(angle, 360.0);

    // Réorientation vitesse selon nouvel angle (très important pour stabilité)
    double v_norme = sqrt(vx*vx + vy*vy);
    double angle_rad = angle * M_PI / 180.0;

    vx = v_norme * cos(angle_rad);
    vy = v_norme * sin(angle_rad);

    // Mise à jour position
    x += vx * dt;
    y += vy * dt;

    // Accélération totale pour affichage
    this->ax = ax;
    this->ay = ay;
}



double Voiture::calculerRayonCourbure(double angle_braquage_deg) const {
    double angle_braquage_rad = angle_braquage_deg * M_PI / 180.0;
    if (std::abs(angle_braquage_rad) < 1e-6) // évite division par zéro
        return 1e6; // très grand rayon ≈ ligne droite
    return empattement / std::tan(angle_braquage_rad);
}




double Voiture::getX() const { return x; }
double Voiture::getY() const { return y; }
double Voiture::getAngle() const { return angle; }
double Voiture::getVitesse() const { return sqrt(vx*vx + vy*vy); }
double Voiture::getVitesseX() const { return vx; }
double Voiture::getVitesseY() const { return vy; }
double Voiture::getAcceleration() const { return sqrt(ax*ax + ay*ay); }
double Voiture::getEmpattement() const { return empattement; }
double Voiture::getVx() const { return vx; }
double Voiture::getVy() const { return vy; }
double Voiture::getMasse() const { return masse; }
double Voiture::getVitesseLacet() const { return sqrt(vx*vx + vy*vy) / empattement; }

