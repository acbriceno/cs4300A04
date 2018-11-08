#include "Physics.h"
#include "Components.h"

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    return Vec2(0, 0);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
    return Vec2(0, 0);
}

Intersect Physics::LineIntersect(const Vec2 & a, const Vec2 & b, const Vec2 & c, const Vec2 & d)
{    
    return { false, Vec2(0,0) };
}

bool Physics::EntityIntersect(const Vec2 & a, const Vec2 & b, std::shared_ptr<Entity> e)
{
    return false;
}