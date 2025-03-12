#ifndef ForceMOTRICE_HPP
#define ForceMOTRICE_HPP

#include "Force.hpp"

class ForceMotrice : public Force {
private:
    double puissanceMoteur;

public:
    ForceMotrice(double puissance);
    double calculer() override;
};

#endif // ForceMOTRICE_HPP
