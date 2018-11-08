#include "Physics.h"
#include "Components.h"

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	Vec2 delta(abs((a->getComponent<CTransform>()->pos.x) - (b->getComponent<CTransform>()->pos.x)), (abs((a->getComponent<CTransform>()->pos.y) - (b->getComponent<CTransform>()->pos.y))));

	float ox = (a->getComponent<CBoundingBox>()->halfSize.x) + (b->getComponent<CBoundingBox>()->halfSize.x) - delta.x;
	float oy = (a->getComponent<CBoundingBox>()->halfSize.y) + (b->getComponent<CBoundingBox>()->halfSize.y) - delta.y;

	return Vec2(ox, oy);
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
