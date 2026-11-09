#pragma once


#include <vector>
#include <glm/glm.hpp>

namespace nu
{
	class Input
	{
	public:
		enum class MouseButton
		{
			Left = 1,
			Middle,
			Right
		};

	public:
		bool Initialize();
		void Shutdown();

		void Update();

		bool GetKeyDown(int key) const { return m_keyStates[key]; }
		bool GetPrevKeyDown(int key) const { return m_prevKeyStates[key]; }
		bool GetKeyPressed(int key) const { return !m_prevKeyStates[key] && m_keyStates[key]; }
		bool GetKeyReleased(int key) const { return m_prevKeyStates[key] && !m_keyStates[key]; }

		bool GetButtonDown(MouseButton button) const { return m_buttonStates & GetButtonBit(button); }
		bool GetPrevButtonDown(MouseButton button) const { return m_prevButtonStates & GetButtonBit(button); } 
		bool GetButtonPressed(MouseButton button) const { return !GetPrevButtonDown(button) && GetButtonDown(button); }
		bool GetButtonReleased(MouseButton button) const { return GetPrevButtonDown(button) && !GetButtonDown(button); }

		glm::vec2 GetMousePosition() const { return m_mousePosition; }
		glm::vec2 GetMouseDelta() const { return m_mousePosition - m_prevMousePosition; }

	private:
		uint32_t GetButtonBit(MouseButton button) const;

	private:
		// keyboard
		std::vector<bool> m_keyStates;
		std::vector<bool> m_prevKeyStates;

		// mouse
		uint32_t m_buttonStates = 0;
		uint32_t m_prevButtonStates = 0;
		
		glm::vec2 m_mousePosition{ 0, 0 };
		glm::vec2 m_prevMousePosition{ 0, 0 };
	};

}