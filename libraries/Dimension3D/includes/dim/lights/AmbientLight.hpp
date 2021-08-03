#ifndef AMBIENTLIGHT_HPP
#define AMBIENTLIGHT_HPP

#include "dim/lights/Light.hpp"

namespace dim
{
	class AmbientLight : public Light
	{
	private:

		Light*	clone() const override;

	public:

		AmbientLight(const AmbientLight& other) = default;
		AmbientLight(const Color& color = Color::White, float intensity = 1.f);

		AmbientLight& operator=(const AmbientLight& other) = default;

		Type get_type() const override;
	};
}

#endif
