#include "mouseinput.h"

void MouseInput::update(int x, int y) 
{
	//stores the previous (x, y)
	m_lastX = m_x;
	m_lastY = m_y;

	//update current (x, y)
	m_x = x;
	m_y = y;
};
