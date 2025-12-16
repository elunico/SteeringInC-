#ifndef FOOD_H
#define FOOD_H

#include "lifespan.h"
#include "utils.h"
#include "vec2d.h"
#include "world.h"

namespace tom {

class Vehicle;

struct Environmental {
    virtual ~Environmental() = default;
    using IdType             = World::FoodIdType;
    IdType   id;
    World*   world;
    Vec2D    position;
    Lifespan lifespan;

    Environmental(World* world, Vec2D const& pos, const Lifespan& ls) noexcept;

    [[nodiscard]] virtual Vec2D const& get_position() const noexcept;

    virtual void update() noexcept = 0;

    virtual void consume(Vehicle& consumer) noexcept = 0;

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
    // prevent the initial food from all disappearing at once
    double nutrition;
    DNA    dna{};

    Food() noexcept;

    Food(World* world, Vec2D const& pos) noexcept;

    Food(World*       world,
         Vec2D const& pos,
         double       nutrition,
         DNA const&   dna) noexcept;

    [[nodiscard]] double get_nutrition() const noexcept;

    [[nodiscard]] Vec2D const& get_position() const noexcept override;

    void update() noexcept override;

    void consume(Vehicle& consumer) noexcept override;

    void perform_explosion(World* world) const;

    void perform_spawn(World* world) const;

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