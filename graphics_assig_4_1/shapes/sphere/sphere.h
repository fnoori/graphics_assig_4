//
//  sphere.h
//  graphics_assig_4_1
//
//  Created by Farzam Noori on 2018-03-14.
//  Copyright © 2018 Farzam Noori. All rights reserved.
//

#ifndef sphere_h
#define sphere_h

#include "rayIntersect.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

class Sphere
{
private:
    vec3 centerOfSphere;
    vec3 originOfRay;
    vec3 directionOfRay;
    vec3 sphereColour;
    float radiusOfSphere;
    RayIntersectDetails intersection;
    float surfaceShine;
    
public:
    Sphere(vec3 centerOfSphere, vec3 originOfRay, vec3 directionOfRay, float radiusOfSphere, float surfaceShine, vec3 sphereColour);
    
    RayIntersectDetails getIntersection();
    bool sphereIntersection();
};

#endif /* sphere_h */
