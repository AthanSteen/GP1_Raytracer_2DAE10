//External includes
#include "SDL.h"
#include "SDL_surface.h"

#define PARALLEL_EXECUTION

//Project includes
#include "Renderer.h"
#include "Maths.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"
#include <execution>

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

	const float ASPECT_RATIO{ (float)m_Width / (float)m_Height };

	const float FOV = tan(camera.fovAngle * (PI / 180.f) / 2.f);
	const Matrix cameraToWorld = camera.CalculateCameraToWorld();

#if defined(PARALLEL_EXECUTION)
	uint32_t amountOfPixels{ uint32_t(m_Width * m_Height) };
	std::vector<uint32_t> pixelIndices{};

	pixelIndices.reserve(amountOfPixels);
	for (uint32_t index{}; index < amountOfPixels; ++index) pixelIndices.emplace_back(index);
	
	std::for_each(std::execution::par, pixelIndices.begin(), pixelIndices.end(), [&](int i)
		{
			RenderPixel(pScene, i, FOV, ASPECT_RATIO, cameraToWorld, camera.origin);
	});
#else
	uint32_t amountOfPixels{ uint32_t(m_Width * m_Height) };

	for (uint32_t pixelIndex{}; pixelIndex < amountOfPixels; ++pixelIndex)
	{
		RenderPixel(pScene, pixelIndex, FOV, ASPECT_RATIO, cameraToWorld, camera.origin);
	}
#endif

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);
}

void Renderer::RenderPixel(Scene* pScene, uint32_t pixelIndex, float fov, float aspectRatio, const Matrix cameraToWorld, const Vector3 cameraOrigin) const
{
	auto materials{ pScene->GetMaterials() };
	const uint32_t px{ pixelIndex % m_Width }, py{ pixelIndex / m_Width };
	float rx{ px + 0.5f }, ry{ py + 0.5f };
	float cx{ (2 * (rx / float(m_Width)) - 1) * aspectRatio * fov };
	float cy{ (1 - (2 * (ry / float(m_Height)))) * fov };

	const float rayDx{ (2.f * (px + 0.5f) / m_Width - 1.f) * aspectRatio * fov };
	const float rayDy{ (1.f - 2.f * (py + 0.5f) / m_Height) * fov };
	Vector3 rayDirection{ rayDx, rayDy, 1.f };

	rayDirection.Normalize();
	rayDirection = cameraToWorld.TransformVector(rayDirection);

	Ray viewRay{ cameraOrigin, rayDirection };
	ColorRGB finalColor{};
	HitRecord closestHit{};

	pScene->GetClosestHit(viewRay, closestHit);

	const auto& LIGHTS{ pScene->GetLights() };

	if (closestHit.didHit)
	{
		for (int i{ 0 }; i < LIGHTS.size(); ++i)
		{
			const Light CURRENT_LIGHT{ LIGHTS[i] };
			const Vector3 LIGHT_DIRECTION = LightUtils::GetDirectionToLight(CURRENT_LIGHT, closestHit.origin);
			float length = LIGHT_DIRECTION.Magnitude();
			Ray shadowRay{ closestHit.origin, LIGHT_DIRECTION.Normalized(), 0.0001f, length };

			if (!pScene->DoesHit(shadowRay)) {
				const float ANGLE_BETWEEN = Vector3::Dot(closestHit.normal, LIGHT_DIRECTION.Normalized());

				if (ANGLE_BETWEEN > 0.0f) {
					Vector3 HIT_TO_CAMERA_DIRECTION = (closestHit.origin, cameraOrigin).Normalized();

					const ColorRGB LIGHT_RADIANCE = LightUtils::GetRadiance(CURRENT_LIGHT, closestHit.origin);
					const ColorRGB BRDF = materials[closestHit.materialIndex]->Shade(closestHit, LIGHT_DIRECTION.Normalized(), HIT_TO_CAMERA_DIRECTION);

					finalColor += BRDF * LIGHT_RADIANCE * std::max(0.0f, ANGLE_BETWEEN);
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

bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}