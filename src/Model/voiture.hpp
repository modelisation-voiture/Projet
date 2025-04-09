class Voiture {
    private:
        double x, y;
        double angle;
        double vx, vy;
        double ax, ay;
        double masse;
        double empattement; // Distance entre roues avant et arrière
        bool frein_actif = false;
        bool frein_main_actif = false;
        bool accelerationActive = false;



    
    public:
        Voiture(double x_init, double y_init, double angle_deg, double masse, double empattement);


        void appliquerForce(double fx, double fy);
        void updatePositionRK4(double dt, double fx, double fy, double coeff_frottement, double angle_braquage_deg);
    
        // Ajout pour virage :
        double calculerRayonCourbure(double angle_braquage_deg) const;
        bool isFreinActif() const { return frein_actif; }
        void activerFrein(bool actif) { frein_actif = actif; }

        void setAccelerationActive(bool value) { accelerationActive = value; }
        bool isAccelerationActive() const { return accelerationActive; }

        
    
        double getX() const;
        double getY() const;
        void setX(double x_val) { x = x_val; }       
        void setY(double y_val) { y = y_val; }

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
        void setFreinMainActif(bool actif) { frein_main_actif = actif; }
        bool isFreinMainActif() const { return frein_main_actif; }

        

    };
    