#include "Scene.h"
#include "Utils.h"
#include "Material.h"

namespace dae {

#pragma region Base Scene
	//Initialize Scene with Default Solid Color Material (RED)
	Scene::Scene() :
		m_Materials({ new Material_SolidColor({1,0,0}) })
	{
		m_SphereGeometries.reserve(32);
		m_PlaneGeometries.reserve(32);
		m_TriangleMeshGeometries.reserve(32);
		m_Lights.reserve(32);
	}

	Scene::~Scene()
	{
		for (auto& pMaterial : m_Materials)
		{
			delete pMaterial;
			pMaterial = nullptr;
		}

		m_Materials.clear();
	}

	void Scene::Clear()
	{
		m_Camera.Clear();

		for (auto& pMaterial : m_Materials)
		{
			delete pMaterial;
		}
		m_Materials.clear();

		m_SphereGeometries.clear();
		m_PlaneGeometries.clear();
		m_TriangleMeshGeometries.clear();
		m_Lights.clear();
	}

	void dae::Scene::GetClosestHit(const Ray& ray, HitRecord& closestHit) const
	{
		HitRecord subHitRecord{};


		for (int i{ 0 }; i < m_SphereGeometries.size(); ++i)
		{
			GeometryUtils::HitTest_Sphere(m_SphereGeometries[i], ray, subHitRecord);
			if (closestHit.t > subHitRecord.t) closestHit = subHitRecord;
		}
		for (int i{ 0 }; i < m_PlaneGeometries.size(); ++i)
		{
			GeometryUtils::HitTest_Plane(m_PlaneGeometries[i], ray, subHitRecord);
			if (closestHit.t > subHitRecord.t) closestHit = subHitRecord;
		}
		for (int i{ 0 }; i < m_TriangleMeshGeometries.size(); ++i)
		{
			GeometryUtils::HitTest_TriangleMesh(m_TriangleMeshGeometries[i], ray, subHitRecord);
			if (closestHit.t > subHitRecord.t) closestHit = subHitRecord;
		}

	}

	bool Scene::DoesHit(const Ray& ray) const
	{
		for (int i{ 0 }; i < m_SphereGeometries.size(); ++i)
		{
			if(GeometryUtils::HitTest_Sphere(m_SphereGeometries[i], ray)) return true;
		}
		for (int i{ 0 }; i < m_PlaneGeometries.size(); ++i)
		{
			if(GeometryUtils::HitTest_Plane(m_PlaneGeometries[i], ray)) return true;
		}
		for (int i{ 0 }; i < m_TriangleMeshGeometries.size(); ++i)
		{
			if(GeometryUtils::HitTest_TriangleMesh(m_TriangleMeshGeometries[i], ray)) return true;
		}

		return false;
	}

#pragma region Scene Helpers
	Sphere* Scene::AddSphere(const Vector3& origin, float radius, unsigned char materialIndex)
	{
		Sphere s;
		s.origin = origin;
		s.radius = radius;
		s.materialIndex = materialIndex;

		m_SphereGeometries.emplace_back(s);
		return &m_SphereGeometries.back();
	}

	Plane* Scene::AddPlane(const Vector3& origin, const Vector3& normal, unsigned char materialIndex)
	{
		Plane p;
		p.origin = origin;
		p.normal = normal;
		p.materialIndex = materialIndex;

		m_PlaneGeometries.emplace_back(p);
		return &m_PlaneGeometries.back();
	}

	TriangleMesh* Scene::AddTriangleMesh(TriangleCullMode cullMode, unsigned char materialIndex)
	{
		TriangleMesh m{};
		m.cullMode = cullMode;
		m.materialIndex = materialIndex;

		m_TriangleMeshGeometries.emplace_back(m);
		return &m_TriangleMeshGeometries.back();
	}

	Light* Scene::AddPointLight(const Vector3& origin, float intensity, const ColorRGB& color)
	{
		Light l;
		l.origin = origin;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Point;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	Light* Scene::AddDirectionalLight(const Vector3& direction, float intensity, const ColorRGB& color)
	{
		Light l;
		l.direction = direction;
		l.intensity = intensity;
		l.color = color;
		l.type = LightType::Directional;

		m_Lights.emplace_back(l);
		return &m_Lights.back();
	}

	unsigned char Scene::AddMaterial(Material* pMaterial)
	{
		m_Materials.push_back(pMaterial);
		return static_cast<unsigned char>(m_Materials.size() - 1);
	}
#pragma endregion
#pragma endregion

#pragma region SCENE W4 Reference
void Scene_W4_ReferenceScene::Initialize()
{
	sceneName = "Reference Scene";
	m_Camera.origin = { 0, 3, -9 };
	m_Camera.fovAngle = 45.f;

	const auto matCT_GrayRoughMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, 1.f));
	const auto matCT_GrayMediumMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .6f));
	const auto matCT_GraySmoothMetal = AddMaterial(new Material_CookTorrence({ .972f, .960f, .915f }, 1.f, .1f));

	const auto matCT_GrayRoughPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, 1.f));
	const auto matCT_GrayMediumPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, .6f));
	const auto matCT_GraySmoothPlastic = AddMaterial(new Material_CookTorrence({ .75f, .75f, .75f }, 0.f, .1f));

	const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, .57f, .57f }, 1.f));
	const auto matLambert_White = AddMaterial(new Material_Lambert({ 1.f, 1.f, 1.f }, 1.f));

	AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f, -1.f }, matLambert_GrayBlue); // BACK
	AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f, 0.f }, matLambert_GrayBlue);   // BOTTOM
	AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f, 0.f }, matLambert_GrayBlue);  // TOP
	AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f, 0.f }, matLambert_GrayBlue);   // RIGHT
	AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f, 0.f }, matLambert_GrayBlue);   // LEFT

	AddSphere({ -1.75f, 1.f, 0.f }, .75f, matCT_GrayRoughMetal);
	AddSphere({ 0.f, 1.f, 0.f }, .75f, matCT_GrayMediumMetal);
	AddSphere({ 1.75f, 1.f, 0.f }, .75f, matCT_GraySmoothMetal);

	AddSphere({ -1.75f, 3.f, 0.f }, .75f, matCT_GrayRoughPlastic);
	AddSphere({ 0.f, 3.f, 0.f }, .75f, matCT_GrayMediumPlastic);
	AddSphere({ 1.75f, 3.f, 0.f }, .75f, matCT_GraySmoothPlastic);

	const Triangle baseTriangle = { { -.75f, 1.5f, 0.f }, { .75f, 0.f, 0.f }, { -.75f, 0.f, 0.f } };

	m_Meshes[0] = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
	m_Meshes[0]->AppendTriangle(baseTriangle, true);
	m_Meshes[0]->Translate({ -1.75f, 4.5f, 0.f });
	m_Meshes[0]->UpdateAABB();
	m_Meshes[0]->UpdateTransforms();

	m_Meshes[1] = AddTriangleMesh(TriangleCullMode::FrontFaceCulling, matLambert_White);
	m_Meshes[1]->AppendTriangle(baseTriangle, true);
	m_Meshes[1]->Translate({ 0.f, 4.5f, 0.f });
	m_Meshes[1]->UpdateAABB();
	m_Meshes[1]->UpdateTransforms();

	m_Meshes[2] = AddTriangleMesh(TriangleCullMode::NoCulling, matLambert_White);
	m_Meshes[2]->AppendTriangle(baseTriangle, true);
	m_Meshes[2]->Translate({ 1.75f, 4.5f, 0.f });
	m_Meshes[2]->UpdateAABB();
	m_Meshes[2]->UpdateTransforms();

	AddPointLight({ 0.f, 5.5f, 5.f }, 50.f, { 1.f, .61f, .45f }); // BackLight
	AddPointLight({ -2.5f, 5.f, -5.f }, 70.f, { 1.f, .8f, .45f }); // Front Light Left
	AddPointLight({ 2.5f, 5.f, -5.f }, 50.f, { .34f, .47f, .68f });
}

void Scene_W4_ReferenceScene::Update(Timer* pTimer)
{
	Scene::Update(pTimer);

	const float ROTATION_ANGLE{ PI_DIV_2 * pTimer->GetTotal() };
	for (const auto m : m_Meshes)
	{
		m->RotateY(ROTATION_ANGLE);
		m->UpdateTransforms();
	}
}
#pragma endregion

#pragma region SCENE W4 Reference
void Scene_W4_BunnyScene::Initialize()
{
	sceneName = "Bunny Scene";
	m_Camera.origin = { 0, 3, -9 };
	m_Camera.fovAngle = 45.f;

	//Materials
	const auto matLambert_GrayBlue = AddMaterial(new Material_Lambert({ .49f, .57f, 0.57f }, 1.f));
	const auto matLambert_White = AddMaterial(new Material_Lambert(colors::White, 1.f));

	//Planes
	AddPlane({ 0.f, 0.f, 10.f }, { 0.f, 0.f,-1.f }, matLambert_GrayBlue);
	AddPlane({ 0.f, 0.f, 0.f }, { 0.f, 1.f,0.f }, matLambert_GrayBlue);
	AddPlane({ 0.f, 10.f, 0.f }, { 0.f, -1.f,0.f }, matLambert_GrayBlue);
	AddPlane({ 5.f, 0.f, 0.f }, { -1.f, 0.f,0.f }, matLambert_GrayBlue);
	AddPlane({ -5.f, 0.f, 0.f }, { 1.f, 0.f,0.f }, matLambert_GrayBlue);

	pMesh = AddTriangleMesh(TriangleCullMode::BackFaceCulling, matLambert_White);
	Utils::ParseOBJ("Resources/lowpoly_bunny.obj",
		pMesh->positions,
		pMesh->normals,
		pMesh->indices);

	pMesh->Scale({ 2.f, 2.f, 2.f });
	pMesh->UpdateAABB();
	pMesh->UpdateTransforms();

	AddPointLight(Vector3{ 0.f, 5.f, 5.f }, 50.f, ColorRGB{ 1.f, .61f, .45f });
	AddPointLight(Vector3{ -2.5f, 5.f, -5.f }, 50.f, ColorRGB{ 1.f, .8f, .45f });
	AddPointLight(Vector3{ 2.5f, 2.5f, -5.f }, 50.f, ColorRGB{ .34f, .47f, .68f });
}

void Scene_W4_BunnyScene::Update(Timer* pTimer)
{
	Scene::Update(pTimer);

	const float ROTATION_ANGLE{ PI_DIV_2 * pTimer->GetTotal() };

	pMesh->RotateY(ROTATION_ANGLE);
	pMesh->UpdateTransforms();
}
#pragma endregion
}