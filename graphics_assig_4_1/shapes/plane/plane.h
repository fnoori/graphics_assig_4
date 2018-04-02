//
//  plane.h
//  graphics_assig_4_1
//
//  Created by Farzam Noori on 2018-03-19.
//  Copyright © 2018 Farzam Noori. All rights reserved.
//

#ifndef plane_h
#define plane_h

#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "rayIntersect.h"

using namespace std;
using namespace glm;

class Plane
{
private:
    vec3 planeNormal;
    vec3 pointOnPlane;
    vec3 originOfRay;
    vec3 directionOfRay;
    vec3 planeColour;
    RayIntersectDetails intersection;
    
public:
    Plane(vec3 originOfRay, vec3 directionOfRay, vec3 planeNormal, vec3 pointOnPlane, vec3 planeColour);
    bool planeIntersection();
    void initPlane();
    RayIntersectDetails getIntersection();
};

#endif /* plane_h */
