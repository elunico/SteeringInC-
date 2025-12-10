#ifndef FOOD_H
#define FOOD_H

#include "utils.h"
#include "vec2d.h"
#include "world.h"

namespace tom {

class Vehicle;

class Lifespan {
    int  remaining_ticks;
    bool unlimited_ = false;

   public:
    Lifespan(int ticks) noexcept;

    static Lifespan unlimited() noexcept
    {
        Lifespan ls(0);
        ls.unlimited_ = true;
        return ls;
    }

    static Lifespan random(int min_ticks, int max_ticks) noexcept
    {
        return Lifespan(static_cast<int>(random_in_range(
            static_cast<double>(min_ticks), static_cast<double>(max_ticks))));
    }

    Lifespan& operator--() noexcept;

    Lifespan& operator--(int) noexcept;

    void update() noexcept;

    virtual void expire() noexcept;
    virtual void expire(bool force) noexcept;

    [[nodiscard]] bool is_expired() const noexcept;

    virtual ~Lifespan();
};

struct Environmental {
    virtual ~Environmental() = default;
    using IdType             = World::FoodIdType;
    IdType   id;
    World*   world;
    Vec2D    position;
    Lifespan lifespan;

    Environmental(World* world, Vec2D const& pos, Lifespan ls) noexcept;

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

template <typename UpdateFn, typename ConsumeFn>
struct CustomEnvironmental : public Environmental {
    UpdateFn  update_fn;
    ConsumeFn consume_fn;
    CustomEnvironmental(World*       world,
                        Vec2D const& pos,
                        Lifespan     ls,
                        UpdateFn     uf,
                        ConsumeFn    cf) noexcept
        : Environmental(world, pos, ls),
          update_fn(std::move(uf)),
          consume_fn(std::move(cf))
    {
    }

    void update() noexcept override
    {
        update_fn(*this);
    }

    void consume(Vehicle& consumer) noexcept override
    {
        consume_fn(*this, consumer);
    }
};

struct Food : public Environmental {
    // prevent the initial food from all disappearing at once
    double nutrition;

    Food() noexcept;

    Food(World* world, Vec2D const& pos) noexcept;

    Food(World* world, Vec2D const& pos, double nutrition) noexcept;

    [[nodiscard]] double get_nutrition() const noexcept;

    [[nodiscard]] Vec2D const& get_position() const noexcept;

    void update() noexcept;

    void consume(Vehicle& consumer) noexcept;

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