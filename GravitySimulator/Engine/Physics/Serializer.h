#pragma once

#include "Body.h"
#include "System.h"
#include <iostream>
#include <fstream>


std::ostream& operator<<(std::ostream& os, const vec3& vec);
std::istream& operator>>(std::istream& is, vec3& vec);

std::ostream& operator<<(std::ostream& os, const Body& body);
std::istream& operator>>(std::istream& is, Body& body);

void serializeBodies(std::ostream& os, const BodiesArray& bodies);
void deserializeBodies(std::istream& is, BodiesArray& bodies);