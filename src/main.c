#include <stdio.h>

#include <GLFW/glfw3.h>

int main() {
	if (glfwInit() == GLFW_FALSE) {
		printf("Unable to initialise GLFW\n");
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(640, 480, "Game Engine", NULL, NULL);
	if (window == NULL) {
		printf("Unable to create GLFW window\n");
		return -1;
	}

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}

	printf("Successfully initialised GLFW and created window\n");

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
