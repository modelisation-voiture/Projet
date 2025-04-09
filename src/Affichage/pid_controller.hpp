#ifndef PID_CONTROLLER_HPP
#define PID_CONTROLLER_HPP

#include <deque>
#include <tuple>

class PIDController {
private:
    float kp, ki, kd;              // Paramètres PID (proportionnel, intégral, dérivé)
    float previousError;           // Erreur précédente
    float integral;                // Somme des erreurs (intégrale)
    float maxIntegral;             // Anti-windup: limite l'intégral pour éviter l'accumulation
    float maxOutput;               // Limite la sortie du contrôleur
    std::deque<float> errorHistory; // Historique des erreurs pour lissage
    
public:
    PIDController(float kp = 1.0f, float ki = 0.0f, float kd = 0.0f, 
                 float maxIntegral = 10.0f, float maxOutput = 1.0f, int historySize = 5);
    
    void reset();
    float calculate(float error, float dt);
    
    // Getters and setters
    void setConstants(float p, float i, float d);
    std::tuple<float, float, float> getConstants() const;
};

#endif // PID_CONTROLLER_HPP