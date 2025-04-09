#include "pid_controller.hpp"
#include <algorithm>

PIDController::PIDController(float kp, float ki, float kd, 
                           float maxIntegral, float maxOutput, int historySize)
    : kp(kp), ki(ki), kd(kd), previousError(0.0f), integral(0.0f),
      maxIntegral(maxIntegral), maxOutput(maxOutput) {
    errorHistory.resize(historySize, 0.0f);
}

void PIDController::reset() {
    previousError = 0.0f;
    integral = 0.0f;
    for (auto& error : errorHistory) {
        error = 0.0f;
    }
}

float PIDController::calculate(float error, float dt) {
    // Mettre à jour l'historique des erreurs pour le lissage
    errorHistory.pop_front();
    errorHistory.push_back(error);
    
    // Calculer l'erreur lissée (moyenne mobile)
    float smoothedError = 0.0f;
    for (const float& e : errorHistory) {
        smoothedError += e;
    }
    smoothedError /= errorHistory.size();
    
    // Calculer l'intégrale avec anti-windup
    integral += smoothedError * dt;
    if (integral > maxIntegral) integral = maxIntegral;
    if (integral < -maxIntegral) integral = -maxIntegral;
    
    // Calculer la dérivée
    float derivative = (smoothedError - previousError) / dt;
    previousError = smoothedError;
    
    // Calculer la sortie du PID
    float output = kp * smoothedError + ki * integral + kd * derivative;
    
    // Limiter la sortie
    if (output > maxOutput) output = maxOutput;
    if (output < -maxOutput) output = -maxOutput;
    
    return output;
}

void PIDController::setConstants(float p, float i, float d) {
    kp = p;
    ki = i;
    kd = d;
}

std::tuple<float, float, float> PIDController::getConstants() const {
    return {kp, ki, kd};
}