#include "mouseinput.h"

void MouseInput::update(int x, int y) 
{
	// Stores the previous (x, y)
	m_lastX = m_x;
	m_lastY = m_y;

	// Update current (x, y)
	m_x = x;
	m_y = y;
};
