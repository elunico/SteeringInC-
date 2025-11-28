#include "dna.h"
#include "utils.h"

DNA::DNA()
    : perceptionRadius(randomInRange(20, 60)),
      maxSpeed(randomInRange(0.5, 3)),
      mutationRate(0.01),
      reproductionCost(randomInRange(1.0, 5.0)),
      reproductionCooldown(randomInRange(40, 60)),
      ageOfMaturity(randomInRange(15, 30))
{
}

[[nodiscard]] DNA DNA::crossover(DNA const& partner) const
{
    DNA child;
    child.perceptionRadius =
        (rand() % 2) ? perceptionRadius : partner.perceptionRadius;
    child.maxSpeed     = (rand() % 2) ? maxSpeed : partner.maxSpeed;
    child.mutationRate = (rand() % 2) ? mutationRate : partner.mutationRate;
    child.reproductionCost =
        (rand() % 2) ? reproductionCost : partner.reproductionCost;
    child.reproductionCooldown =
        (rand() % 2) ? reproductionCooldown : partner.reproductionCooldown;
    child.ageOfMaturity = (rand() % 2) ? ageOfMaturity : partner.ageOfMaturity;
    return child;
}

void DNA::mutate()
{
    if ((rand() % 100) / 100.0 < mutationRate) {
        perceptionRadius += randomDelta();
    }
    if ((rand() % 100) / 100.0 < mutationRate) {
        maxSpeed += randomDelta();
    }

    if ((rand() % 100) / 100.0 < mutationRate) {
        reproductionCost += randomDelta();
    }
    if ((rand() % 100) / 100.0 < mutationRate) {
        reproductionCooldown += static_cast<int>(randomDelta() * 5);
    }
    if ((rand() % 100) / 100.0 < mutationRate) {
        ageOfMaturity += static_cast<int>(randomDelta() * 5);
    }
}
