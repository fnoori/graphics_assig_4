//
//  sphere.cpp
//  graphics_assig_4_1
//
//  Created by Farzam Noori on 2018-03-14.
//  Copyright © 2018 Farzam Noori. All rights reserved.
//

#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "sphere.h"
#include "rayIntersect.h"

using namespace std;
using namespace glm;

Sphere :: Sphere(vec3 centerOfSphere, vec3 originOfRay, vec3 directionOfRay, float radiusOfSphere, float surfaceShine, vec3 sphereColour)
{
    this -> centerOfSphere = centerOfSphere;
    this -> originOfRay = originOfRay;
    this -> directionOfRay = directionOfRay;
    this -> radiusOfSphere = radiusOfSphere;
    this -> surfaceShine = surfaceShine;
    this -> sphereColour = sphereColour;
}

RayIntersectDetails Sphere :: getIntersection()
{
    return intersection;
}

bool Sphere :: sphereIntersection()
{
    float A = dot(directionOfRay, directionOfRay);
    float B = 2 * dot(directionOfRay, (originOfRay - centerOfSphere));
    float C = (dot((originOfRay - centerOfSphere), (originOfRay - centerOfSphere)) - pow(radiusOfSphere, 2));
    float tVal;
    float intersectionX;
    float intersectionY;
    float intersectionZ;
    vec3 unitNormalOfSphere;
    
    float determ = pow(B, 2) - (4 * A * C);
    if (determ < 0) {
        return false;
    }
    
    float quadraticPos = (-B + sqrt(determ))/(2 * A);
    float quadraticNeg = (-B - sqrt(determ))/(2 * A);
    
    if (quadraticPos < quadraticNeg) {
        tVal = quadraticPos;
    } else {
        tVal = quadraticNeg;
    }
    
    /* set the details of the intersection */
    intersection.intersection = true;
    
    // set vector position of point of intersection
    intersectionX = originOfRay.x + (tVal * (directionOfRay.x - originOfRay.x));
    intersectionY = originOfRay.y + (tVal * (directionOfRay.y - originOfRay.y));
    intersectionZ = originOfRay.z + (tVal * (directionOfRay.z - originOfRay.z));
    intersection.position = vec3(intersectionX, intersectionY, intersectionZ);
    
    // set colour of sphere at that point
    intersection.colour = sphereColour;
    
    // set vector normal at the point of intersection
    intersection.normal = vec3(
        (intersectionX - centerOfSphere.x),
        (intersectionY - centerOfSphere.y),
        (intersectionZ - centerOfSphere.z)
    );
    
    return true;
}
