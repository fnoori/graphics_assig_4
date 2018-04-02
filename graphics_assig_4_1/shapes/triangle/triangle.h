//
//  triangle.hpp
//  graphics_assig_4_1
//
//  Created by Farzam Noori on 2018-03-15.
//  Copyright © 2018 Farzam Noori. All rights reserved.
//

#ifndef triangle_h
#define triangle_h

#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "rayIntersect.h"

using namespace std;
using namespace glm;

class Triangle
{
private:
    vec3 pointA;
    vec3 pointB;
    vec3 pointC;
    vec3 originOfRay;
    vec3 directionOfRay;
    vec3 normalVectorOfPlane;
    vec3 triangleColour;
    RayIntersectDetails intersection;
    
public:
    Triangle(vec3 pointA, vec3 pointB, vec3 pointC, vec3 originOfRay, vec3 directionOfRay, vec3 triangleColour);
    
    bool triangleIntersection();
    void initTrianglePlane();
    RayIntersectDetails getIntersection();
};

#endif /* triangle_h */
