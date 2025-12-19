#ifndef VEHICLE_H
#define VEHICLE_H

#include "dna.h"
#include "lifespan.h"
#include "utils.h"
#include "vec2d.h"
#include "world.h"

namespace tom {

static inline double find_distance(Vec2D const& a, Positionable auto const& p)
{
    return a.distance_to(p.get_position());
}

template <Positionable Obj, typename ID = typename Obj::IdType>
static inline double find_distance(const Vec2D& a, std::pair<ID, Obj>& b)
{
    return a.distance_to(b.second.get_position());
}

class Vehicle {
   public:
    using IdType       = World::VehicleIdType;
    using Foods        = World::Foods;
    using Vehicles     = World::Vehicles;
    using LifespanType = Lifespan<double, 0.05>;

    explicit Vehicle(Vec2D const& position);
    Vehicle();

    [[nodiscard]] LifespanType get_health() const;
    [[nodiscard]] int          get_age() const;
    [[nodiscard]] double       get_fitness() const;
    [[nodiscard]] DNA const&   get_dna() const;
    [[nodiscard]] Vec2D const& get_position() const;
    [[nodiscard]] Vec2D const& get_velocity() const;
    [[nodiscard]] int          get_generation() const;
    [[nodiscard]] Vec2D const& get_acceleration() const;
    [[nodiscard]] Vehicle&     last_sought_vehicle() const;
    [[nodiscard]] Vehicle&     last_sought_vehicle(double& record) const;
    [[nodiscard]] Food&        last_sought_food() const;
    [[nodiscard]] IdType       get_last_sought_vehicle_id() const;
    [[nodiscard]] bool         is_verbose() const;
    [[nodiscard]] bool         can_see(Vec2D const& target) const;
    [[nodiscard]] bool         will_seek_vehicle() const;
    [[nodiscard]] bool         can_touch(Vec2D const& target) const;
    [[nodiscard]] bool         can_see(double distance) const;
    [[nodiscard]] bool         can_touch(double distance) const;
    [[nodiscard]] bool         is_dead() const;
    void                       update();
    void                       kill();
    void                       avoid_edges();
    void behaviors(Vehicles& vehicles, Foods& food_positions);
    void populate_in_place(IdType       id,
                           World*       world,
                           Vec2D const& position,
                           Vec2D const& velocity,
                           DNA const&   dna,
                           int          generation,
                           LifespanType health,
                           bool         verbose);

    template <class Container, typename T = Container::value_type::second_type>
    T* find_nearest(Container& items, double& out_distance)
    {
        T*     nearest = nullptr;
        double record  = std::numeric_limits<double>::infinity();

        for (auto& [item_id, item] : items) {
            if constexpr (std::is_same_v<T, decltype(item)>) {
                if (item_id == id) {
                    continue;
                };
            }
            auto distance = find_distance(position, item);
            if (distance < record) {
                record  = distance;
                nearest = &item;
            }
        }

        out_distance = record;
        return nearest;
    }

    [[nodiscard]] bool is_less_fit(Vehicle const& other) const;

   private:
    static IdType       global_id_counter;
    void                seek_for_eat(Food* target, double record);
    void                flee_poison(Food* target, double record);
    void                seek_for_malice(Vehicle* target, double record);
    void                seek_for_altruism(Vehicle* target, double record);
    void                seek_for_reproduction(Vehicle* target, double record);
    [[nodiscard]] Vec2D seek(Vec2D const& target) const;
    [[nodiscard]] Vec2D flee(Vec2D const& target) const;
    Food&               last_sought_food(double& record) const;
    void                food_behaviors(Foods& food_positions);
    void                check_sought_vehicle();
    void                check_sought_food();
    void                vehicle_behaviors(Vehicles& vehicles);
    void                try_explosion();
    void                apply_force(Vec2D force, bool unlimited = false);
    void perform_reproduction(Vehicle const* mom, Vehicle const* dad) const;
    void perform_explosion(World* world) const;

    World* world = nullptr;
    // double health                       = 20.0;
    LifespanType health                       = 20.0;
    int          age                          = 0;
    double       mass                         = 1.0;
    int          time_since_last_reproduction = -1;
    int          generation                   = 0;
    DNA          dna;

    Vec2D position;
    Vec2D velocity;
    Vec2D acceleration{};

   public:
    static decltype(global_id_counter) next_id()
    {
        return ++global_id_counter;
    }

    IdType            id;
    IdType            last_sought_vehicle_id = 0;
    World::FoodIdType last_sought_food_id    = 0;
    bool              verbose                = false;
    bool              highlighted            = false;

    friend struct World;
    friend struct Food;
};

}  // namespace tom

#endif  // VEHICLE_H
