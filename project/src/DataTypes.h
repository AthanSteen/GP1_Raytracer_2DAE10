#pragma once
#include <stdexcept>
#include <vector>

#include "Maths.h"


namespace dae
{
#pragma region GEOMETRY
	struct Sphere
	{
		Vector3 origin{};
		float radius{};

		unsigned char materialIndex{ 0 };
	};

	struct Plane
	{
		Vector3 origin{};
		Vector3 normal{};

		unsigned char materialIndex{ 0 };
	};

	enum class TriangleCullMode
	{
		FrontFaceCulling,
		BackFaceCulling,
		NoCulling
	};

	struct Triangle
	{
		Triangle() = default;
		Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2, const Vector3& _normal) :
			v0{ _v0 }, v1{ _v1 }, v2{ _v2 }, normal{ _normal.Normalized() } {}

		Triangle(const Vector3& _v0, const Vector3& _v1, const Vector3& _v2) :
			v0{ _v0 }, v1{ _v1 }, v2{ _v2 }
		{
			const Vector3 edgeV0V1 = v1 - v0;
			const Vector3 edgeV0V2 = v2 - v0;
			normal = Vector3::Cross(edgeV0V1, edgeV0V2).Normalized();
		}

		Vector3 v0{};
		Vector3 v1{};
		Vector3 v2{};

		Vector3 normal{};

		TriangleCullMode cullMode{};
		unsigned char materialIndex{};
	};

	struct TriangleMesh
	{
		TriangleMesh() = default;
		TriangleMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, TriangleCullMode _cullMode) :
			positions(std::move(_positions)), indices(std::move(_indices)), cullMode(_cullMode)
		{
			CalculateNormals();
			UpdateTransforms();
		}

		TriangleMesh(const std::vector<Vector3>& _positions, const std::vector<int>& _indices, const std::vector<Vector3>& _normals, TriangleCullMode _cullMode) :
			positions(_positions), indices(_indices), normals(_normals), cullMode(_cullMode)
		{
			UpdateTransforms();
		}

		std::vector<Vector3> positions{};
		std::vector<Vector3> normals{};
		std::vector<int> indices{};
		unsigned char materialIndex{};

		TriangleCullMode cullMode{ TriangleCullMode::BackFaceCulling };

		Matrix rotationTransform{};
		Matrix translationTransform{};
		Matrix scaleTransform{};

		Vector3 minAABB;
		Vector3 maxAABB;

		Vector3 transformedMinAABB;
		Vector3 transformedMaxAABB;

		std::vector<Vector3> transformedPositions{};
		std::vector<Vector3> transformedNormals{};

		void Translate(const Vector3& translation)
		{
			translationTransform = Matrix::CreateTranslation(translation);
		}

		void RotateY(float yaw)
		{
			rotationTransform = Matrix::CreateRotationY(yaw);
		}

		void Scale(const Vector3& scale)
		{
			scaleTransform = Matrix::CreateScale(scale);
		}

		void AppendTriangle(const Triangle& triangle, bool ignoreTransformUpdate = false)
		{
			int startIndex = static_cast<int>(positions.size());

			positions.reserve(positions.size() + 3);
			indices.reserve(indices.size() + 3);
			normals.reserve(normals.size() + 1);

			positions.emplace_back(triangle.v0);
			positions.emplace_back(triangle.v1);
			positions.emplace_back(triangle.v2);

			indices.emplace_back(startIndex);
			indices.emplace_back(++startIndex);
			indices.emplace_back(++startIndex);

			normals.emplace_back(triangle.normal);

			//Not ideal, but making sure all vertices are updated
			if (!ignoreTransformUpdate)
				UpdateTransforms();
		}

		void CalculateNormals()
		{
			normals.clear();
			normals.reserve(indices.size() / 3);

			for (size_t i{ 0 }; i < indices.size(); i += 3)
			{
				int index0 = indices[i];
				int index1 = indices[i + 1];
				int index2 = indices[i + 2];

				const Vector3& v0 = positions[index0];
				const Vector3& v1 = positions[index1];
				const Vector3& v2 = positions[index2];

				Vector3 edge1 = v1 - v0;
				Vector3 edge2 = v2 - v0;

				Vector3 normal = Vector3::Cross(edge1, edge2).Normalized();

				normals.emplace_back(normal);
			}
		}

		void UpdateTransforms()
		{
			Matrix finalTransform = scaleTransform * rotationTransform * translationTransform;


			transformedPositions.clear();
			transformedNormals.clear();

			transformedPositions.reserve(positions.size());
			transformedNormals.reserve(normals.size());

			for (auto& n : normals)
			{
				transformedNormals.emplace_back(
					(rotationTransform * scaleTransform).TransformVector(n).Normalized());

			}

			for (auto& p : positions)
			{
				transformedPositions.emplace_back(
					finalTransform.TransformPoint(p));
			}

			UpdateTransformedAABB(finalTransform);
		}

		void UpdateAABB()
		{
			if (positions.size() > 0)
			{
				minAABB = positions[0];
				maxAABB = positions[0];
				for (auto& p : positions)
				{
					minAABB = Vector3::Min(p, minAABB);
					maxAABB = Vector3::Max(p, maxAABB);
				}
			}
		}

		void UpdateTransformedAABB(const Matrix& finalTransform)
		{
			Vector3 tMinAABB = finalTransform.TransformPoint(minAABB);
			Vector3 tMaxAABB = tMinAABB;

			const std::vector<Vector3> corners = {
				{minAABB.x, minAABB.y, minAABB.z}, {maxAABB.x, minAABB.y, minAABB.z},
				{maxAABB.x, minAABB.y, maxAABB.z}, {minAABB.x, minAABB.y, maxAABB.z},
				{minAABB.x, maxAABB.y, minAABB.z}, {maxAABB.x, maxAABB.y, minAABB.z},
				{maxAABB.x, maxAABB.y, maxAABB.z}, {minAABB.x, maxAABB.y, maxAABB.z}
			};

			for (const auto& corner : corners)
			{
				const Vector3 tAABB = finalTransform.TransformPoint(corner);
				tMinAABB = Vector3::Min(tAABB, tMinAABB);
				tMaxAABB = Vector3::Max(tAABB, tMaxAABB);
			}

			transformedMinAABB = tMinAABB;
			transformedMaxAABB = tMaxAABB;
		}
	};
#pragma endregion
#pragma region LIGHT
	enum class LightType
	{
		Point,
		Directional
	};

	struct Light
	{
		Vector3 origin{};
		Vector3 direction{};
		ColorRGB color{};
		float intensity{};

		LightType type{};
	};
#pragma endregion
#pragma region MISC
	struct Ray
	{
		Vector3 origin{};
		Vector3 direction{};

		float min{ 0.0001f };
		float max{ FLT_MAX };
	};

	struct HitRecord
	{
		Vector3 origin{};
		Vector3 normal{};
		float t = FLT_MAX;

		bool didHit{ false };
		unsigned char materialIndex{ 0 };
	};
#pragma endregion
}