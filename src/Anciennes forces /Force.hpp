#ifndef FORCE_HPP
#define FORCE_HPP

class Force {
protected:
    double valeur;

public:
    virtual double calculer() = 0;
    virtual ~Force() {}
};

#endif // FORCE_HPP
