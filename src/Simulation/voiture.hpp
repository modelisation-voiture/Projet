class Voiture {
    private:
        double x, y;
        double angle;
        double vx, vy;
        double ax, ay;
        double masse;
        double empattement; // Distance entre roues avant et arrière
        bool frein_actif = false;

    
    public:
        Voiture(double x_init, double y_init, double angle_deg, double masse, double empattement);


        void appliquerForce(double fx, double fy);
        void updatePositionRK4(double dt, double fx, double fy, double coeff_frottement, double angle_braquage_deg);
    
        // Ajout pour virage :
        double calculerRayonCourbure(double angle_braquage_deg) const;
        bool isFreinActif() const { return frein_actif; }
        void activerFrein(bool actif) { frein_actif = actif; }
    
        double getX() const;
        double getY() const;
        double getAngle() const;
        double getVitesse() const;
        double setVitesse(double v);
        double getVitesseX() const;
        double getVitesseY() const;
        double getAcceleration() const;
        double getVx() const;
        double getVy() const;
        double getMasse() const ;
        double getEmpattement() const ;
        double getVitesseLacet() const;
        double setVitesseLacet(double vpsi);
        

    };
    