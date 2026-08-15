#ifndef FOODDNA_H
#define FOODDNA_H

#include "basedna.h"

namespace tom {
struct FoodDNA : public BaseDNA<FoodDNA> {
    double nutrition;
    double lifeticks;
    double speed;
    double explosionChance;
    double explosionCount;
    double mutationRate;
    double perceptionRadius;
    double fleeChance;
    double fleeStrength;

    FoodDNA();

    [[nodiscard]] virtual FoodDNA crossover(
        FoodDNA const& partner) const noexcept;
    virtual void mutate() noexcept;
};
}  // namespace tom

#endif
