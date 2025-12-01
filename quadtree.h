#ifndef QUADTREE_H
#define QUADTREE_H

#include <concepts>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include "rectangle.h"
#include "utils.h"
#include "vec2d.h"

class Vehicle;

template <typename T>
concept Positionable = requires(T a) {
    { a.getPosition() } -> std::convertible_to<Vec2D>;
};

template <typename S>
concept Shapeable = requires(S a, Vec2D t) {
    { a.contains(t) } -> std::convertible_to<bool>;
    { a.intersects(a) } -> std::convertible_to<bool>;
};

template <typename Contained, Shapeable Shape>
struct QuadTree {
    Shape                   boundary;
    std::size_t             capacity;
    std::vector<Contained*> points{};
    bool                    divided = false;

    std::unique_ptr<QuadTree> northWest{};
    std::unique_ptr<QuadTree> northEast{};
    std::unique_ptr<QuadTree> southWest{};
    std::unique_ptr<QuadTree> southEast{};

    QuadTree() : boundary(Shape()), capacity(0)
    {
    }

    QuadTree(Rectangle boundary, int capacity)
        : boundary(boundary), capacity(capacity)
    {
    }

    QuadTree(QuadTree&& other) = default;
    QuadTree& operator=(QuadTree&& other)
    {
        boundary  = other.boundary;
        capacity  = other.capacity;
        points    = std::move(other.points);
        divided   = other.divided;
        northWest = std::move(other.northWest);
        northEast = std::move(other.northEast);
        southWest = std::move(other.southWest);
        southEast = std::move(other.southEast);
        return *this;
    }

    std::vector<Contained*> all() const
    {
        return query(boundary);
    }

    std::vector<Contained*> query(Rectangle const& range) const
    {
        std::vector<Contained*> found{};
        query(range, found);
        return found;
    }

    [[nodiscard]] bool contains(Vec2D const& point) const
    {
        return boundary.contains(point);
    }

    bool insert(Contained* item)
    {
        if (!contains(item->getPosition())) {
            return false;
        }

        if constexpr (std::is_same_v<Contained, Vehicle>) {
            if (item->verbose) {
                output("Inserting highlighted vehicle into quadtree\n");
            }
        }

        if (points.size() < capacity) {
            points.push_back(item);
            return true;
        }

        if (!divided) {
            subdivide();
        }

        // Insert into appropriate quadrant
        if (northWest->insert(item))
            return true;
        if (northEast->insert(item))
            return true;
        if (southWest->insert(item))
            return true;
        if (southEast->insert(item))
            return true;

        UNREACHABLE();
    }

    void query(Rectangle const& range, std::vector<Contained*>& found) const
    {
        if (!boundary.intersects(range)) {
            return;
        }

        for (const auto& p : points) {
            if (range.contains(p->getPosition())) {
                found.push_back(p);
            }
        }

        if (divided) {
            northWest->query(range, found);
            northEast->query(range, found);
            southWest->query(range, found);
            southEast->query(range, found);
        }
    }

    void subdivide()
    {
        Vec2D center((boundary.topLeft.x + boundary.bottomRight.x) / 2.0,
                     (boundary.topLeft.y + boundary.bottomRight.y) / 2.0);

        northWest = std::make_unique<QuadTree>(
            Rectangle(boundary.topLeft, center), capacity);
        northEast = std::make_unique<QuadTree>(
            Rectangle(Vec2D(center.x, boundary.topLeft.y),
                      Vec2D(boundary.bottomRight.x, center.y)),
            capacity);
        southWest = std::make_unique<QuadTree>(
            Rectangle(Vec2D(boundary.topLeft.x, center.y),
                      Vec2D(center.x, boundary.bottomRight.y)),
            capacity);
        southEast = std::make_unique<QuadTree>(
            Rectangle(center, boundary.bottomRight), capacity);

        divided = true;
    }
};

#endif  // QUADTREE_H