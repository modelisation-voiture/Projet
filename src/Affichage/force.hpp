#ifndef FORCE_HPP
#define FORCE_HPP

class Force {
public:
    float frictionCoefficient;

    Force();
    void setFriction(bool isGrass);
    float computeFriction(float velocity);
};

#endif // FORCE_HPP
