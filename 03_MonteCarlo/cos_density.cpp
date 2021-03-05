#include "rtweekend.h"

#include <iostream>
#include <math.h>
#include <iomanip>

inline vec3 random_cosine_direction() {		//pdf(directions) = cos/pi ，这个概率密度函数在法向半球上积分为1
	auto r1 = random_double();
	auto r2 = random_double();
	auto z = sqrt(1 - r2);

	auto phi = 2 * pi * r1;
	auto x = cos(phi) * sqrt(r2);
	auto y = sin(phi) * sqrt(r2);

	return vec3(x, y, z);
}

int main() {
	int N = 1000;

	auto sum = 0.0;
	for (int i = 0; i < N; ++i) {
		auto v = random_cosine_direction();
		sum += v.z() * v.z() * v.z() / (v.z() / pi);		//估计cos^3在半球面上采样
	}

	std::cout << std::fixed << std::setprecision(12);
	std::cout<<"Pi/2			="<<pi/2<<'\n';
	std::cout << "Estimate		=" << sum / N;
}