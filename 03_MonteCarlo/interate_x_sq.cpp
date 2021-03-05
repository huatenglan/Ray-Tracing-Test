#include "rtweekend.h"

#include <iostream>
#include <iomanip>
#include <math.h>
#include <stdlib.h>
#include <time.h>

inline double pdf(const vec3& p) {
	return 1 / (4 * pi);
}

int main() {
	int N = 1000000;
	auto sum = 0.0;
	clock_t start, end;
	start = clock();

	for (int i = 0; i < N; ++i) {
		vec3 d = random_unit_vector();
		auto cosine_squared = d.z() * d.z();		//cos(theta) = d.z()
		sum += cosine_squared / pdf(d);
	}
	std::cout << std::fixed << std::setprecision(12);
	std::cout << "I = " << sum / N << '\n';
	end = clock();
	int time = double(end - start) / CLOCKS_PER_SEC;
	std::cout << "time =" << time << '\n';
}