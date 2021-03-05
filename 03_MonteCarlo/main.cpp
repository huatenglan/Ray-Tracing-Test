
#include <time.h>

#include "rtweekend.h"

#include "color.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "moving_sphere.h"
#include "aarect.h"
#include "box.h"
#include "constant_medium.h"
#include "bvh.h"
#include <iomanip>

#include <future>

#include <iostream>



color ray_color(const ray& r,
	const color& background,
	const hittable& world,
	shared_ptr<hittable> lights,
	int depth) {
	hit_record rec;

	//If we've exceeded the ray bunce limit,no more liht is gathered.
	if (depth <= 0)
		return color(0, 0, 0);

	//If the ray hits nothing, return the background color
	if (!world.hit(r, EPSILON, infinity, rec))
		return background;

	scatter_record srec;
	color emitted = rec.mat_ptr->emitted(r, rec, rec.u, rec.v, rec.p);

	if (!rec.mat_ptr->scatter(r, rec, srec))
		return emitted;
	//-----
		////sample the light
		//hittable_pdf light_pdf(lights, rec.p);	
		//scattered = ray(rec.p, light_pdf.generate(), r.time());
		//pdf_val = light_pdf.value(scattered.direction());
		////sample inhemisphere, PDF = cos/pi
		//cosine_pdf p(rec.normal);	//½¨Á¢onb
		//scattered = ray(rec.p, p.generate(), r.time());
		//pdf_val = p.value(scattered.direction());
	//-----

	//specular implicit
	if (srec.is_specular) {
		return srec.attenuation
			* ray_color(srec.specular_ray, background, world, lights, depth - 1);
	}

	//pdf mixture
	auto light_ptr = make_shared<hittable_pdf>(lights, rec.p);
	mixture_pdf mixture_pdf(light_ptr, srec.pdf_ptr);

	ray scattered = ray(rec.p, mixture_pdf.generate(), r.time());
	auto pdf_val = mixture_pdf.value(scattered.direction());

	return emitted
		+ srec.attenuation * rec.mat_ptr->scattering_pdf(r, rec, scattered)
		* ray_color(scattered, background, world, lights, depth - 1) / pdf_val/*+0.0000000001*/;
}

hittable_list cornell_box() {
	hittable_list objects;

	auto red = make_shared <lambertian>(color(.65, .05, .05));
	auto white = make_shared<lambertian>(color(.73, .73, .73));
	auto green = make_shared <lambertian>(color(.12, .45, .15));
	auto light = make_shared <diffuse_light>(color(15, 15, 15));



	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));		//left
	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));			//right
	objects.add(make_shared<flip_face>(make_shared<xz_rect>(213, 343, 227, 332, 554, light)));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
	objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

	//shared_ptr<material> aluminum = make_shared<metal>(color(0.8, 0.85, 0.88), 0.0);
	//shared_ptr<hittable>box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), aluminum);
	shared_ptr<hittable>box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, vec3(265, 0, 295));
	objects.add(box1);

	//shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
	//box2 = make_shared<rotate_y>(box2, -18);
	//box2 = make_shared<translate>(box2, vec3(130, 0, 65));
	//objects.add(box2);

	auto glass = make_shared<dielectric>(1.5);
	objects.add(make_shared<sphere>(point3(190, 90, 190), 90, glass));

	return objects;
}

hittable_list cornell_smoke() {
	hittable_list objects;

	auto red = make_shared<lambertian>(color(.65, .05, .05));
	auto white = make_shared<lambertian>(color(.73, .73, .73));
	auto green = make_shared<lambertian>(color(.12, .45, .15));
	auto light = make_shared<diffuse_light>(color(7, 7, 7));

	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 555, green));		//left
	objects.add(make_shared<yz_rect>(0, 555, 0, 555, 0, red));			//right
	objects.add(make_shared<xz_rect>(113, 443, 127, 432, 554, light));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 0, white));
	objects.add(make_shared<xz_rect>(0, 555, 0, 555, 555, white));
	objects.add(make_shared<xy_rect>(0, 555, 0, 555, 555, white));

	shared_ptr<hittable> box1 = make_shared<box>(point3(0, 0, 0), point3(165, 330, 165), white);
	box1 = make_shared<rotate_y>(box1, 15);
	box1 = make_shared<translate>(box1, vec3(265, 0, 295));

	shared_ptr<hittable> box2 = make_shared<box>(point3(0, 0, 0), point3(165, 165, 165), white);
	box2 = make_shared<rotate_y>(box2, -18);
	box2 = make_shared<translate>(box2, vec3(130, 0, 65));

	objects.add(make_shared<constant_medium>(box1, 0.01, color(0, 0, 0)));
	objects.add(make_shared<constant_medium>(box2, 0.01, color(1, 1, 1)));

	return objects;
}

int main() {

	//Image

	double aspect_ratio = 1.0 / 1.0;
	int image_width = 1200;
	const int image_height = static_cast<int>(image_width / aspect_ratio);
	int samples_per_pixel = 1000;
	int max_depth = 50;

	//World

	auto world = cornell_box();

	auto lights = make_shared<hittable_list>();
	lights->add(make_shared<xz_rect>(213, 343, 227, 332, 554, nullptr));	//it's only used to calculate samples and PDF,so dont send materials
	lights->add(make_shared<sphere>(point3(190, 90, 190), 90, nullptr));

	color background(0, 0, 0);

	//Camera
	point3 lookfrom(278, 278, -800);
	point3 lookat(278, 278, 0);
	vec3 vup(0, 1, 0);
	auto dist_to_focus = 10.0;
	auto aperture = 0.0;
	auto vfov = 40.0;
	auto time0 = 0.0;
	auto time1 = 1.0;


	camera cam(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, dist_to_focus, 0.0, 1.0);

	//Render
	std::cerr << "Begin Rendering...\n";
	clock_t start, end;
	start = clock();

	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int j = image_height - 1; j >= 0; --j) {
		std::cerr << "\rScanlines remaining:" << j << ' ' << std::flush;
		for (int i = 0; i < image_width; ++i) {
			color pixel_color(0, 0, 0);
			for (int s = 0; s < samples_per_pixel; ++s) {
				auto u = (i + random_double()) / (image_width - 1);
				auto v = (j + random_double()) / (image_height - 1);
				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, background, world, lights, max_depth);
			}
			write_color(std::cout, pixel_color, samples_per_pixel);
		}
	}
	end = clock();
	int time = double(end - start) / CLOCKS_PER_SEC;
	std::cerr << "\nDone.\n";
	std::cerr << "total time = " << static_cast<int>(time / 3600) << "hour," << static_cast<int>((time % 3600) / 60) << "minutes," << (time % 60) << "second\n";
}