#include "../include/calibrator.h"

void Calibrator::linearRegression(double samplingPeriod_ms)
{
    if (samples.size() < 2) // Si moins de deux données on évite le crash
    {
        return;
    }
    
    double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
    int n = samples.size();
    
    for (int i = 0; i < n; i++) 
    {
        double x = i * samplingPeriod_ms; 
        double y = samples[i];
        sumX += x;
        sumY += y;
        sumXY += x * y;
        sumX2 += x * x;
    }
    
    // Formules de régression linéaire
    // a = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX)
    // b = (sumY - a * sumX) / n
    double denominator = n * sumX2 - sumX * sumX;
    if (std::abs(denominator) > 1e-10) 
    {
        a = (n * sumXY - sumX * sumY) / denominator;
        b = (sumY - a * sumX) / n;
    }
}

void Calibrator::callback()
{ 
    timer_cpt++;
    samples.push_back(looper.getSample());
    
    // Arrêter le looper une fois nSamples atteint
    if (timer_cpt >= nSamples) {
        looper.stopLoop();
        stop();
    }
}

Calibrator::Calibrator(double samplingPeriod_ms, unsigned int nSamples_) : nSamples(nSamples_)
{
    start_ms(samplingPeriod_ms, true);
    looper.runLoop(DBL_MAX);
    stop();
    
    // Calcul de la régression linéaire
    linearRegression(samplingPeriod_ms);
}

double Calibrator::nLoops(double duration_ms)
{
    return a * duration_ms + b;
}
