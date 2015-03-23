#include <sys/time.h>
#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <sstream>
#include "vec3f.h"

using namespace std;

#define PI 3.141592653589
#define DEG2RAD(deg) (deg * PI / 180)

class Lattu {
	public:
		float latx, latz, latvx, latvz, lath1, laty;
		float latr1, latr2, lath2, latn1, latn2;
		float speed;
		int score;
		Lattu() {
			latn1 = 0.1;
			latn2 = 0.3;
			latx = 0.0f;
			latz = 127.0f;
			latvx = 0.0f;
			latvz = 0.0f;
			lath1 = 2.5f;
			latr1 = 1.2f;
			latr2 = 1.0f;
			lath2 = 2.0f;
			speed = 0.0; 
			score = 0;
		}
		Lattu(float latn1_, float latn2_, float latx_, float speed_, float score_) {
			latn1 = latn1_;
			latn2 = latn2_;
			latx = latx_;
			speed = speed_;
			score = score_;
			latz = 127.0f;
			latvx = 0.0f;
			latvz = 0.0f;
			lath1 = 2.5f;
			latr1 = 1.2f;
			latr2 = 1.0f;
			lath2 = 2.0f;
		}
		void uplats(float s) {
			this->speed = s;
		}
		void uplatx(float x) {
			this->latx = x;
		}
		void uplatz(float z) {
			this->latz = z;
		}
		void uplatvx(float vx) {
			this->latvx = vx;
		}
		void uplatvz(float vz) {
			this->latvz = vz;
		}
		void uplaty(float y) {
			this->laty = y;
		}
};
