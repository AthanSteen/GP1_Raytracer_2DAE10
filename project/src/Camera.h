#pragma once
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Maths.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;

		Camera(const Vector3& _origin, float _fovAngle) :
			origin{ _origin },
			fovAngle{ _fovAngle }
		{
		}


		Vector3 origin{};
		float fovAngle{ 90.f };

		Vector3 forward{ Vector3::UnitZ };
		//Vector3 forward{ 0.266f, -0.453f, 0.860f };
		Vector3 up{ Vector3::UnitY };
		Vector3 right{ Vector3::UnitX };

		float totalPitch{ 0.f };
		float totalYaw{ 0.f };

		Matrix cameraToWorld{};


		Matrix CalculateCameraToWorld()
		{
			right = Vector3::Cross(Vector3::UnitY, forward).Normalized();
			up = Vector3::Cross(forward, right).Normalized();

			const Matrix ONB = Matrix({ right.x, right.y, right.z }, { up.x, up.y, up.z }, { forward.x, forward.y, forward.z }, { origin.x, origin.y, origin.z });
			return ONB;
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();
			const float baseSpeed = 10.f;

			//Keyboard Input
			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);

			//Mouse Input
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			if (pKeyboardState[SDL_SCANCODE_W])
			{
				origin += forward * baseSpeed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_A])
			{
				origin -= right * baseSpeed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_S])
			{
				origin -= forward * baseSpeed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_D])
			{
				origin += right * baseSpeed * deltaTime;
			}

			if (mouseState == 1) {
				origin -= forward * mouseY * deltaTime * 0.05f;

				totalYaw -= mouseX * deltaTime * 0.005f;
				
			}
			if (mouseState == 4) {

				totalYaw -= mouseX * deltaTime * 0.005f;
				totalPitch -= mouseY * deltaTime * 0.005f;
			}

			const Matrix finalRotation = Matrix::CreateRotation(totalPitch, totalYaw, 0.f);
			forward = finalRotation.TransformVector(Vector3::UnitZ);
			forward.Normalize();
		}
	};
}
