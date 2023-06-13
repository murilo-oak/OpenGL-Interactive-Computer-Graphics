#include "mouseinput.h"

void MouseInput::update(int x, int y) 
{
	m_lastX = m_x;
	m_lastY = m_y;

	m_x = x;
	m_y = y;
};
