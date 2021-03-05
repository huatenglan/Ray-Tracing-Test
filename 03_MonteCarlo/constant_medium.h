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

	if (!boundary->hit(r, -infinity, infinity, rec1))			//ֻ�ж������ߣ������û�н���,���������,ע�������t_min,��Ϊ����С��0���п����ǹ����������ڲ�����һ���ж�
		return false;											//������Ϊ���ص�������ģ����Բ�����ʲô�����ͨ���˵ڶ����жϵĶ����дα���ɢ��
	if (!boundary->hit(r, rec1.t + 0.0001, infinity, rec2))		//����н��㣬�ж���û�еڶ�������(һ��һ��ģ�;ͻ���2���������)
		return false;											//ͨ������жϣ���Դ�����������ڲ������ǿ��Դ�������

	if (debugging) std::cerr << "\nt_min=" << rec1.t << ",t_max=" << rec2.t << '\n';

	if (rec1.t < t_min)rec1.t = t_min;							//����������Ҫ���������(��������ܲ����������ڲ�ȡ����t_min������)
	if (rec2.t > t_max)rec2.t = t_max;							//��һ����Ϊ�����Ʋ����������㶼С�����Ǹ�����t_min
		
	if (rec1.t >= rec2.t)
		return false;

	if (rec1.t < 0)												//������һ��,�����ļ��������ȷ�ģ�����ӿ�ʼ���������ľ��룩
		rec1.t = 0;

	const auto ray_length = r.direction().length();
	const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
	const auto hit_distance = neg_inv_density * log(random_double());	//log������e,���ص���Զ��һ�����������hit_distance��ɢ�䷢���ľ���.
																		//(������������濪ʼ)���ܶ�Խ�󣬾���ԽС
	if (hit_distance > distance_inside_boundary)						//��Ҫ�ľ�����ڴ��ڵľ��룬û�дα���ɢ��
		return false;

	rec.t = rec1.t + hit_distance / ray_length;							//�õ����Ƿ����α���ɢ���ʱ��t,	t*ray_length = distance
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