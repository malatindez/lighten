#pragma once
#include <concepts>
template <typename T>
concept Primitive = std::floating_point<T> || std::integral<T>;

#include <cmath>
#include <istream>
#include <ostream>

#include "math/vec2.hpp"
#include "math/vec3.hpp"
#include "math/vec4.hpp"