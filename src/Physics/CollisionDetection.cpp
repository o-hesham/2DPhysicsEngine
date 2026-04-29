#include "Physics/CollisionDetection.h"
#include <limits>
#include "Graphics.h"

bool CollisionDetection::IsColliding(Body *a, Body *b, std::vector<Contact> &contacts)
{
    if (a->justSpawned || b->justSpawned)
        return false;

    bool aIsCircle = a->shape->GetType() == CIRCLE;
    bool bIsCircle = b->shape->GetType() == CIRCLE;
    bool aIsPolygon = a->shape->GetType() == POLYGON || a->shape->GetType() == BOX;
    bool bIsPolygon = b->shape->GetType() == POLYGON || b->shape->GetType() == BOX;

    if (aIsCircle && bIsCircle)
    {
        return IsCollidingCircleCircle(a, b, contacts);
    }
    else if (aIsPolygon && bIsPolygon)
    {
        return IsCollidingPolygonPolygon(a, b, contacts);
    }
    else if (aIsPolygon && bIsCircle)
    {
        return IsCollidingPolygonCircle(a, b, contacts);
    }
    else if (aIsCircle && bIsPolygon)
    {
        return IsCollidingPolygonCircle(b, a, contacts);
    }

    return false;
}

bool CollisionDetection::IsCollidingCircleCircle(Body *a, Body *b, std::vector<Contact> &contacts)
{
    const CircleShape *aCircle = dynamic_cast<CircleShape *>(a->shape.get());
    const CircleShape *bCircle = dynamic_cast<CircleShape *>(b->shape.get());

    const Vec2 ab = b->position - a->position;
    const float radiusSum = aCircle->radius + bCircle->radius;
    const float distSqrd = ab.MagnitudeSquared();

    bool isColliding = distSqrd <= (radiusSum * radiusSum);

    if (!isColliding)
    {
        return false;
    }

    Contact contact;
    contact.a = a;
    contact.b = b;

    if (distSqrd < __FLT_EPSILON__)
    {
        contact.normal = Vec2(1.0, 0.0);
    }
    else
    {
        contact.normal = ab;
        contact.normal.Normalize();
    }

    contact.start = b->position - contact.normal * bCircle->radius;
    contact.end = a->position + contact.normal * aCircle->radius;

    contact.depth = (contact.end - contact.start).Magnitude();

    contacts.push_back(contact);

    return true;
}

bool CollisionDetection::IsCollidingPolygonPolygon(Body *a, Body *b, std::vector<Contact> &contacts)
{
    PolygonShape *aPolygonShape = (PolygonShape *)a->shape.get();
    PolygonShape *bPolygonShape = (PolygonShape *)b->shape.get();

    int aIndexReferenceEdge, bIndexReferenceEdge;
    Vec2 aSupportPoint, bSupportPoint;

    float abSeparation = aPolygonShape->FindMinSeparation(bPolygonShape, aIndexReferenceEdge, aSupportPoint);
    if (abSeparation >= 0)
    {
        return false;
    }

    float baSeparation = bPolygonShape->FindMinSeparation(aPolygonShape, bIndexReferenceEdge, bSupportPoint);
    if (baSeparation >= 0)
    {
        return false;
    }

    PolygonShape *referenceShape;
    PolygonShape *incidentShape;
    int indexReferenceEdge;

    if (abSeparation > baSeparation)
    {
        referenceShape = aPolygonShape;
        incidentShape = bPolygonShape;
        indexReferenceEdge = aIndexReferenceEdge;
    }
    else
    {
        referenceShape = bPolygonShape;
        incidentShape = aPolygonShape;
        indexReferenceEdge = bIndexReferenceEdge;
    }

    // Find the reference edge based on the index that returned from the function
    Vec2 referenceEdge = referenceShape->EdgeAt(indexReferenceEdge);

    /////////////////////////////////////
    // Clipping
    /////////////////////////////////////
    int incidentIndex = incidentShape->FindIncidentEdge(referenceEdge.Normal());
    int incidentNextIndex = (incidentIndex + 1) % incidentShape->worldVertices.size();
    Vec2 v0 = incidentShape->worldVertices[incidentIndex];
    Vec2 v1 = incidentShape->worldVertices[incidentNextIndex];

    std::vector<Vec2> contactPoints = {v0, v1};
    std::vector<Vec2> clippedPoints = contactPoints;

    for (int i = 0; i < referenceShape->worldVertices.size(); i++)
    {
        if (i == indexReferenceEdge)
            continue;
        Vec2 c0 = referenceShape->worldVertices[i];
        Vec2 c1 = referenceShape->worldVertices[(i + 1) % referenceShape->worldVertices.size()];
        int numClipped = referenceShape->ClipSegmentToLine(contactPoints, clippedPoints, c0, c1);
        if (numClipped < 2)
        {
            break;
        }

        contactPoints = clippedPoints; // make the next contact points the ones that were just clipped
    }

    auto vref = referenceShape->worldVertices[indexReferenceEdge];

    // Loop all clipped points, but only consider those where separation is negative (objects are penetrating each other)
    for (auto &vclip : clippedPoints)
    {
        float separation = (vclip - vref).Dot(referenceEdge.Normal());
        if (separation <= 0)
        {
            Contact contact;
            contact.a = a;
            contact.b = b;
            contact.normal = referenceEdge.Normal();
            contact.start = vclip;
            contact.end = vclip + contact.normal * -separation;
            if (baSeparation >= abSeparation)
            {
                std::swap(contact.start, contact.end); // the start-end points are always from "a" to "b"
                contact.normal *= -1.0;                // the collision normal is always from "a" to "b"
            }

            contacts.push_back(contact);
        }
    }
    return true;
}

bool CollisionDetection::IsCollidingPolygonCircle(Body *polygon, Body *circle, std::vector<Contact> &contacts)
{
    const PolygonShape *aPolygon = dynamic_cast<PolygonShape *>(polygon->shape.get());
    const CircleShape *bCircle = dynamic_cast<CircleShape *>(circle->shape.get());

    bool isOutside = false;
    Vec2 minCurrVertex;
    Vec2 minNextVertex;
    float distanceCircleEdge = std::numeric_limits<float>::lowest();

    // Loop all the edges of the polygon finding the nearest edge to the circle center
    for (int i = 0; i < aPolygon->worldVertices.size(); i++)
    {
        int currVertex = i;
        int nextVertex = (i + 1) % aPolygon->worldVertices.size();
        Vec2 edge = aPolygon->EdgeAt(currVertex);
        Vec2 normal = edge.Normal();

        // Compare the circle center with the polygon vertex
        Vec2 vertexToCircleCenter = circle->position - aPolygon->worldVertices[currVertex];
        float projection = vertexToCircleCenter.Dot(normal);

        // If we found a dot product projection that is in the positive/outside side of the normal
        if (projection > 0)
        {
            // Circle center is outside the polygon
            distanceCircleEdge = projection;
            minCurrVertex = aPolygon->worldVertices[currVertex];
            minNextVertex = aPolygon->worldVertices[nextVertex];
            isOutside = true;
            break;
        }
        else
        {
            // Circle center is inside the polygon, find the min edge (the one with the least negative projection)
            if (projection > distanceCircleEdge)
            {
                distanceCircleEdge = projection;
                minCurrVertex = aPolygon->worldVertices[currVertex];
                minNextVertex = aPolygon->worldVertices[nextVertex];
            }
        }
    }

    Contact contact;

    if (isOutside)
    {
        /////////////////////////////////////////////
        // Check if we are inside region A: (left)
        /////////////////////////////////////////////
        Vec2 v1 = circle->position - minCurrVertex; // Vector from the nearest vertex to the circle center
        Vec2 v2 = minNextVertex - minCurrVertex;    // the nearest edge
        if (v1.Dot(v2) < 0)
        {
            // Distance from vertex to the circle center is greater than radius..... no collision
            if (v1.Magnitude() > bCircle->radius)
            {
                return false;
            }
            else
            {
                // Detected collision in region A
                contact.a = polygon;
                contact.b = circle;
                contact.depth = bCircle->radius - v1.Magnitude();
                contact.normal = v1.Normalize();
                contact.start = circle->position + (contact.normal * -bCircle->radius);
                contact.end = contact.start + (contact.normal * contact.depth);
            }
        }
        else
        {
            /////////////////////////////////////////////
            // Check if we are inside region B: (right)
            /////////////////////////////////////////////
            v1 = circle->position - minNextVertex;
            v2 = minCurrVertex - minNextVertex;
            if (v1.Dot(v2) < 0)
            {
                // Distance from vertex to the circle center is greater than radius..... no collision
                if (v1.Magnitude() > bCircle->radius)
                {
                    return false;
                }
                else
                {
                    // Detected collision in region A
                    contact.a = polygon;
                    contact.b = circle;
                    contact.depth = bCircle->radius - v1.Magnitude();
                    contact.normal = v1.Normalize();
                    contact.start = circle->position + (contact.normal * -bCircle->radius);
                    contact.end = contact.start + (contact.normal * contact.depth);
                }
            }
            else
            {
                /////////////////////////////////////////////
                // Check if we are inside region C: (middle)
                /////////////////////////////////////////////
                if (distanceCircleEdge > bCircle->radius)
                {
                    // Distance from vertex to the circle center is greater than radius..... no collision
                    return false;
                }
                else
                {
                    contact.a = polygon;
                    contact.b = circle;
                    contact.depth = bCircle->radius - distanceCircleEdge;
                    contact.normal = (minNextVertex - minCurrVertex).Normal();
                    contact.start = circle->position + (contact.normal * -bCircle->radius);
                    contact.end = contact.start + (contact.normal * contact.depth);
                }
            }
        }
    }
    else
    {
        // The center of the circle is inside the polygon..... definitely colliding
        contact.a = polygon;
        contact.b = circle;
        contact.depth = bCircle->radius - distanceCircleEdge;
        contact.normal = (minNextVertex - minCurrVertex).Normal();
        contact.start = circle->position - (contact.normal * bCircle->radius);
        contact.end = contact.start + (contact.normal * contact.depth);
    }

    contacts.push_back(contact);
    return true;
}