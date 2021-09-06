#include <iostream>
#include <cmath>

#include "color.h"
#include "rtweekend.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"

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
		// point3 target = rec.p + rec.normal + random_unit_vector();
		point3 target = rec.p + random_in_hemisphere(rec.normal);
		return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth - 1);
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
	camera cam;
	hittable_list world;

	world.add(std::make_shared<sphere>(point3(0, 0, -1), 0.5));
	world.add(std::make_shared<sphere>(point3(0, -100.5, 1), 100));

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