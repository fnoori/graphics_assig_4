//
//  camera.cpp
//  graphics_assig_4_1
//
//  Created by Farzam Noori on 2018-03-15.
//  Copyright © 2018 Farzam Noori. All rights reserved.
//

#include <stdio.h>
#include "rayIntersect.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace std;
using namespace glm;

RayIntersect :: RayIntersect()
{}

RayIntersectDetails RayIntersect :: getIntersectDetails()
{
    return intersectDetails;
}
