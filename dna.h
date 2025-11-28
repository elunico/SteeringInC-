#ifndef DNA_H
#define DNA_H

struct DNA {
    double perceptionRadius;
    double maxSpeed;
    double mutationRate;
    double reproductionCost;
    int    reproductionCooldown;
    int    ageOfMaturity;

    DNA();

    [[nodiscard]] DNA crossover(DNA const& partner) const;
    void              mutate();
};

#endif  // DNA_H