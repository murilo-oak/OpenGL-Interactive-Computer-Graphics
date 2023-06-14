#pragma once
#include "../ThirdParty/include/glm/glm.hpp"
#include "../ThirdParty/include/glm/gtc/matrix_transform.hpp"
#include "../ThirdParty/include/glm/gtc/type_ptr.hpp"
#include "../ThirdParty/include/GL/glew.h"
#include "../ThirdParty/include/GL/freeglut.h"

class Camera
{
public:
	//Position coordinates (x,y,z)
	glm::vec3 m_position{};

	//Polar coordinates (angle1, angle2, r)
	glm::vec2 m_angle{ 0.90f, 0.90f };
	float m_radius = 3;
	
	glm::mat4 m_projection{};
	glm::mat4 m_model{};
	glm::mat4 m_view{};

	glm::mat3 m_mv{};
	glm::mat4 m_mv4{};
	glm::mat4 m_mvp{};
	glm::mat4 m_invMv4{};

	/**
	 * @brief Constructs a Camera object with the given position and radius.
	 *
	 * This constructor initializes a Camera object with the specified position and radius.
	 * The position represents the initial position of the camera in 3D space, and the radius
	 * represents the distance between the camera position and the center of the scene.
	 *
	 * @param position The initial position of the camera.
	 * @param radius The distance between the camera position and the center of the scene.
	 *
	 * @note The radius must not be zero.
	 */
	Camera(glm::vec3 position = glm::vec3(), float radius = 3.f);
	
	/**
	 * @brief Destructor for the Camera object.
	 *
	 * This destructor is empty and does not perform any specific operations.
	 * It is provided to ensure proper cleanup and deallocation of resources
	 * if necessary in future implementations.
	 */
	~Camera();

	/**
	 * @brief Updates the camera position, view matrix, and MVP matrix.
	 *
	 * This function updates the camera position by calling the `updatePosition()` function,
	 * sets the view matrix by calling the `setViewMat()` function,
	 * and updates the Model-View-Projection (MVP) matrix by calling the `updateMVP()` function.
	 *
	 * It is important to call this function whenever there is a change in the camera position
	 * or when updating the view or projection matrices to ensure that the MVP matrix and related
	 * matrices are up to date.
	 */
	void update();

	/**
	 * @brief Updates the camera position based on the given delta radius.
	 *
	 * This function updates the camera's position by adjusting the radius based on the given delta radius.
	 * It calculates the new camera position using spherical coordinates and updates the corresponding member variable.
	 *
	 * @param deltaRadius The change in radius to be applied to the camera position.
	 */
	void updatePosition(float deltaRadius = 0);

	/**
	 * @brief Sets the view matrix of the camera.
	 *
	 * This function sets the view matrix of the camera based on its current position and orientation.
	 * It uses the camera's position, the target point (0, 0, 0), and the up vector (0, 1, 0) to calculate
	 * the view matrix and updates the corresponding member variable.
	 */
	void setViewMat();

	/**
	 * @brief Sets the projection matrix of the camera.
	 *
	 * This function sets the projection matrix of the camera based on the given window height and width.
	 * It uses the field of view angle, the aspect ratio and the near and far clipping planes to create the projection matrix.
	 * The resulting matrix is then assigned to the corresponding member variable.
	 *
	 * @param windowHeight The height of the window.
	 * @param windowWidth The width of the window.
	 */
	void setProjectionMat(unsigned int windowHeight, unsigned int windowWidth);

	/**
	 * @brief Sets the model matrix of the camera.
	 *
	 * This function sets the model matrix of the camera to an identity matrix.
	 * The model matrix is responsible for transforming the vertices of the objects
	 * in the scene from model space to world space. By setting it to the identity matrix,
	 * no transformation is applied, resulting in the objects being rendered in their original
	 * model space coordinates.
	 */
	void setModelMat();

	/**
	 * @brief Sets the Model-View-Projection (MVP) matrices of the camera.
	 *
	 * This function sets the Model-View-Projection (MVP) matrices of the camera
	 * by calling the functions to set the projection matrix, view matrix, and model matrix.
	 * After setting these matrices, it updates the MVP matrix and related matrices
	 * by calling the `updateMVP()` function.
	 *
	 * @param windowHeight The height of the window or viewport.
	 * @param windowWidth The width of the window or viewport.
	 */
	void setMVP(unsigned int windowHeight, unsigned int windowWidth);

	/**
	 * @brief Updates the Model-View-Projection (MVP) matrices of the camera.
	 *
	 * This function updates the Model-View-Projection (MVP) matrix of the camera
	 * by multiplying the projection matrix, model matrix, and view matrix together.
	 * The MVP matrix is commonly used in rendering to transform the vertices of objects
	 * from model space to clip space, which is necessary for correct rendering on the screen.
	 *
	 * Additionally, this function calculates the Model-View (MV) matrix by multiplying
	 * the model matrix and view matrix, and the inverse transpose of the MV matrix.
	 * The MV matrix is used for transforming normals and is required for proper lighting calculations.
	 *
	 * It is important to call this function whenever any of the camera's matrices (model, view, projection)
	 * are modified to ensure that the MVP matrix and related matrices are up to date.
	 */
	void updateMVP();
};

