#ifndef SHAPE_H
#define SHAPE_H

#include <vector>
#include "Math/Vec2.h"

enum ShapeType
{
    CIRCLE,
    POLYGON,
    BOX
};

struct Shape
{
    virtual ~Shape() = default;
    virtual ShapeType GetType() const = 0;
    virtual float GetMomentOfInertia() const = 0;
    virtual void UpdateVertices(float angle, const Vec2 &position) = 0;
};

struct CircleShape : public Shape
{
    float radius;

    CircleShape(const float radius);

    ShapeType GetType() const override;
    float GetMomentOfInertia() const;
    void UpdateVertices(float angle, const Vec2 &position) override;

    virtual ~CircleShape();
};

struct PolygonShape : public Shape
{
    float width;
    float height;

    std::vector<Vec2> localVertices;
    std::vector<Vec2> worldVertices;

    PolygonShape() = default;
    PolygonShape(const std::vector<Vec2> vertices);
    ShapeType GetType() const override;

    Vec2 EdgeAt(int index) const;
    float FindMinSeparation(const PolygonShape *other, int &indexReferenceEdge, Vec2 &supportPoint) const;
    int FindIncidentEdge(const Vec2 &normal) const;
    int ClipSegmentToLine(const std::vector<Vec2> &contactsIn, std::vector<Vec2> &contactsOut, const Vec2 &c0, const Vec2 &c1) const;

    float PolygonArea() const;
    Vec2 PolygonCentroid() const;
    float GetMomentOfInertia() const override;

    // Function to rotate and translate polygon vertives from local space to world space
    void UpdateVertices(float angle, const Vec2 &position) override;

    ~PolygonShape();
};

struct BoxShape : public PolygonShape
{
    float width, height;

    BoxShape(float width, float height);

    ShapeType GetType() const override;
    float GetMomentOfInertia() const;

    virtual ~BoxShape();
};

#endif