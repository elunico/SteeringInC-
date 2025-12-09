#ifndef DNA_H
#define DNA_H

namespace tom {

struct DNA {
    double perception_radius;
    double max_speed;
    double mutation_rate;
    double reproduction_cost;
    double malice_desire;
    double altruism_desire;
    double malice_probability;
    double altruism_probability;
    double malice_damage;
    double altruism_heal;
    double explosion_chance;
    double explosion_tries;
    int    reproduction_cooldown;
    int    age_of_maturity;

    DNA() noexcept;

    [[nodiscard]] DNA crossover(DNA const& partner) const noexcept;
    void              mutate() noexcept;
};

}  // namespace tom

#endif  // DNA_H