#include "dim/dimension3D.h"

namespace dim
{
	sf::RenderWindow*	Window::window			= NULL;
	float				Window::screen_coef		= 1.f;
	bool				Window::running			= false;
	sf::Clock			Window::clock;
	float				Window::elapsed_time	= 1.f / 60.f;
	sf::Vector2i		Window::scene_size		= sf::Vector2i(10, 10);
	sf::Vector2i		Window::scene_min		= sf::Vector2i(0, 0);
	sf::Vector2i		Window::scene_max		= sf::Vector2i(10, 10);
	bool				Window::is_resized		= false;
	bool				Window::scene_active	= false;
	bool				Window::scene_moving	= false;

	void Window::create_relative(const std::string& project_name, float screen_ratio, float window_ratio, bool resizable, const std::string& icon_path)
	{
		uint16_t width;
		uint16_t height;

		if (sf::VideoMode::getDesktopMode().width > window_ratio * sf::VideoMode::getDesktopMode().height)
			height = sf::VideoMode::getDesktopMode().height * screen_ratio, width = height * window_ratio;

		else if (sf::VideoMode::getDesktopMode().width < (16.f / 9.f) * sf::VideoMode::getDesktopMode().height)
			width = sf::VideoMode::getDesktopMode().width * screen_ratio, height = width / window_ratio;

		else
			width = sf::VideoMode::getDesktopMode().width * screen_ratio, height = sf::VideoMode::getDesktopMode().height * screen_ratio;

		screen_coef = width / 1920.f;

		sf::ContextSettings settings;
		settings.depthBits = 24;
		settings.stencilBits = 8;
		settings.antialiasingLevel = 8;
		settings.majorVersion = 3;
		settings.minorVersion = 1;

		sf::Uint32 style;

		if (resizable)
			style = sf::Style::Default;

		else
			style = sf::Style::Close | sf::Style::Titlebar;

		window = new sf::RenderWindow(sf::VideoMode(width, height), project_name, style, settings);
		//window->setFramerateLimit(60);
		//window->setVerticalSyncEnabled(true);

		if (icon_path != "")
		{
			sf::Image icon;
			icon.loadFromFile(icon_path);
			window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
		}

		window->setActive(true);
		init();

		running = true;
	}

	void Window::create_absolute(const std::string& project_name, uint16_t width, uint16_t height, bool resizable, const std::string& icon_path)
	{
		screen_coef = width / 1920.f;

		sf::ContextSettings settings;
		settings.depthBits = 24;
		settings.stencilBits = 8;
		settings.antialiasingLevel = 8;
		settings.majorVersion = 3;
		settings.minorVersion = 1;

		sf::Uint32 style;

		if (resizable)
			style = sf::Style::Default;

		else
			style = sf::Style::Close | sf::Style::Titlebar;

		window = new sf::RenderWindow(sf::VideoMode(get_width(), get_height()), project_name, style, settings);
		window->setFramerateLimit(60);
		window->setVerticalSyncEnabled(true);

		if (icon_path != "")
		{
			sf::Image icon;
			icon.loadFromFile(icon_path);
			window->setIcon(icon.getSize().x, icon.getSize().y, icon.getPixelsPtr());
		}

		window->setActive(true);
		init();

		running = true;
	}

	sf::RenderWindow& Window::get_window()
	{
		return *window;
	}

	uint16_t Window::get_width()
	{
		return window->getSize().x;
	}

	uint16_t Window::get_height()
	{
		return window->getSize().y;
	}

	void Window::clear(const sf::Color& color)
	{
		window->setActive(true);
		window->clear(color);

		if (is_resized)
			FrameBuffer::default_frame_buffer.reload();

		FrameBuffer::clear_default(color);

		ImGui::SFML::Update(dim::Window::get_window(), sf::seconds(Window::elapsed_time));

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;

		if (is_on_border_window(sf::Mouse::getPosition(Window::get_window())))
			ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
		else
			ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;

		ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

		ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->Pos);
		ImGui::SetNextWindowSize(viewport->Size);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("InvisibleWindow", NULL, windowFlags);
		ImGui::PopStyleVar(3);

		ImGuiID dockSpaceId = ImGui::GetID("InvisibleWindowDockSpace");

		ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::End();
	}

	void Window::draw(const sf::Drawable& drawable)
	{
		window->draw(drawable);
	}

	void Window::display()
	{
		static bool first_frame = true;

		ImGui::Begin("Scene");

		ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

		sf::Vector2i temp = scene_size;

		if (first_frame || scene_size.x != viewportPanelSize.x || scene_size.y != viewportPanelSize.y)
		{
			scene_size.x = viewportPanelSize.x;
			scene_size.y = viewportPanelSize.y;

			is_resized = true;
		}

		else
			is_resized = false;

		ImVec2 vMin = ImGui::GetWindowContentRegionMin();
		ImVec2 vMax = ImGui::GetWindowContentRegionMax();
		ImVec2 vPos = ImGui::GetWindowPos();

		scene_moving = scene_min.x != vMin.x + vPos.x || scene_min.y != vMin.y + vPos.y || scene_max.x != vMax.x + vPos.x || scene_max.y != vMax.y + vPos.y;

		scene_min.x = vMin.x + vPos.x;
		scene_min.y = vMin.y + vPos.y;
		scene_max.x = vMax.x + vPos.x;
		scene_max.y = vMax.y + vPos.y;

		scene_active = ImGui::IsWindowFocused();

		uint64_t textureID = FrameBuffer::default_frame_buffer.get_texture().get_id();
		ImGui::Image(textureID, ImVec2{ (float)temp.x, (float)temp.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

		ImGui::End();

		ImGui::SFML::Render(get_window());

		window->display();

		elapsed_time = clock.restart().asSeconds();

		first_frame = false;
	}

	void Window::close()
	{
		if (window != NULL)
		{
			window->close();
			delete window;
		}
	}

	uint16_t Window::to_relative(uint16_t position)
	{
		return round(position * screen_coef);
	}

	void Window::check_events(const sf::Event& sf_event)
	{
		ImGui::SFML::ProcessEvent(sf_event);

		if (sf_event.type == sf::Event::Closed)
			running = false;
	}

	sf::Vector2i Window::get_scene_size()
	{
		return scene_size;
	}

	sf::RenderWindow& get_window()
	{
		return Window::get_window();
	}

	uint16_t to_relative(uint16_t position)
	{
		return Window::to_relative(position);
	}

	bool is_in_window(const Vector2& position)
	{
		return position.x >= 0 && position.x <= Window::get_width() && position.y >= 0 && position.y <= Window::get_height();
	}

	bool is_on_border_window(const Vector2& position)
	{
		return (position.x >= -2 && position.x <= 2) || (position.x >= Window::get_width() - 2 && position.x <= Window::get_width() + 2) ||
			(position.y >= -2 && position.y <= 2) || (position.y >= Window::get_height() - 2 && position.y <= Window::get_height() + 2);
	}

	bool is_in_scene(const Vector2& position)
	{
		return position.x >= Window::scene_min.x && position.x <= Window::scene_max.x && position.y >= Window::scene_min.y && position.y <= Window::scene_max.y;
	}
}
