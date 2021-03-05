#pragma once

#include "vec3.h"

class ray {
public:
	ray() {}
	ray(const point3& origin, const vec3& direction, double time = 0.0)
		:orig(origin), dir(direction), tm(time) {}

	point3 origin() const { return orig; }
	vec3 direction() const { return dir; }
	double time()const { return tm; }

	point3 at(double t) const {
		return orig + t * dir;//r(t) = o + td  at,光线打到到哪里
	}
private:
	point3 orig;
	vec3 dir;
	double tm;
};