#include "dna.h"
#include "utils.h"

namespace tom {

DNA::DNA() noexcept
    : perception_radius(random_in_range(40, 100)),
      max_speed(random_in_range(0.75, 3.0)),
      mutation_rate(0.1),
      reproduction_cost(random_in_range(5, 15.0)),
      malice_desire(random_in_range(0.01, 0.3)),
      altruism_desire(random_in_range(0.05, 0.5)),
      malice_probability(random_in_range(-0.1, 0.05)),
      altruism_probability(random_in_range(0.0, 0.1)),
      malice_damage(random_in_range(2.0, 5.0)),
      altruism_heal(random_in_range(2.0, 5.0)),
      explosion_chance(random_in_range(0.001, 0.005)),
      explosion_tries(random_in_range(2, 10)),
      reproduction_cooldown(random_int(500, 1000)),
      age_of_maturity(random_int(75, 200))
{
}

[[nodiscard]] DNA DNA::crossover(DNA const& partner) const noexcept
{
    DNA child;
    child.perception_radius =
        (random_bool()) ? perception_radius : partner.perception_radius;
    child.max_speed = (random_bool()) ? max_speed : partner.max_speed;
    child.mutation_rate =
        (random_bool()) ? mutation_rate : partner.mutation_rate;
    child.reproduction_cost =
        (random_bool()) ? reproduction_cost : partner.reproduction_cost;
    child.reproduction_cooldown =
        (random_bool()) ? reproduction_cooldown : partner.reproduction_cooldown;
    child.age_of_maturity =
        (random_bool()) ? age_of_maturity : partner.age_of_maturity;
    child.malice_desire =
        (random_bool()) ? malice_desire : partner.malice_desire;
    child.altruism_desire =
        (random_bool()) ? altruism_desire : partner.altruism_desire;
    child.malice_probability =
        (random_bool()) ? malice_probability : partner.malice_probability;
    child.altruism_probability =
        (random_bool()) ? altruism_probability : partner.altruism_probability;
    child.explosion_chance =
        (random_bool()) ? explosion_chance : partner.explosion_chance;
    child.explosion_tries =
        (random_bool()) ? explosion_tries : partner.explosion_tries;
    child.malice_damage =
        (random_bool()) ? malice_damage : partner.malice_damage;
    child.altruism_heal =
        (random_bool()) ? altruism_heal : partner.altruism_heal;
    return child;
}

void DNA::mutate() noexcept
{
    if (random_in_range(0, 1) < mutation_rate) {
        perception_radius += random_delta();
    }
    if (random_in_range(0, 1) < mutation_rate) {
        max_speed += random_delta();
    }
    if (random_in_range(0, 1) < mutation_rate) {
        malice_desire += random_delta();
    }
    if (random_in_range(0, 1) < mutation_rate) {
        altruism_desire += random_delta();
    }

    if (random_in_range(0, 1) < mutation_rate) {
        explosion_chance += random_delta() * 0.1;
    }
    if (random_in_range(0, 1) < mutation_rate) {
        explosion_tries += static_cast<int>(random_delta() * 5);
    }

    if (random_in_range(0, 1) < mutation_rate) {
        reproduction_cost += random_delta();
    }
    if (random_in_range(0, 1) < mutation_rate) {
        reproduction_cooldown += static_cast<int>(random_delta() * 5);
    }
    if (random_in_range(0, 1) < mutation_rate) {
        age_of_maturity += static_cast<int>(random_delta() * 5);
    }
    if (random_in_range(0, 1) < mutation_rate) {
        malice_probability += random_delta() * 0.1;
    }
    if (random_in_range(0, 1) < mutation_rate) {
        altruism_probability += random_delta() * 0.1;
    }
    if (random_in_range(0, 1) < mutation_rate) {
        malice_damage += random_delta();
    }
    if (random_in_range(0, 1) < mutation_rate) {
        altruism_heal += random_delta();
    }
}
}  // namespace tom