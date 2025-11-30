#ifndef DNA_H
#define DNA_H

struct DNA {
    double perceptionRadius;
    double maxSpeed;
    double mutationRate;
    double reproductionCost;
    double maliceDesire;  // New trait for cohesion behavior
    double altruismDesire;  // New trait for avoidance behavior
    double maliceProbability;
    double altruismProbability;
    double maliceDamage;
    double altruismHeal;
    int    reproductionCooldown;
    int    ageOfMaturity;

    DNA();

    [[nodiscard]] DNA crossover(DNA const& partner) const;
    void              mutate();
};

#endif  // DNA_H