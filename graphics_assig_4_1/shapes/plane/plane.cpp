//
//  plane.cpp
//  graphics_assig_4_1
//
//  Created by Farzam Noori on 2018-03-19.
//  Copyright © 2018 Farzam Noori. All rights reserved.
//

#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "plane.h"
#include "rayIntersect.h"

using namespace std;
using namespace glm;

Plane :: Plane(vec3 originOfRay, vec3 directionOfRay, vec3 planeNormal, vec3 pointOnPlane, vec3 planeColour)
{
    this -> originOfRay = originOfRay;
    this -> directionOfRay = directionOfRay;
    this -> planeNormal = planeNormal;
    this -> pointOnPlane = pointOnPlane;
    this -> planeColour = planeColour;
}

bool Plane :: planeIntersection()
{
    if (dot(directionOfRay, planeNormal) == 0) {
        return false;
    }
    
    float A = dot(pointOnPlane, planeNormal) - dot(originOfRay, planeNormal);
    float B = dot(directionOfRay, planeNormal);
    
    float tVal = A/B;
    float intersectionX;
    float intersectionY;
    float intersectionZ;
    
    if (tVal == 0) {
        return false;
    }
    
    /* set the details of the intersection */
    intersection.intersection = true;
    
    // set vector position of point of intersection
    intersectionX = originOfRay.x + (tVal * (directionOfRay.x - originOfRay.x));
    intersectionY = originOfRay.y + (tVal * (directionOfRay.y - originOfRay.y));
    intersectionZ = originOfRay.z + (tVal * (directionOfRay.z - originOfRay.z));
    intersection.position = vec3(intersectionX, intersectionY, intersectionZ);
    
    // set colour of sphere at that point
    intersection.colour = planeColour;
    
    // set vector normal at the point of intersection
    intersection.normal = vec3(planeNormal.x, planeNormal.y, planeNormal.z);
    
    return true;
}

RayIntersectDetails Plane :: getIntersection()
{
    return intersection;
}
