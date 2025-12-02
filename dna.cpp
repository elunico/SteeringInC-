#include "dna.h"
#include "utils.h"

DNA::DNA()
    : perception_radius(random_in_range(40, 100)),
      max_speed(random_in_range(1, 4)),
      mutation_rate(0.1),
      reproduction_cost(random_in_range(5, 15.0)),
      malice_desire(random_in_range(0.01, 0.3)),
      altruism_desire(random_in_range(0.05, 0.5)),
      malice_probability(random_in_range(-0.1, 0.05)),
      altruism_probability(random_in_range(0.0, 0.1)),
      malice_damage(random_in_range(2.0, 5.0)),
      altruism_heal(random_in_range(2.0, 5.0)),
      reproduction_cooldown(random_in_range(500, 1000)),
      age_of_maturity(random_in_range(75, 200))
{
}

[[nodiscard]] DNA DNA::crossover(DNA const& partner) const
{
    DNA child;
    child.perception_radius =
        (rand() % 2) ? perception_radius : partner.perception_radius;
    child.max_speed     = (rand() % 2) ? max_speed : partner.max_speed;
    child.mutation_rate = (rand() % 2) ? mutation_rate : partner.mutation_rate;
    child.reproduction_cost =
        (rand() % 2) ? reproduction_cost : partner.reproduction_cost;
    child.reproduction_cooldown =
        (rand() % 2) ? reproduction_cooldown : partner.reproduction_cooldown;
    child.age_of_maturity =
        (rand() % 2) ? age_of_maturity : partner.age_of_maturity;
    child.malice_desire = (rand() % 2) ? malice_desire : partner.malice_desire;
    child.altruism_desire =
        (rand() % 2) ? altruism_desire : partner.altruism_desire;
    child.malice_probability =
        (rand() % 2) ? malice_probability : partner.malice_probability;
    child.altruism_probability =
        (rand() % 2) ? altruism_probability : partner.altruism_probability;
    child.malice_damage = (rand() % 2) ? malice_damage : partner.malice_damage;
    child.altruism_heal = (rand() % 2) ? altruism_heal : partner.altruism_heal;
    return child;
}

void DNA::mutate()
{
    if ((rand() % 100) / 100.0 < mutation_rate) {
        perception_radius += random_delta();
    }
    if ((rand() % 100) / 100.0 < mutation_rate) {
        max_speed += random_delta();
    }
    if ((rand() % 100) / 100.0 < mutation_rate) {
        malice_desire += random_delta();
    }
    if ((rand() % 100) / 100.0 < mutation_rate) {
        altruism_desire += random_delta();
    }

    if ((rand() % 100) / 100.0 < mutation_rate) {
        reproduction_cost += random_delta();
    }
    if ((rand() % 100) / 100.0 < mutation_rate) {
        reproduction_cooldown += static_cast<int>(random_delta() * 5);
    }
    if ((rand() % 100) / 100.0 < mutation_rate) {
        age_of_maturity += static_cast<int>(random_delta() * 5);
    }
    if ((rand() % 100) / 100.0 < mutation_rate) {
        malice_probability += random_delta() * 0.1;
    }
    if ((rand() % 100) / 100.0 < mutation_rate) {
        altruism_probability += random_delta() * 0.1;
    }
    if ((rand() % 100) / 100.0 < mutation_rate) {
        malice_damage += random_delta();
    }
    if ((rand() % 100) / 100.0 < mutation_rate) {
        altruism_heal += random_delta();
    }
}
