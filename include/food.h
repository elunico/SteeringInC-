#ifndef FOOD_H
#define FOOD_H

#include "fooddna.h"
#include "lifespan.h"
#include "utils.h"
#include "vec2d.h"
#include "world.h"

namespace tom {

class Vehicle;

struct Environmental {
    virtual ~Environmental() = default;
    using IdType             = World::FoodIdType;

    IdType           id;
    World*           world;
    Vec2D            position;
    Vec2D            velocity{};
    Vec2D            acceleration{};
    Lifespan<int, 1> lifespan;

    template <typename T, T tick_amt>
    Environmental(World*                       world,
                  Vec2D const&                 pos,
                  const Lifespan<T, tick_amt>& ls) noexcept
        : id(next_id()), world(world), position(pos), lifespan(ls)
    {
    }

    [[nodiscard]] virtual Vec2D const& get_position() const noexcept;

    virtual void update() noexcept = 0;

    virtual void consume(Vehicle& consumer) noexcept = 0;

    virtual void expire() noexcept = 0;

    [[nodiscard]] virtual bool is_expired() const noexcept;

    static IdType next_id() noexcept
    {
        return ++global_id_counter;
    }

   private:
    static IdType global_id_counter;
};

struct Food : Environmental {
    using IdType = World::FoodIdType;
    double velocity_dampening;

    FoodDNA dna{};

    Food() noexcept;

    Food(World* world, Vec2D const& pos) noexcept;

    Food(World* world, Vec2D const& pos, FoodDNA const& dna) noexcept;

    [[nodiscard]] double get_nutrition() const noexcept;

    [[nodiscard]] Vec2D const& get_position() const noexcept override;

    void update() noexcept override;

    void dampen_velocity();

    void consume(Vehicle& consumer) noexcept override;

    void try_flee(Vehicle const& source) noexcept;

    void apply_force(Vec2D const& force);

    bool can_see(Vec2D const& position) const noexcept;

    void expire() noexcept override;

    void behaviors(World::Vehicles const& vehicles);

    void perform_explosion(World* world) const;

    void perform_spawn(World* world) const;

    void avoid_edges() noexcept;

    static IdType next_id() noexcept
    {
        return ++global_id_counter;
    }

   private:
    Vec2D         velocity = Vec2D::random(0.25);
    static IdType global_id_counter;
};

}  // namespace tom

#endif  // FOOD_H
