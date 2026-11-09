#pragma once
#include "Serialization/Json.h"

#define GLM_ENABLE_EXPERIMENTAL 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/norm.hpp>
#include <glm/gtx/quaternion.hpp>



namespace nu
{
    struct Transform
    {
        glm::vec3 position{ 0.0f };
		glm::quat rotation{ glm::identity<glm::quat>() };
        glm::vec3  scale{ 1.0f };

        void SetRotation(const glm::vec3 euler) { rotation = glm::quat(glm::radians(euler)); }
		void SetRotation(const glm::quat& qRotation) { rotation = glm::normalize(qRotation); }


        const glm::mat4 GetMatrix() const
        {
            auto mxt = glm::translate(position);
            auto mxr = glm::toMat4(rotation);
            auto mxs = glm::scale(scale);

            return mxt * mxr * mxs;
		}

        glm::vec3 Forward() const { return rotation * glm::vec3(0, 0, -1); }

        glm::vec3 Up() const { return rotation * glm::vec3(0, 1, 0); }

        glm::vec3 Right() const { return rotation * glm::vec3(1, 0, 0); }


        void Read(const json::value_t& value)
        {
            JSON_READ(value, position);

            glm::vec3 euler;
            JSON_READ_NAME(value, "rotation", euler);
            JSON_READ(value, scale);
        }
    };
}