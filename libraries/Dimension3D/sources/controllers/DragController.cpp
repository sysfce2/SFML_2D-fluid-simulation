#include "dim/dimension3D.hpp"

namespace dim
{
	Controller* DragController::clone() const
	{
		return new DragController(*this);
	}

	DragController::DragController(float sensitivity, float speed)
	{
		this->sensitivity = sensitivity;
		this->speed = speed;
		prev_mouse_pos = sf::Mouse::getPosition();
		prev_mouse_click = false;
		move_forbidden = false;
	}

	Controller::Type DragController::get_type() const
	{
		return Type::Drag;
	}

	void DragController::check_events(const sf::Event& sf_event, Scene& scene)
	{
		if (sf_event.type == sf::Event::MouseWheelMoved && scene.is_in(sf::Mouse::getPosition(Window::get_window())))
		{
			scene.camera.position += normalized(scene.camera.direction) * speed * sf_event.mouseWheel.delta;
			scene.camera.view = glm::lookAt(scene.camera.position.to_glm(), (scene.camera.position + scene.camera.direction).to_glm(), glm::vec3(0.f, 1.f, 0.f));

			scene.camera2D.zoom(1.f - (speed * sf_event.mouseWheel.delta * 0.5));
			scene.render_texture.setView(scene.camera2D.get_view());
		}
	}

	void DragController::update(Scene& scene)
	{
		if ((!prev_mouse_click && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left) && (!scene.is_in(sf::Mouse::getPosition(Window::get_window())) ||
			!scene.is_active())) || scene.is_moving())
			move_forbidden = true;

		else if (!scene.is_moving() && prev_mouse_click && !sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
			move_forbidden = false;

		if (!scene.is_moving() && scene.is_active() && scene.is_in(sf::Mouse::getPosition(Window::get_window())))
			move_forbidden = false;

		if (!scene.is_active())
			move_forbidden = true;

		if (!move_forbidden && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		{
			Vector2 move = Vector2(sf::Mouse::getPosition()) - prev_mouse_pos;

			glm::vec3 right = glm::normalize(glm::cross(scene.camera.direction.to_glm(), glm::vec3(0.f, 1.f, 0.f)));

			scene.camera.position -= right * sensitivity * move.x * (4.f / scene.size.y);
			scene.camera.position -= glm::normalize(glm::cross(scene.camera.direction.to_glm(), right)) * sensitivity * move.y * (4.f / scene.size.y);
			scene.camera.view = glm::lookAt(scene.camera.position.to_glm(), (scene.camera.position + scene.camera.direction).to_glm(), glm::vec3(0.f, 1.f, 0.f));

			scene.camera2D.move(-move * sensitivity * scene.camera2D.get_zoom());
			scene.render_texture.setView(scene.camera2D.get_view());
		}

		prev_mouse_pos = sf::Mouse::getPosition();
		prev_mouse_click = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
	}
}