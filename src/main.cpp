#include <iostream>
#include <cmath>

#include "color.h"
#include "rtweekend.h"
#include "hittable_list.h"
#include "sphere.h"

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

color ray_color(const ray &r, const hittable &world) {
	hit_record rec;
	if (world.hit(r, 0, infinity, rec)) {
		return 0.5 * (rec.normal + color(1, 1, 1));
	}
	vec3 unit_direction = unit_vector(r.direction());
	auto t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

int main(void) {
	const auto aspect_ratio = 16.0 / 9.0;
	const int32_t image_width = 400;
	const int32_t image_height = static_cast<int32_t>(image_width / aspect_ratio);

	hittable_list world;
	world.add(std::make_shared<sphere>(point3(0, 0, -1), 0.5));
	world.add(std::make_shared<sphere>(point3(0, -100.5, 1), 100));

	auto viewport_height = 2.0;
	auto viewport_width = aspect_ratio * viewport_height;
	auto focal_length = 1.0;

	auto origin = point3(0, 0, 0);
	auto horizontal = vec3(viewport_width, 0, 0);
	auto vertical = vec3(0, viewport_height, 0);
	auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

	std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

	for (int32_t j = image_height - 1; j >= 0; --j) {
		std::cerr << "Scanlines remaining: " << j << '\n';
		for (int32_t i = 0; i < image_width; ++i) {
			auto u = double(i) / (image_width - 1);
			auto v = double(j) / (image_height - 1);
			ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
			color pixel_color = ray_color(r, world);
			write_color(std::cout, pixel_color);
		}
	}

	std::cerr << "Done\n";
}