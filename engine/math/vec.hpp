#pragma once
#include <cmath>
#include <concepts>
#include <istream>
#include <ostream>

template <typename T>
concept Primitive = std::floating_point<T> || std::integral<T>;

#include "math/vec2.hpp"
#include "math/vec3.hpp"
#include "math/vec4.hpp"
