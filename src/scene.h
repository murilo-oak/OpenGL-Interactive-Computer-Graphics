#pragma once
#include "../ThirdParty/include/GL/glew.h"
#include "../ThirdParty/include/GL/freeglut.h"
#include"../ThirdParty/include/cyCodeBase/cyGL.h"

#include "camera.h"
#include "cubemap.h"
#include "object3D.h"
#include "plane.h"
#include "mouseinput.h"

#include "../ThirdParty/include/glm/glm.hpp"
#include "../ThirdParty/include/glm/gtc/matrix_transform.hpp"
#include "../ThirdParty/include/glm/gtc/type_ptr.hpp"

class IScene
{
public:
	IScene(){}

	/**
	 * @brief Sets up the initial configuration for the derived class.
	 *
	 * This pure virtual function must be implemented by the derived class to perform the necessary setup tasks.
	 * It takes the window width and height as parameters.
	 *
	 * @param windowWidth The width of the window.
	 * @param windowHeight The height of the window.
	 */
	virtual void setup(unsigned int windowWidth, unsigned int windowHeight) = 0;
	
	/**
	 * @brief Reshapes the window with the specified width and height.
	 *
	 * This pure virtual function must be implemented by the derived class to handle the window reshaping logic.
	 * It takes the new window width and height as parameters.
	 *
	 * @param windowWidth The new width of the window.
	 * @param windowHeight The new height of the window.
	 */
	virtual void reshapeWindow(unsigned int windowWidth, unsigned int windowHeight) = 0;
	
	/**
	 * @brief Updates the scene state.
	 *
	 * This pure virtual function must be implemented by the derived class to update the state of the scene.
	 * It should handle any necessary updates or calculations for the scene's logic and behavior.
	 */
	virtual void update() = 0;

	/**
	 * @brief Renders the scene.
	 *
	 * This pure virtual function must be implemented by the derived class to render the scene.
	 * It should handle all the necessary rendering operations to display the scene on the screen.
	 */
	virtual void render() = 0;

	/**
	 * @brief Event handler for right mouse button.
	 *
	 * This is a pure virtual function that serves as an event handler for the right mouse button.
	 * It takes a `MouseInput` object as a parameter, which provides information about the mouse input.
	 * Subclasses of the `Scene` class must implement this function to define their own behavior for handling right mouse button events.
	 *
	 * @param mouse The `MouseInput` object containing information about the mouse input.
	 */
	virtual void onRightButton(MouseInput mouse) = 0;

	/**
	 * @brief Event handler for left mouse button.
	 *
	 * This is a pure virtual function that serves as an event handler for the left mouse button.
	 * It takes a `MouseInput` object as a parameter, which provides information about the mouse input.
	 * Subclasses of the `Scene` class must implement this function to define their own behavior for handling left mouse button events.
	 *
	 * @param mouse The `MouseInput` object containing information about the mouse input.
	 */
	virtual void onLeftButton(MouseInput mouse) = 0;

	/**
	 * @brief Event handler for secondary left mouse button.
	 *
	 * This is a pure virtual function that serves as an event handler for the secondary (or additional) left mouse button.
	 * It takes a `MouseInput` object as a parameter, which provides information about the mouse input.
	 * Subclasses of the `Scene` class must implement this function to define their own behavior for handling the secondary left mouse button events.
	 *
	 * @param mouse The `MouseInput` object containing information about the mouse input.
	 */
	virtual void onLeftButton2(MouseInput mouse) = 0;

	/**
	 * @brief Recompiles the shaders used by the derived class.
	 *
	 * This is a pure virtual function that serves to recompile the shaders used by the derived class.
	 * Subclasses of the `Scene` class must implement this function to define their own shader recompilation logic.
	 */
	virtual void recompileShaders() = 0;
};

