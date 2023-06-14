#pragma once
#include "../src/scene.h"

class Scene1 : public IScene
{
public:
	Camera   m_cam{};
	Object3D m_object3D{};
	Plane    m_plane{};
	Cubemap  m_cubemap{};

	cy::GLSLProgram m_objectProgram{};
	cy::GLSLProgram m_planeProgram{};
	cy::GLSLProgram m_skyboxProgram{};

	glm::vec3 m_lightDir{ 0.0f, 1.0f, 0.0f };
	unsigned int m_windowHeight{}, m_windowWidth{};


	/**
	 * @brief Configures the initial setup for the Scene1 class.
	 *
	 * This function initializes the Scene1 class by performing various setup tasks. It sets the window width and height,
	 * loads the cubemap images, updates the camera position and sets the MVP (Model-View-Projection) matrix,
	 * loads the 3D object and sets the plane. It also compiles and loads shaders, creates program objects,
	 * sets textures, and sets uniform variables for object, skybox, and plane programs.
	 *
	 * @param windowWidth The width of the window.
	 * @param windowHeight The height of the window.
	 */
	void setup(unsigned int windowWidth, unsigned int windowHeight) override;

	/**
	 * @brief Updates the scene state.
	 *
	 * This function updates the scene state by calling the necessary update functions.
	 * It updates the camera, as well as the uniform variables for the object, skybox, and plane shader programs.
	 */
	void update() override;

	/**
	 * @brief Renders the scene.
	 *
	 * This function renders the scene by performing the necessary OpenGL operations.
	 * It binds the default framebuffer, clears the color and depth buffers, sets the viewport,
	 * draws the skybox, enables depth testing, and finally, draws the 3D object and the plane.
	 * After rendering, it swaps the buffers.
	 */
	void render() override;

	/**
	 * @brief Reshapes the window with the specified width and height.
	 *
	 * This function adjusts the window shape by resizing the viewport and performing necessary operations.
	 * If the height is too small compared to the width, it reshapes the viewport to avoid displaying the skybox boundaries.
	 * It also ensures that the window won't crash if one of the window sides has a size of zero.
	 * The function updates the MVP (Model-View-Projection) matrix, sets the new window width and height,
	 * and resizes the frame buffer of the plane.
	 *
	 * @param windowWidth The new width of the window.
	 * @param windowHeight The new height of the window.
	 */
	void reshapeWindow(unsigned int windowWidth, unsigned int windowHeight) override;

	/**
	 * @brief Handles right button input event.
	 *
	 * This function is called when the right mouse button is pressed or held down.
	 * It takes a `MouseInput` object as a parameter, which provides information about the mouse input.
	 * In this implementation, it updates the `m_angle` of the camera based on the delta values of the mouse movement.
	 *
	 * @param mouse The `MouseInput` object containing information about the mouse input.
	 */
	void onRightButton(MouseInput mouse) override;

	/**
	 * @brief Handles left button input event.
	 *
	 * This function is called when the left mouse button is pressed or held down.
	 * It takes a `MouseInput` object as a parameter, which provides information about the mouse input.
	 * In this implementation, it updates the camera position based on the delta value of the mouse movement along the Y-axis.
	 *
	 * @param mouse The `MouseInput` object containing information about the mouse input.
	 */
	void onLeftButton (MouseInput mouse) override;

	/**
	 * @brief Handles secondary left button input event.
	 *
	 * This function is called when the control key is pressed or held down while mouse left button is down.
	 * It takes a `MouseInput` object as a parameter, which provides information about the mouse input.
	 * In this implementation, it applies rotation transformations to the light direction based on the delta values of the mouse movement.
	 *
	 * @param mouse The `MouseInput` object containing information about the mouse input.
	 */
	void onLeftButton2(MouseInput mouse) override;

	/**
	 * @brief Sets the uniform variables for the specified shader program.
	 *
	 * This function sets the uniform variables for the specified shader program.
	 * It takes the shader program ID, the window height, and the window width as parameters.
	 * In this implementation, it sets the sampler uniforms for the skybox and texture, and calls the `updateUniformVariables` function.
	 *
	 * @param programID The ID of the shader program.
	 * @param windowHeight The height of the window.
	 * @param windowWidth The width of the window.
	 */
	void setUniformVariables   (GLuint programID, unsigned int windowHeight, unsigned int windowWidth);
	
	/**
	 * @brief Updates the uniform variables for the specified shader program.
	 *
	 * This function updates the uniform variables for the specified shader program.
	 * It takes the shader program ID as a parameter.
	 * In this implementation, it sets the `mvp`, `mv3`, `mv4`, `invMv4`, `cameraPos`, `lightDir`, and `windowSize` uniform variables.
	 *
	 * @param programID The ID of the shader program.
	 */
	void updateUniformVariables(GLuint programID);
	
	/**
	 * @brief Recompiles the shaders used by the Scene1 object.
	 *
	 * This function recompiles the shaders used by the Scene1 object.
	 * It creates new shader programs, compiles the vertex and fragment shaders, attaches them to the programs,
	 * and links the programs.
	 * In this implementation, it recompiles the shaders for the object program, skybox program, and plane program.
	 */
	void recompileShaders() override;

private:
	/**
	 * @brief Draws the skybox.
	 *
	 * This function draws the skybox by enabling seamless cube map texture sampling,
	 * setting the skybox program as the current shader program, and drawing the skybox geometry.
	 * It also handles the depth mask to ensure proper rendering of the skybox.
	 */
	void inline drawSkybox();

	/**
	 * @brief Draws the 3D object.
	 *
	 * This function draws the 3D object by setting the object program as the current shader program,
	 * binding the vertex array object (VAO), binding the 2D texture, binding the element buffer object (EBO),
	 * and drawing the elements using the faces indices.
	 */
	void inline draw3dObj();

	/**
	 * @brief Draws the plane.
	 *
	 * This function draws the plane by binding the frame buffer, clearing the color and depth buffers,
	 * setting the viewport, binding the rendered texture, and rendering the 3D object reflected on the plane.
	 * It then generates the mipmap for the texture and draws the plane with the rendered texture and the skybox texture.
	 */
	void inline drawPlane();
};

