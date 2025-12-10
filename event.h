
#ifndef EVENT_H
#define EVENT_H
#include "vec2d.h"

namespace tom {

struct World;

template <typename Source, typename Target>
class Event {
   public:
    enum class Type {
        VEHICLE_CREATE,  // Create a new vehicle
        VEHICLE_BORN,    // A vehicle has been born
        VEHICLE_DEAD,    // A vehicle has died
        FOOD_CONSUMED,   // Food has been consumed
        FOOD_CREATED,    // New food has been created
        FOOD_EXPIRED,    // Food has expired
        NONE             // No event
    };

    Type    type;
    Vec2D   position;
    World*  world;
    Source* source;
    Target* target;

    Event()
        : type(Type::NONE),
          position(Vec2D()),
          world(nullptr),
          source(nullptr),
          target(nullptr)
    {
    }

    Event(Type         type,
          Vec2D const& position,
          World*       world,
          Source*      source = nullptr,
          Target*      target = nullptr)
        : type(type),
          position(position),
          world(world),
          source(source),
          target(target)
    {
    }

    virtual ~Event()
    {
    }
};

class AnyEvent {
   private:
    struct Holder {
        virtual ~Holder() = default;
    };

    template <typename Source, typename Target>
    struct TypedHolder : Holder {
        Event<Source, Target> event;
        TypedHolder(const Event<Source, Target>& e) : event(e)
        {
        }
    };

    std::unique_ptr<Holder> holder;

   public:
    AnyEvent() = default;

    template <typename Source, typename Target>
    AnyEvent(const Event<Source, Target>& event)
        : holder(std::make_unique<TypedHolder<Source, Target>>(event))
    {
    }

    template <typename Source, typename Target>
    Event<Source, Target>* get()
    {
        auto* typed = dynamic_cast<TypedHolder<Source, Target>*>(holder.get());
        return typed ? &typed->event : nullptr;
    }

    template <typename Source, typename Target>
    const Event<Source, Target>* get() const
    {
        auto* typed =
            dynamic_cast<const TypedHolder<Source, Target>*>(holder.get());
        return typed ? &typed->event : nullptr;
    }
};

}  // namespace tom
#endif  // EVENT_H