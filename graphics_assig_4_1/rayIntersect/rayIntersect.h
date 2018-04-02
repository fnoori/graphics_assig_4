//
//  camera.h
//  graphics_assig_4_1
//
//  Created by Farzam Noori on 2018-03-15.
//  Copyright © 2018 Farzam Noori. All rights reserved.
//

#ifndef rayIntersect_h
#define rayIntersect_h

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

struct RayIntersectDetails {
    bool intersection;
    float tVal;
    vec3 position;
    vec3 normal;
    vec3 colour;
    float closest;
};

class RayIntersect
{
private:
    RayIntersectDetails intersectDetails;
    
public:
    RayIntersect();
    RayIntersectDetails getIntersectDetails();
};


#endif /* rayIntersect_h */
