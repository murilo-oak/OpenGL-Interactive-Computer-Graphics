#pragma once
class MouseInput
{
private:
	int m_lastX{};
	int m_lastY{};

	int m_x{};
	int m_y{};

public:
	void update(int x, int y);

	int getX() const { return m_x; }
	int getY() const { return m_y; }

	int getLastX() const { return m_lastX; }
	int getLastY() const { return m_lastY; }
};

