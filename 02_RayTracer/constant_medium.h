#pragma once

#include "rtweekend.h"

#include "hittable.h"
#include "material.h"
#include "texture.h"


class constant_medium :public hittable {
public:
	constant_medium(shared_ptr<hittable> b, double d, shared_ptr<texture> a) :
		boundary(b), neg_inv_density(-1 / d), phase_fuction(make_shared<isotropic>(a)) {}

	constant_medium(shared_ptr<hittable> b, double d, color c)
		:boundary(b), neg_inv_density(-1 / d), phase_fuction(make_shared<isotropic>(c)) {}

	virtual bool hit(
		const ray& r, double t_min, double t_max, hit_record& rec) const override;

	virtual bool bounding_box(double time0, double time1, aabb& output_box) const override {
		return boundary->bounding_box(time0, time1, output_box);
	}
public:
	shared_ptr<hittable> boundary;
	shared_ptr<material> phase_fuction;
	double neg_inv_density;
};

bool constant_medium::hit(const ray& r, double t_min, double t_max, hit_record& rec)const {
	//Print occasional samples when debugging.To enable ,set enableDbug true
	const bool enableDebug = false;
	const bool debugging = enableDebug && random_double() < 0.00001;

	hit_record rec1, rec2; 

	if (!boundary->hit(r, -infinity, infinity, rec1))			//只判断这条线（无穷）有没有交点,返回最近的,注意这里的t_min,因为交点小于0，有可能是光线在物体内部，下一步判断
		return false;											//这里因为返回的是最近的，所以不管是什么情况，通过了第二个判断的都会有次表面散射
	if (!boundary->hit(r, rec1.t + 0.0001, infinity, rec2))		//如果有交点，判断有没有第二个交点(一般一个模型就会有2个交点最多)
		return false;											//通过这个判断，光源不是在物体内部，就是可以穿过物体

	if (debugging) std::cerr << "\nt_min=" << rec1.t << ",t_max=" << rec2.t << '\n';

	if (rec1.t < t_min)rec1.t = t_min;							//限制在我们要求的区间上(这里光线能不能在物体内部取决于t_min的设置)
	if (rec2.t > t_max)rec2.t = t_max;							//这一步是为了限制不能两个交点都小于我们给定的t_min
		
	if (rec1.t >= rec2.t)
		return false;

	if (rec1.t < 0)												//经过这一步,后续的计算才是正确的（计算从开始到射出物体的距离）
		rec1.t = 0;

	const auto ray_length = r.direction().length();
	const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
	const auto hit_distance = neg_inv_density * log(random_double());	//log底数是e,返回的永远是一个随机负数，hit_distance是散射发生的距离.
																		//(从物体射入表面开始)，密度越大，距离越小
	if (hit_distance > distance_inside_boundary)						//需要的距离大于存在的距离，没有次表面散射
		return false;

	rec.t = rec1.t + hit_distance / ray_length;							//得到的是发生次表面散射的时间t,	t*ray_length = distance
	rec.p = r.at(rec.t);

	if (debugging) {
		std::cerr << "hit_distance = " << hit_distance << '\n'
			<< "rec.t = " << rec.t << '\n'
			<< "rec.p = " << rec.p << '\n';
	}

	rec.normal = vec3(1, 0, 0);											//arbitrary
	rec.front_face = true;												//also arbitrart
	rec.mat_ptr = phase_fuction;

	return true;
}