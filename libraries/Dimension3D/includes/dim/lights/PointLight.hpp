#ifndef POINTLIGHT_HPP
#define POINTLIGHT_HPP

#include "dim/lights/Light.hpp"

namespace dim
{
	class PointLight : public Light
	{
	private:

		Light*	clone() const override;

	public:

		PointLight(const PointLight& other) = default;
		PointLight(const Vector3& position = Vector3(0.f, 0.f, 0.f), const Color& color = Color::White, float intensity = 1.f);

		PointLight& operator=(const PointLight& other) = default;

		void set_position(const Vector3& position);
		Vector3 get_position() const;
		Type get_type() const override;
	};
}

#endif
