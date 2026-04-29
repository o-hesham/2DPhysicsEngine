#include "Physics/Shape.h"
#include <iostream>
#include <limits>

///////////////////////////////// Circles /////////////////////////////////
CircleShape::CircleShape(float radius)
{
    this->radius = radius;
    std::cout << "CircleShape constructor called!" << std::endl;
}

CircleShape::~CircleShape()
{
    std::cout << "CircleShape destructor called!" << std::endl;
}

ShapeType CircleShape::GetType() const
{
    return CIRCLE;
}

float CircleShape::GetMomentOfInertia() const
{
    // Inertia for circles is 1/2 * r^2 * mass(multiplication by mass happens in body.cpp)
    return 0.5 * (radius * radius);
}

void CircleShape::UpdateVertices(float angle, const Vec2 &position)
{
    return; // Circles dont have vertices... nothing to do here
}

///////////////////////////////// Polygons /////////////////////////////////
PolygonShape::PolygonShape(const std::vector<Vec2> vertices)
{
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();

    // Initialize the vertices of the polygon shape and set width and height
    for (auto vertex : vertices)
    {
        localVertices.push_back(vertex);
        worldVertices.push_back(vertex);

        // Find min and max X and Y to calculate polygon width and height
        minX = std::min(minX, vertex.x);
        maxX = std::max(maxX, vertex.x);
        minY = std::min(minY, vertex.y);
        maxY = std::max(maxY, vertex.y);
    }

    width = maxX - minX;
    height = maxY - minY;
    std::cout << "PolygonShape constructor called!" << std::endl;
};

PolygonShape::~PolygonShape()
{
    std::cout << "PolygonShape destructor called!" << std::endl;
}

ShapeType PolygonShape::GetType() const
{
    return POLYGON;
}

float PolygonShape::PolygonArea() const
{
    float area = 0.0;
    for (int i = 0; i < localVertices.size(); i++)
    {
        int j = (i + 1) % localVertices.size();
        area += localVertices[i].Cross(localVertices[j]);
    }
    return area / 2.0;
}

Vec2 PolygonShape::PolygonCentroid() const
{
    Vec2 cg{0, 0};
    for (int i = 0; i < localVertices.size(); i++)
    {
        int j = (i + 1) % localVertices.size();
        cg += (localVertices[i] + localVertices[j]) * localVertices[i].Cross(localVertices[j]);
    }
    return cg / 6 / PolygonArea();
}

float PolygonShape::GetMomentOfInertia() const
{
    float acc0 = 0;
    float acc1 = 0;
    for (int i = 0; i < localVertices.size(); i++)
    {
        auto a = localVertices[i];
        auto b = localVertices[(i + 1) % localVertices.size()];
        auto cross = abs(a.Cross(b));
        acc0 += cross * (a.Dot(a) + b.Dot(b) + a.Dot(b));
        acc1 += cross;
    }
    return acc0 / 6 / acc1;
}

Vec2 PolygonShape::EdgeAt(int index) const
{
    int currVertex = index;
    int nextVertex = (index + 1) % worldVertices.size();

    return worldVertices[nextVertex] - worldVertices[currVertex];
}

float PolygonShape::FindMinSeparation(const PolygonShape *other, int &indexReferenceEdge, Vec2 &supportPoint) const
{
    float seperation = std::numeric_limits<float>::lowest();

    for (int i = 0; i < worldVertices.size(); i++) // Loop all the vertices of this
    {
        Vec2 va = worldVertices[i];
        Vec2 normal = EdgeAt(i).Normal();

        float minSep = std::numeric_limits<float>::max();
        Vec2 minVertex;

        for (int j = 0; j < other->worldVertices.size(); j++) // Loop all the vertices of other
        {
            Vec2 vb = other->worldVertices[j];
            float proj = (vb - va).Dot(normal);
            if (proj < minSep)
            {
                minSep = proj;
                minVertex = vb;
            }
        }
        if (minSep > seperation)
        {
            seperation = minSep;
            indexReferenceEdge = i;
            supportPoint = minVertex;
        }
    }

    return seperation;
}

int PolygonShape::FindIncidentEdge(const Vec2 &normal) const
{
    int indexIncidentEdge;
    float minProj = std::numeric_limits<float>::max();
    for (size_t i = 0; i < this->worldVertices.size(); i++)
    {
        auto edgeNormal = this->EdgeAt(i).Normal();
        auto proj = edgeNormal.Dot(normal);

        if (proj < minProj)
        {
            minProj = proj;
            indexIncidentEdge = i;
        }
    }
    return indexIncidentEdge;
}

int PolygonShape::ClipSegmentToLine(const std::vector<Vec2> &contactsIn, std::vector<Vec2> &contactsOut, const Vec2 &c0, const Vec2 &c1) const
{
    // Start with no output points
    int numOut = 0;

    // Calculate the distance of end points to the line
    Vec2 normal = (c1 - c0).Normalize();
    float dist0 = (contactsIn[0] - c0).Cross(normal);
    float dist1 = (contactsIn[1] - c0).Cross(normal);

    // If the points are behind the plane
    if (dist0 <= 0)
        contactsOut[numOut++] = contactsIn[0];
    if (dist1 <= 0)
        contactsOut[numOut++] = contactsIn[1];

    // If the points are on different sides of the plane (one distance is negative and the other is positive)
    if (dist0 * dist1 < 0)
    {
        float totalDist = dist0 - dist1;

        // Fint the intersection using linear interpolation: lerp(start,end) => start + t*(end-start)
        float t = dist0 / (totalDist);
        Vec2 contact = contactsIn[0] + (contactsIn[1] - contactsIn[0]) * t;
        contactsOut[numOut] = contact;
        numOut++;
    }
    return numOut;
}

void PolygonShape::UpdateVertices(float angle, const Vec2 &position)
{
    // Loop all the vertices, transforming from local to world space
    for (int i = 0; i < localVertices.size(); i++)
    {
        // First rotate, then transition
        worldVertices[i] = localVertices[i].Rotate(angle);
        worldVertices[i] += position;
    }
}

///////////////////////////////// Boxes /////////////////////////////////
BoxShape::BoxShape(float width, float height)
    : width(width), height(height)
{
    localVertices.push_back(Vec2(-width / 2, -height / 2));
    localVertices.push_back(Vec2(width / 2, -height / 2));
    localVertices.push_back(Vec2(width / 2, height / 2));
    localVertices.push_back(Vec2(-width / 2, height / 2));

    worldVertices.push_back(Vec2(-width / 2, -height / 2));
    worldVertices.push_back(Vec2(width / 2, -height / 2));
    worldVertices.push_back(Vec2(width / 2, height / 2));
    worldVertices.push_back(Vec2(-width / 2, height / 2));

    std::cout << "BoxShape constructor called!" << std::endl;
}

BoxShape::~BoxShape()
{
    // TODO:.....
    std::cout << "BoxShape destructor called!" << std::endl;
}

ShapeType BoxShape::GetType() const
{
    return BOX;
}

float BoxShape::GetMomentOfInertia() const
{
    // Inertia for a rectangle is 1/12 * (w^2 + h^2) * mass(multiplication by mass happens in body.cpp)
    return (0.083333) * (width * width + height * height);
}