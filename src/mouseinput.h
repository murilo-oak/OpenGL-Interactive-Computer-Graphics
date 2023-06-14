#pragma once
class MouseInput
{
private:
	int m_lastX{};
	int m_lastY{};

	int m_x{};
	int m_y{};

public:
	/**
	 * @brief Updates the mouse input with new coordinates.
	 *
	 * This function updates the mouse input with new coordinates by setting the last known coordinates
	 * to the previous coordinates and updating the current coordinates with the new values.
	 *
	 * @param x The new x-coordinate.
	 * @param y The new y-coordinate.
	 */
	void update(int x, int y);

	/**
	 * @brief Returns the current x-coordinate of the mouse input.
	 *
	 * This function retrieves the current x-coordinate of the mouse input.
	 *
	 * @return The current x-coordinate.
	 */
	int getX() const { return m_x; }

	/**
	 * @brief Returns the current y-coordinate of the mouse input.
	 *
	 * This function retrieves the current y-coordinate of the mouse input.
	 *
	 * @return The current y-coordinate.
	 */
	int getY() const { return m_y; }
	/**
	 * @brief Returns the previous x-coordinate of the mouse input.
	 *
	 * This function retrieves the previous x-coordinate of the mouse input.
	 *
	 * @return The previous x-coordinate.
	 */
	int getLastX() const { return m_lastX; }

	/**
	 * @brief Returns the previous y-coordinate of the mouse input.
	 *
	 * This function retrieves the previous y-coordinate of the mouse input.
	 *
	 * @return The previous y-coordinate.
	 */
	int getLastY() const { return m_lastY; }

	/**
	 * @brief Returns the change in x-coordinate of the mouse input.
	 *
	 * This function calculates and returns the change in the x-coordinate of the mouse input
	 * since the last update.
	 *
	 * @return The change in x-coordinate.
	 */
	int getDeltaX() const { return m_x - m_lastX; }

	/**
	 * @brief Returns the change in y-coordinate of the mouse input.
	 *
	 * This function calculates and returns the change in the y-coordinate of the mouse input
	 * since the last update.
	 *
	 * @return The change in y-coordinate.
	 */
	int getDeltaY() const { return m_y - m_lastY; }
};

