//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Maths.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();

	const float ASPECT_RATIO{ (float) m_Width / (float)m_Height };

	for (int px{}; px < m_Width; ++px)
	{
		for (int py{}; py < m_Height; ++py)
		{
			///GRADIENT
			/*
			float gradient = px / static_cast<float>(m_Width);
			gradient += py / static_cast<float>(m_Width);
			gradient /= 2.0f;

			ColorRGB finalColor{ gradient, gradient, gradient };
			*/

			///Colors
			//float rayDx{(2.f * (px + 0.5f) / m_Width - 1.f) * ASPECT_RATIO };
			//float rayDy{1.f - 2.f * (py + 0.5f) / m_Height};
			//Vector3 rayDirection{rayDx, rayDy, 1.f};
			//
			//rayDirection.Normalize();
			//
			//Ray hitRay{ {0.f,0.f,0.f}, rayDirection };
			//ColorRGB finalColor{ rayDirection.x, rayDirection.y, rayDirection.z };

			///Red Circle
			//float rayDx{(2.f * (px + 0.5f) / m_Width - 1.f) * ASPECT_RATIO };
			//float rayDy{1.f - 2.f * (py + 0.5f) / m_Height};
			//Vector3 rayDirection{ rayDx, rayDy, 1.f };
			//
			//rayDirection.Normalize();
			//
			//Ray viewRay{ {0,0,0}, rayDirection };
			//ColorRGB finalColor{};
			//HitRecord closestHit{};
			//
			//Sphere testSphere{ {0.f, 0.f, 100.f}, 50.f, 0 };
			//
			//GeometryUtils::HitTest_Sphere(testSphere, viewRay, closestHit);
			//
			//if (closestHit.didHit)
			//{
			//	finalColor = materials[closestHit.materialIndex]->Shade();
			//}
			
			///Trans Circle
			//float rayDx{(2.f * (px + 0.5f) / m_Width - 1.f) * ASPECT_RATIO };
			//float rayDy{1.f - 2.f * (py + 0.5f) / m_Height};
			//Vector3 rayDirection{ rayDx, rayDy, 1.f };
			//
			//rayDirection.Normalize();
			//
			//Ray viewRay{ {0,0,0}, rayDirection };
			//ColorRGB finalColor{};
			//HitRecord closestHit{};
			//
			//Sphere testSphere{ {0.f, 0.f, 100.f}, 50.f, 0 };
			//
			//GeometryUtils::HitTest_Sphere(testSphere, viewRay, closestHit);
			//
			//if (closestHit.didHit)
			//{
			//	const float scaled_t{ (closestHit.t - 50.f) / 40.f };
			//	finalColor = { scaled_t, scaled_t, scaled_t };
			//}
			///EndExcercise
			//float rayDx{ (2.f * (px + 0.5f) / m_Width - 1.f) * ASPECT_RATIO };
			//float rayDy{ 1.f - 2.f * (py + 0.5f) / m_Height };
			//Vector3 rayDirection{ rayDx, rayDy, 1.f };
			//
			//rayDirection.Normalize();
			//
			//Ray viewRay{ {0,0,0}, rayDirection };
			//ColorRGB finalColor{};
			//HitRecord closestHit{};
			//
			//pScene->GetClosestHit(viewRay, closestHit);
			//
			//if (closestHit.didHit)
			//{
			//	finalColor = materials[closestHit.materialIndex]->Shade();
			//}
			 
			///TEST EXTRA PLANE
			//float rayDx{ (2.f * (px + 0.5f) / m_Width - 1.f) * ASPECT_RATIO };
			//float rayDy{ 1.f - 2.f * (py + 0.5f) / m_Height };
			//Vector3 rayDirection{ rayDx, rayDy, 1.f };
			//
			//rayDirection.Normalize();
			//
			//Ray viewRay{ {0,0,0}, rayDirection };
			//ColorRGB finalColor{};
			//HitRecord closestHit{};
			//Plane testPlane{ {0.f, -50.f, 0.f}, {0.f, 1.f, 0.f}, 0 };
			//GeometryUtils::HitTest_Plane(testPlane, viewRay, closestHit);
			//
			//if (closestHit.didHit)
			//{
			//	finalColor = materials[closestHit.materialIndex]->Shade();
			//}

			///TEST EXTRA PLANE
			//float rayDx{ (2.f * (px + 0.5f) / m_Width - 1.f) * ASPECT_RATIO };
			//float rayDy{ 1.f - 2.f * (py + 0.5f) / m_Height };
			//Vector3 rayDirection{ rayDx, rayDy, 1.f };
			//
			//rayDirection.Normalize();
			//
			//Ray viewRay{ {0,0,0}, rayDirection };
			//ColorRGB finalColor{};
			//HitRecord closestHit{};
			//Plane testPlane{ {0.f, -50.f, 0.f}, {0.f, 1.f, 0.f}, 0 };
			//GeometryUtils::HitTest_Plane(testPlane, viewRay, closestHit);
			//
			//if (closestHit.didHit)
			//{
			//	const float scaled_t = closestHit.t / 500.f;
			//	finalColor = {scaled_t, scaled_t, scaled_t};
			//}

			//const float FOV = tan(camera.fovAngle * (PI / 180.f) / 2.f);
			//const float rayDx{ (2.f * (px + 0.5f) / m_Width - 1.f) * ASPECT_RATIO * FOV };
			//const float rayDy{ (1.f - 2.f * (py + 0.5f) / m_Height) * FOV };
			//Vector3 rayDirection{ rayDx, rayDy, 1.f };
			//
			//rayDirection.Normalize();
			//
			//const Matrix cameraToWorld = camera.CalculateCameraToWorld();
			//
			//rayDirection = cameraToWorld.TransformVector(rayDirection);
			//
			//Ray viewRay{ camera.origin, rayDirection };
			//ColorRGB finalColor{};
			//HitRecord closestHit{};
			//
			//pScene->GetClosestHit(viewRay, closestHit);
			//
			//if (closestHit.didHit)
			//{
			//	finalColor = materials[closestHit.materialIndex]->Shade();
			//
			//	for (int i{ 0 }; i < lights.size(); ++i)
			//	{	
			//		const Vector3 LIGHT_DIRECTION = LightUtils::GetDirectionToLight(lights[i], closestHit.origin);
			//		float length = LIGHT_DIRECTION.Magnitude();
			//		Ray shadowRay{ closestHit.origin, LIGHT_DIRECTION.Normalized(), 0.0001f, length};
			//		//if(pScene->DoesHit(shadowRay))
			//			//finalColor *= 0.5f;
			//	}
			//}
			//
			////Update Color in Buffer
			//finalColor.MaxToOne();
			//
			//m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
			//	static_cast<uint8_t>(finalColor.r * 255),
			//	static_cast<uint8_t>(finalColor.g * 255),
			//	static_cast<uint8_t>(finalColor.b * 255));

			const float FOV = tan(camera.fovAngle * (PI / 180.f) / 2.f);
			const float rayDx{ (2.f * (px + 0.5f) / m_Width - 1.f) * ASPECT_RATIO * FOV };
			const float rayDy{ (1.f - 2.f * (py + 0.5f) / m_Height) * FOV };
			Vector3 rayDirection{ rayDx, rayDy, 1.f };

			rayDirection.Normalize();

			const Matrix cameraToWorld = camera.CalculateCameraToWorld();

			rayDirection = cameraToWorld.TransformVector(rayDirection);

			Ray viewRay{ camera.origin, rayDirection };
			ColorRGB finalColor{};
			HitRecord closestHit{};

			pScene->GetClosestHit(viewRay, closestHit);

			if (closestHit.didHit)
			{
				finalColor = materials[closestHit.materialIndex]->Shade();

				for (int i{ 0 }; i < lights.size(); ++i)
				{
					const Light CURRENT_LIGHT{ lights[i] };
					const Vector3 LIGHT_DIRECTION = LightUtils::GetDirectionToLight(CURRENT_LIGHT, closestHit.origin);
					float length = LIGHT_DIRECTION.Magnitude();
					Ray shadowRay{ closestHit.origin, LIGHT_DIRECTION.Normalized(), 0.0001f, length };

					if (!pScene->DoesHit(shadowRay)) {
						Vector3 HIT_TO_CAMERA_DIRECTION = (closestHit.origin, camera.origin).Normalized();

						const float ANGLE_BETWEEN = Vector3::Dot(closestHit.normal, LIGHT_DIRECTION.Normalized());
						const ColorRGB BRDF = materials[closestHit.materialIndex]->Shade(closestHit, LIGHT_DIRECTION.Normalized(), HIT_TO_CAMERA_DIRECTION);

						if (ANGLE_BETWEEN >= 0.0f) {
							const ColorRGB LIGHT_RADIANCE = LightUtils::GetRadiance(CURRENT_LIGHT, closestHit.origin);
							
							finalColor += BRDF
								* LIGHT_RADIANCE * ANGLE_BETWEEN;
						}
					}
				}
			}

			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}
	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}
