#define G (6.673e-11)
#define SHIELD_RADIUS (5)
#define EPSILON (1)

typedef struct {
	float x;
	float y;
	float z;
} vec;

typedef struct {
	unsigned long mass;
	vec position;
	vec velocity;
	vec acceleration;
} star;


__kernel void calculateGravity(__global star *stars, int numStars, int workGroupSize) {
	int globalIndex = get_global_id(0);
	int i;
	star first = stars[globalIndex];
	first.acceleration.x = 0;
	first.acceleration.y = 0;
	first.acceleration.z = 0;

	for(i = 0; i < numStars; i++) {
		if(i != globalIndex) {

			float dx = stars[i].position.x - first.position.x;
			float dy = stars[i].position.y - first.position.y;
			float dz = stars[i].position.z - first.position.z;
			
			float r = sqrt(dx * dx + dy * dy + dz * dz);


			if(r != 0) {
				float magnitude = (G * first.mass * stars[i].mass) / pow(r * r + EPSILON, 1.5f);
				vec force = {magnitude * dx, magnitude * dy, magnitude * dz};
			
				first.acceleration.x += force.x / first.mass;
				first.acceleration.y += force.y / first.mass;
				first.acceleration.z += force.z / first.mass;
			}
		}
	}
	
	barrier(CLK_GLOBAL_MEM_FENCE);
	
	first.velocity.x += first.acceleration.x;
	first.velocity.y += first.acceleration.y;
	first.velocity.z += first.acceleration.z;
	barrier(CLK_GLOBAL_MEM_FENCE);
	
	first.position.x += first.velocity.x;
	first.position.y += first.velocity.y;
	first.position.z += first.velocity.z;
	stars[globalIndex] = first;
}