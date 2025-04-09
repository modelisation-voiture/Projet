#ifndef FORCE_HPP
#define FORCE_HPP

#include "voiture.hpp"
#include <utility>
#include <cmath>
#include <algorithm>

// === Classe abstraite Force ===
class Force {
public:
    virtual std::pair<double, double> calculer_force(const Voiture& voiture) const = 0;
    virtual ~Force() = default;
};

// === Force Motrice ===
class ForceMotrice : public Force {
private:
    double intensite_max;
    double vitesse_saturation;

public:
    ForceMotrice(double intensite_max, double vitesse_saturation = 20.0)
        : intensite_max(intensite_max), vitesse_saturation(vitesse_saturation) {}

    std::pair<double, double> calculer_force(const Voiture& voiture) const override {
        double vitesse = voiture.getVitesse();
        double ratio = 1.0 - (vitesse / vitesse_saturation);
        ratio = std::max(0.0, std::min(1.0, ratio));
        double force = intensite_max * ratio;
        double rad = voiture.getAngle() * M_PI / 180.0;
        return { -force * std::cos(rad), -force * std::sin(rad) };
    }
};

// Force motrice à accélération progressive (sans vitesse max)
class ForceMotriceProgressive : public Force {
    private:
        double acceleration_max;              
        mutable double acceleration_courante = 0.0; 
        double temps_montée;
        double temps_descente;
        double coeff_montée;
        double coeff_descente;
    
    public:
        ForceMotriceProgressive(double a_max = 10.0, double t_monte = 10.0, double t_descend = 5.0)
            : acceleration_max(a_max), temps_montée(t_monte), temps_descente(t_descend) {
            coeff_montée = (1.5)*acceleration_max / temps_montée;
            coeff_descente = acceleration_max * temps_descente ;// temps_descente;
        }
    
        std::pair<double, double> calculer_force(const Voiture& voiture) const override {
            // printf("L'attribut acceleration_active : %d\n", voiture.isAccelerationActive());
            if (voiture.isAccelerationActive()) {
                acceleration_courante += coeff_montée * 0.01;
                // printf("acceleration_courante : %f\n", acceleration_courante);
                if (acceleration_courante > acceleration_max)
                    acceleration_courante = acceleration_max;
            } else {
                acceleration_courante -= coeff_descente * 0.001;
                // printf("acceleration_courante : %f\n", acceleration_courante);

                if (acceleration_courante < 0.0)
                    acceleration_courante = 0.0;
            }
    
            double force = voiture.getMasse() * acceleration_courante;
            double rad = voiture.getAngle() * M_PI / 180.0;
    
            double fx = -force * std::cos(rad);
            double fy = -force * std::sin(rad);
            return {fx, fy};
        }

        double getAccelerationCourante() const { return acceleration_courante; }
        void setAccelerationCourante(double acc) { acceleration_courante = acc; }
    };

    
    


// === Force de Frottement ===
class ForceFrottement : public Force {
private:
    double coefficient;
    double g;

public:
    ForceFrottement(double coeff, double gravite = 9.81)
        : coefficient(coeff), g(gravite) {}

    std::pair<double, double> calculer_force(const Voiture& voiture) const override {
        double vitesse = voiture.getVitesse();
        if (vitesse == 0) return {0, 0};
        double frottement = coefficient * vitesse * vitesse * voiture.getMasse() * g;
        double fx = -frottement * (voiture.getVx() / vitesse);
        double fy = -frottement * (voiture.getVy() / vitesse);
        return {fx, fy};
    }

    void setCoefficient(double coeff) { coefficient = coeff; }

};

// === Force de Virage ===
class ForceVirage : public Force {
private:
    double angle_braquage_deg;

public:
    ForceVirage(double angle_deg) : angle_braquage_deg(angle_deg) {}

    std::pair<double, double> calculer_force(const Voiture& voiture) const override {
        double vx = voiture.getVitesseX();
        double vy = voiture.getVitesseY();
        double vpsi = voiture.getVitesseLacet();
        double angle_rad = voiture.getAngle() * M_PI / 180.0;
        double masse = voiture.getMasse();
        double empattement = voiture.getEmpattement();

        if (std::abs(vx) < 1e-3) return {0.0, 0.0};

        double braquage_rad = angle_braquage_deg * M_PI / 180.0;
        double beta = (vy + empattement * vpsi) / vx - braquage_rad;

        double rigidite = 1000.0;
        double Fmax = 0.8 * masse * 9.81;
        if (voiture.isFreinActif()) Fmax *= 0.1;

        double Fy = std::clamp(rigidite * beta, -Fmax, Fmax);
        double fx = -Fy * std::sin(angle_rad);
        double fy = Fy * std::cos(angle_rad);
        return {fx, fy};
    }
};

// === Force de Freinage (pédale) ===
class ForceFreinage : public Force {
private:
    double intensite_max;
    const double epsilon = 1e-3;

public:
    ForceFreinage(double intensite) : intensite_max(intensite) {}

    std::pair<double, double> calculer_force(const Voiture& voiture) const override {
        if (!voiture.isFreinActif()) return {0.0, 0.0};
        double vitesse = voiture.getVitesse();
        if (vitesse < epsilon) return {0.0, 0.0};
        double angle = voiture.getAngle() * M_PI / 180.0;
        return { -intensite_max * std::cos(angle), -intensite_max * std::sin(angle) };
    }

    void avancer(double) {} // Placeholder inutilisé
};

// === Force Aérodynamique (Cx) ===
class ForceAerodynamique : public Force {
private:
    double coefficient_trainee;

public:
    ForceAerodynamique(double coeff) : coefficient_trainee(coeff) {}

    std::pair<double, double> calculer_force(const Voiture& voiture) const override {
        double vx = voiture.getVitesseX();
        double vy = voiture.getVitesseY();
        double v_norme = std::sqrt(vx * vx + vy * vy);
        if (v_norme < 1e-3) return {0.0, 0.0};
        return { -coefficient_trainee * vx * v_norme, -coefficient_trainee * vy * v_norme };
    }
};

// === Force de Frein à main ===
class ForceFreinGlisse : public Force {
private:
    double intensite_max;
    mutable double intensite_courante = 0.0;
    double coeff_montée = 100.0;
    double coeff_descente = 20000.0;

public:
    ForceFreinGlisse(double intensite_max)
        : intensite_max(intensite_max) {}

    std::pair<double, double> calculer_force(const Voiture& voiture) const override {
        double v = voiture.getVitesse();
        if (v < 0.1) return {0.0, 0.0};

        if (voiture.isFreinMainActif()) {
            intensite_courante += coeff_montée * 0.01;
            if (intensite_courante > intensite_max)
                intensite_courante = intensite_max;
        } else {
            intensite_courante -= coeff_descente * 0.01;
            if (intensite_courante < 0)
                intensite_courante = 0;
        }

        double vx = voiture.getVx();
        double vy = voiture.getVy();
        double v_module = std::sqrt(vx * vx + vy * vy);
        if (v_module == 0) return {0.0, 0.0};

        double angle_decalage = M_PI / 12.0;
        double vx_rot = vx * std::cos(angle_decalage) - vy * std::sin(angle_decalage);
        double vy_rot = vx * std::sin(angle_decalage) + vy * std::cos(angle_decalage);
        double v_mod_rot = std::sqrt(vx_rot * vx_rot + vy_rot * vy_rot);

        return { -intensite_courante * (vx_rot / v_mod_rot), intensite_courante * (vy_rot / v_mod_rot)};
    }
};

class ForceCollision : public Force {
    private:
        const sf::Image& collisionMap;
    
    public:
        ForceCollision(const sf::Image& image) : collisionMap(image) {}
    
        std::pair<double, double> calculer_force(const Voiture& voiture) const override {
            sf::Vector2f futurePos(
                voiture.getX() + voiture.getVx() * 0.01,
                voiture.getY() + voiture.getVy() * 0.01
            );
    
            int x = static_cast<int>(futurePos.x);
            int y = static_cast<int>(futurePos.y);
    
            if (x < 0 || x >= static_cast<int>(collisionMap.getSize().x) ||
                y < 0 || y >= static_cast<int>(collisionMap.getSize().y)) {
                return { -voiture.getVx() * 200.0, -voiture.getVy() * 200.0 };  // Freinage brutal
            }
    
            sf::Color pixel = collisionMap.getPixel(x, y);
            if (pixel.r < 30 && pixel.g < 30 && pixel.b < 30) {
                // Surface noire : appliquer une décélération brutale
                return { -voiture.getVx() * 500.0, -voiture.getVy() * 500.0 };
            }
    
            return {0.0, 0.0}; // Pas de collision
        }
    };
    

#endif // FORCE_HPP
