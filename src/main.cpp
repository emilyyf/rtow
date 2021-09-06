#include <iostream>
#include <cmath>

#include "color.h"
#include "rtweekend.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"

double hit_sphere(const point3 &center, double radius, const ray &r) {
	vec3 oc = r.origin() - center;
	double a = r.direction().length_squared();
	double half_b = dot(oc, r.direction());
	double c = oc.length_squared() - radius * radius;
	double discriminant = half_b * half_b - a * c;
	if (discriminant < 0) {
		return -1.0;
	} else {
		return (-half_b - std::sqrt(discriminant)) / a;
	}
}

color ray_color(const ray &r, const hittable &world, int32_t depth) {
	hit_record rec;

	if (depth <= 0)
		return color(0, 0, 0);

	if (world.hit(r, 0.001, infinity, rec)) {
		ray scattered;
		color attenuation;
		if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
			return attenuation * ray_color(scattered, world, depth - 1);
		return color(0, 0, 0);
	}
	vec3 unit_direction = unit_vector(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

int main(void) {
	const auto aspect_ratio = 16.0 / 9.0;
	const int32_t image_width = 400;
	const int32_t image_height = static_cast<int32_t>(image_width / aspect_ratio);
	const int32_t samples_per_pixel = 100;
	const int32_t max_depth = 50;
	camera cam(point3(-2, 2, 1), point3(0, 0, -1), vec3(0, 1, 0), 20.0, aspect_ratio);
	hittable_list world;
	auto material_ground = std::make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto material_center = std::make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto material_left   = std::make_shared<dielectric>(1.5);
	auto material_right  = std::make_shared<metal>(color(0.8, 0.6, 0.2), 0.0);

	world.add(std::make_shared<sphere>(point3( 0.0, -100.5, -1.0),  100.0, material_ground));
	world.add(std::make_shared<sphere>(point3( 0.0,    0.0, -1.0),    0.5, material_center));
	world.add(std::make_shared<sphere>(point3(-1.0,    0.0, -1.0),    0.5, material_left));
	world.add(std::make_shared<sphere>(point3(-1.0,    0.0, -1.0),   -0.4, material_left));
	world.add(std::make_shared<sphere>(point3( 1.0,    0.0, -1.0),    0.5, material_right));

	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int32_t j = image_height - 1; j >= 0; --j) {
		std::cerr << "Scanlines remaining: " << j << '\n';
		for (int32_t i = 0; i < image_width; ++i) {
			color pixel_color(0, 0, 0);
			for (int32_t s = 0; s < samples_per_pixel; ++s) {
				auto u = (i + random_double()) / (image_width - 1);
				auto v = (j + random_double()) / (image_height - 1);
				ray r = cam.get_ray(u, v);
				pixel_color += ray_color(r, world, max_depth);
			}
			write_color(std::cout, pixel_color, samples_per_pixel);
		}
	}

	std::cerr << "Done\n";
}