#include "../include/looper.h"
#include "../include/chrono.h"
#include "../include/calibrator.h"
#include <iostream>

int main()
{
    Looper looper;
    Chrono chrono;
    Calibrator calibrator(200.0, 50); // On prend une dizaine d'échantillons tous les 200ms pour la calibration
    
    std::cout << "Calibration finie" << std::endl;
    std::cout << "Durée (ms) | Estimation de nLoops | Durée mesurée (ms) | Erreur (%)" << std::endl;

    for (double duration_ms = 500.0; duration_ms <= 10000.0; duration_ms += 200.0) {
        double estimatedLoops = calibrator.nLoops(duration_ms); // On estime le nombre de boucles pour cette durée
        
        chrono.restart();
        looper.runLoop(estimatedLoops);
        double measuredDuration_ms = timespec_to_ms(chrono.stop());
        
        // Calcul de l'erreur
        double error = ((measuredDuration_ms - duration_ms) / duration_ms) * 100.0;
        
        // Afficher les résultats
        std::cout << duration_ms << " | " << estimatedLoops << " | " << measuredDuration_ms << " | " << error << "%" << std::endl;
    }
    
    return 0;
}