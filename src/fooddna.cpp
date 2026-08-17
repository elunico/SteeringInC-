#include "fooddna.h"
#include "utils.h"

namespace tom {
FoodDNA::FoodDNA()
    // nutrition is now a percentage of max health not an absolute value
    : nutrition(random_in_range(0.05, 0.2)),
      lifeticks(random_in_range(300, 1000)),
      speed(random_in_range(1, 3)),
      explosionChance(random_in_range(0.01, 0.1)),
      explosionCount(random_in_range(5, 15)),
      mutationRate(0.1),
      perceptionRadius(50),
      fleeChance(0.1),
      fleeStrength(1.5)
{
}

FoodDNA FoodDNA::crossover(FoodDNA const& other) const noexcept
{
    FoodDNA child;
    child.nutrition = random_bool() ? nutrition : other.nutrition;
    child.lifeticks = random_bool() ? lifeticks : other.lifeticks;
    child.speed     = random_bool() ? speed : other.speed;
    child.explosionChance =
        random_bool() ? explosionChance : other.explosionChance;
    child.explosionCount =
        random_bool() ? explosionCount : other.explosionCount;
    child.perceptionRadius =
        random_bool() ? perceptionRadius : other.perceptionRadius;
    child.fleeChance   = random_bool() ? fleeChance : other.fleeChance;
    child.fleeStrength = random_bool() ? fleeStrength : other.fleeStrength;
    return child;
}

void FoodDNA::mutate() noexcept
{
    if (random_in_range(0, 1) < mutationRate) {
        nutrition += random_delta(0.01);
    }
    if (random_in_range(0, 1) < mutationRate) {
        lifeticks += random_delta(20);
    }
    if (random_in_range(0, 1) < mutationRate) {
        speed += random_delta(0.01);
    }
    if (random_in_range(0, 1) < mutationRate) {
        explosionChance += random_delta(0.02);
    }
    if (random_in_range(0, 1) < mutationRate) {
        explosionCount += random_delta(1);
    }
    if (random_in_range(0, 1) < mutationRate) {
        perceptionRadius += random_delta(2);
    }
    if (random_in_range(0, 1) < mutationRate) {
        fleeChance += random_delta(0.01);
    }
    if (random_in_range(0, 1) < mutationRate) {
        fleeStrength += random_delta(0.1);
    }
}

}  // namespace tom
