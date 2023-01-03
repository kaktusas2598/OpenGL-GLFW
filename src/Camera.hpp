#ifndef __Camera__
#define __Camera__

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera {
    public:
    private:

        glm::vec3 position = glm::vec3(0.0f, 0.0f, 3.0f);
        glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::vec3 direction = glm::normalize(position - target); // This vector actually points in the opposite direction
};

#endif // __Camera__
