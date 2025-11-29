#include "dna.h"
#include "utils.h"

DNA::DNA()
    : perceptionRadius(randomInRange(75, 200)),
      maxSpeed(randomInRange(0.5, 2)),
      mutationRate(0.05),
      reproductionCost(randomInRange(5, 15.0)),
      coherence(randomInRange(0.1, 1.0)),
      avoidance(randomInRange(0.1, 1.0)),
      maliceProbability(randomInRange(0.0, 0.3)),
      altruismProbability(randomInRange(0.0, 0.3)),
      maliceDamage(randomInRange(2.0, 15.0)),
      altruismHeal(randomInRange(2.0, 15.0)),
      reproductionCooldown(randomInRange(100, 250)),
      ageOfMaturity(randomInRange(20, 60))
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
    child.coherence     = (rand() % 2) ? coherence : partner.coherence;
    child.avoidance     = (rand() % 2) ? avoidance : partner.avoidance;
    child.maliceProbability =
        (rand() % 2) ? maliceProbability : partner.maliceProbability;
    child.altruismProbability =
        (rand() % 2) ? altruismProbability : partner.altruismProbability;
    child.maliceDamage = (rand() % 2) ? maliceDamage : partner.maliceDamage;
    child.altruismHeal = (rand() % 2) ? altruismHeal : partner.altruismHeal;
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
        coherence += randomDelta();
    }
    if ((rand() % 100) / 100.0 < mutationRate) {
        avoidance += randomDelta();
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
    if ((rand() % 100) / 100.0 < mutationRate) {
        maliceProbability += randomDelta() * 0.1;
    }
    if ((rand() % 100) / 100.0 < mutationRate) {
        altruismProbability += randomDelta() * 0.1;
    }
    if ((rand() % 100) / 100.0 < mutationRate) {
        maliceDamage += randomDelta();
    }
    if ((rand() % 100) / 100.0 < mutationRate) {
        altruismHeal += randomDelta();
    }
}
