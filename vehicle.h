#ifndef VEHICLE_H
#define VEHICLE_H

#include "dna.h"
#include "utils.h"
#include "vec2d.h"
#include "world.h"

namespace tom {

template <typename T, typename U>
bool is_equal([[maybe_unused]] T* t, [[maybe_unused]] U* u)
{
    return false;
}

template <typename T>
bool is_equal(T* t, T* u)
{
    return t == u;
}

double find_distance(Vec2D const& a, Positionable auto const* p)
{
    return a.distance_to(p->get_position());
}

template <Positionable Obj, typename ID = typename Obj::IdType>
double find_distance(const Vec2D& a, std::pair<ID, Obj>& b)
{
    return a.distance_to(b.second.get_position());
}

class Vehicle {
   public:
    using IdType   = World::VehicleIdType;
    using Foods    = std::unordered_map<World::FoodIdType, Food>;
    using Vehicles = std::unordered_map<World::VehicleIdType, Vehicle>;

    explicit Vehicle(Vec2D const& position);
    Vehicle();

    [[nodiscard]] double       get_health() const;
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
                           double       health,
                           bool         verbose);

    template <class Container, typename T = Container::value_type>
    T* find_nearest(Container& items, double& out_distance)
    {
        T*     nearest = nullptr;
        double record  = std::numeric_limits<double>::infinity();

        for (auto& item : items) {
            if (is_equal(&item, this))  // if (item == this)
                continue;
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
    void                flee_poison(Food const* target, double record);
    void                seek_for_malice(Vehicle* target, double record);
    void                seek_for_altruism(Vehicle* target, double record);
    void                seek_for_reproduction(Vehicle* target, double record);
    [[nodiscard]] Vec2D seek(Vec2D const& target) const;
    Food&               last_sought_food(double& record) const;
    void                food_behaviors(Foods& food_positions);
    void                check_sought_vehicle();
    void                check_sought_food();
    void                vehicle_behaviors(Vehicles& vehicles);
    void                try_explosion();
    void                apply_force(Vec2D force, bool unlimited = false);
    void                perform_reproduction(Vehicle* mom, Vehicle* dad);
    void                perform_explosion(World* world);

    World* world                        = nullptr;
    double health                       = 20.0;
    int    age                          = 0;
    double mass                         = 1.0;
    int    time_since_last_reproduction = -1;
    int    generation                   = 0;
    DNA    dna;

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
