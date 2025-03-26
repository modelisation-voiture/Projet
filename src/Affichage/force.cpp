#include "force.hpp"

Force::Force() : frictionCoefficient(0.2f) {}

void Force::setFriction(bool isGrass) {
    frictionCoefficient = isGrass ? 0.8f : 0.2f; // L'herbe a un frottement plus élevé
}

float Force::computeFriction(float velocity) {
    return -frictionCoefficient * velocity;
}
