//
//  triangle.cpp
//  graphics_assig_4_1
//
//  Created by Farzam Noori on 2018-03-15.
//  Copyright © 2018 Farzam Noori. All rights reserved.
//

#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "triangle.h"
#include "rayIntersect.h"

using namespace std;
using namespace glm;

Triangle :: Triangle(vec3 pointA, vec3 pointB, vec3 pointC, vec3 originOfRay, vec3 directionOfRay, vec3 triangleColour)
{
    this -> pointA = pointA;
    this -> pointB = pointB;
    this -> pointC = pointC;
    
    this -> originOfRay = originOfRay;
    this -> directionOfRay = directionOfRay;
    this -> triangleColour = triangleColour;
}

bool Triangle :: triangleIntersection()
{
    initTrianglePlane();
    
    vec3 pointOfIntersection = vec3(1.f);
    
    float A = dot(pointA, normalVectorOfPlane);
    float B = dot(originOfRay, normalVectorOfPlane);
    float C = dot(directionOfRay, normalVectorOfPlane);
    float tVal = (A - B) / C;
    
    float intersectionX;
    float intersectionY;
    float intersectionZ;
    
    pointOfIntersection = originOfRay + (tVal * directionOfRay);

    if (dot(cross((pointB - pointA), (pointOfIntersection - pointA)), normalVectorOfPlane) < 0) {
        return false;
    }
    if (dot(cross((pointC - pointB), (pointOfIntersection - pointB)), normalVectorOfPlane) < 0) {
        return false;
    }
    if (dot(cross((pointA - pointC), (pointOfIntersection - pointC)), normalVectorOfPlane) < 0) {
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
    intersection.colour = triangleColour;
    
    // set vector normal at the point of intersection
    intersection.normal = vec3(normalVectorOfPlane.x, normalVectorOfPlane.y, normalVectorOfPlane.z);
    
    intersection.closest = length(pointOfIntersection);
    
    // There is an intersection
    return true;
}

void Triangle :: initTrianglePlane()
{
    vec3 uVal = (pointB - pointA);
    vec3 vVal = (pointC - pointA);
    
    normalVectorOfPlane = normalize(cross(uVal, vVal));
}

RayIntersectDetails Triangle :: getIntersection()
{
    return intersection;
}
