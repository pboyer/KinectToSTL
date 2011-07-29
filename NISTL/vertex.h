#include <math.h>

namespace NISTL{
	struct vertex {
		vertex(double a, double b, double c):x(a), y(b), z(c) {}
		vertex():x(0), y(0), z(0) {}

		double x;
		double y;
		double z;

		vertex subtract(vertex v) {
			return vertex(x-v.x,y-v.y, z-v.z);
		}

		vertex cross(vertex o) {
			double xi = y * o.z - z * o.y;
			double yi = z * o.x - x * o.z;
			double zi = x * o.y - y * o.x;

			return vertex(xi,yi,zi);
		}

		vertex normalize() {
			double magnitude = sqrt(x*x+y*y+z*z);

			if(magnitude == 0) 
				magnitude = 1;

			return vertex(x/magnitude, y/magnitude, z/magnitude);
		}

	};
}