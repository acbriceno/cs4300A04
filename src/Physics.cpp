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
	Vec2 aPP = a->getComponent<CTransform>()->pos - a->getComponent<CTransform>()->speed;
	Vec2 bPP = b->getComponent<CTransform>()->pos - b->getComponent<CTransform>()->speed;
	Vec2 delta(abs((aPP.x) - (bPP.x)), (abs((aPP.y) - (bPP.y))));
	
	float ox = (a->getComponent<CBoundingBox>()->halfSize.x) + (b->getComponent<CBoundingBox>()->halfSize.x) - delta.x;
	float oy = (a->getComponent<CBoundingBox>()->halfSize.y) + (b->getComponent<CBoundingBox>()->halfSize.y) - delta.y;
	
	return Vec2(ox, oy);
}

Intersect Physics::LineIntersect(const Vec2 & a, const Vec2 & b, const Vec2 & c, const Vec2 & d)
{    
	Vec2 r = (b - a);
	Vec2 s = (d - c);
	float rxs = r.cross(s);
	Vec2 cma = c - a;
	float t = cma.cross(s) / rxs;
	float u = cma.cross(r) / rxs;
	if (t >= 0 && t <= 1 && u >= 0 && u <= 1) 
	{
		return { true, Vec2(a.x + t * r.x,a.y + t * r.y) };
	}
	else{return { false, Vec2(0,0) };}
}

bool Physics::EntityIntersect(const Vec2 & a, const Vec2 & b, std::shared_ptr<Entity> e)
{
	std::vector<Vec2> vecs;
	
	vecs.push_back(e->getComponent<CTransform>()->pos + Vec2(-32,-32));
	vecs.push_back(e->getComponent<CTransform>()->pos + Vec2(32,-32));
	vecs.push_back(e->getComponent<CTransform>()->pos + Vec2(32, 32));
	vecs.push_back(e->getComponent<CTransform>()->pos + Vec2(-32,32));
	Intersect  v1 = Physics::LineIntersect(a, b, vecs[0], vecs[1]);
	Intersect  v2 = Physics::LineIntersect(a, b, vecs[1], vecs[2]);
	Intersect  v3 = Physics::LineIntersect(a, b, vecs[2], vecs[3]);
	Intersect  v4 = Physics::LineIntersect(a, b, vecs[3], vecs[0]);
	if (v1.result || v2.result || v3.result || v4.result) { return true; }
	else { return false; }
}

bool Physics:: PointInBounds(Vec2 mousePosition, std::shared_ptr<Entity> player)
{
    
    return !(mousePosition.x < (player->getComponent<CTransform>()->pos.x - player->getComponent<CBoundingBox>()->halfSize.x) || mousePosition.x > (player->getComponent<CTransform>()->pos.x + player->getComponent<CBoundingBox>()->halfSize.x) || mousePosition.y < (player->getComponent<CTransform>()->pos.y - player->getComponent<CBoundingBox>()->halfSize.y) || mousePosition.y > (player->getComponent<CTransform>()->pos.y + player->getComponent<CBoundingBox>()->halfSize.y));
    
}
