#pragma once
#include <fstream>
#include "Maths.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			const Vector3 ORIGIN_RAY_SPHERE{ ray.origin - sphere.origin };

			const float A{ Vector3::Dot(ray.direction, ray.direction) };
			const float B{ Vector3::Dot(2.f * ray.direction, ORIGIN_RAY_SPHERE)};
			const float C{ Vector3::Dot(ORIGIN_RAY_SPHERE, ORIGIN_RAY_SPHERE) - (sphere.radius * sphere.radius)};
			const float D{ B*B - 4.f*A*C};

			if (D <= 0.f)
			{
				hitRecord.didHit = false;
				return false;
			}

			const float NormDiscriminant = sqrtf(D);

			float t = (-B - NormDiscriminant) / (2 * A);
			if (t < ray.min) t = (-B + NormDiscriminant) / (2 * A);

			if (t < ray.min || t >= ray.max)
			{
				hitRecord.didHit = false;
				return false;
			}

			if (!ignoreHitRecord)
			{
				hitRecord.didHit = true;
				hitRecord.t = t;
				hitRecord.materialIndex = sphere.materialIndex;
				hitRecord.origin = ray.origin + t * ray.direction;
				hitRecord.normal = Vector3{ hitRecord.origin - sphere.origin}.Normalized();
			}
			return true;
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			const Vector3 ORIGIN_RAY_PLANE{ plane.origin - ray.origin };

			const float t{(Vector3::Dot(ORIGIN_RAY_PLANE, plane.normal))/(Vector3::Dot(ray.direction, plane.normal))};

			if (t < ray.min || t >= ray.max)
			{
				hitRecord.didHit = false;
				return false;
			}

			if (!ignoreHitRecord)
			{
				hitRecord.didHit = true;
				hitRecord.t = t;
				hitRecord.materialIndex = plane.materialIndex;
				hitRecord.origin = ray.origin + t * ray.direction;
				hitRecord.normal = plane.normal;
			}

			return true;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			float normalDotRayDir = Vector3::Dot(triangle.normal, ray.direction);

			bool isFrontFacing = normalDotRayDir < 0;
			switch (triangle.cullMode)
			{
			case TriangleCullMode::FrontFaceCulling:
				if (isFrontFacing == !ignoreHitRecord) return false;
				break;
			case TriangleCullMode::BackFaceCulling:
				if (isFrontFacing == ignoreHitRecord) return false;
				break;
			case TriangleCullMode::NoCulling:
			default:
				break;
			}

			if (std::abs(normalDotRayDir) < FLT_EPSILON) return false;

			float d = Vector3::Dot(triangle.normal, triangle.v0);
			float t = (d - Vector3::Dot(triangle.normal, ray.origin)) / normalDotRayDir;
			if (t < ray.min || t > ray.max) return false;

			Vector3 hitPoint = ray.origin + t * ray.direction;

			Vector3 edge0 = triangle.v1 - triangle.v0;
			Vector3 edge1 = triangle.v2 - triangle.v1;
			Vector3 edge2 = triangle.v0 - triangle.v2;

			Vector3 C0 = hitPoint - triangle.v0;
			Vector3 C1 = hitPoint - triangle.v1;
			Vector3 C2 = hitPoint - triangle.v2;

			if (Vector3::Dot(triangle.normal, Vector3::Cross(edge0, C0)) < 0) return false;
			if (Vector3::Dot(triangle.normal, Vector3::Cross(edge1, C1)) < 0) return false;
			if (Vector3::Dot(triangle.normal, Vector3::Cross(edge2, C2)) < 0) return false;

			if (!ignoreHitRecord)
			{
				hitRecord.didHit = true;
				hitRecord.t = t; 
				hitRecord.materialIndex = triangle.materialIndex;
				hitRecord.origin = hitPoint;
				hitRecord.normal = isFrontFacing ? triangle.normal : -triangle.normal;
			}

			return true;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest


		inline bool SlabTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			float tx1 = (mesh.transformedMinAABB.x - ray.origin.x) / ray.direction.x;
			float tx2 = (mesh.transformedMaxAABB.x - ray.origin.x) / ray.direction.x;

			float tmin = std::min(tx1, tx2);
			float tmax = std::max(tx1, tx2);

			float ty1 = (mesh.transformedMinAABB.y - ray.origin.y) / ray.direction.y;
			float ty2 = (mesh.transformedMaxAABB.y - ray.origin.y) / ray.direction.y;

			tmin = std::max(tmin, std::min(ty1, ty2));
			tmax = std::min(tmax, std::max(ty1, ty2));

			float tz1 = (mesh.transformedMinAABB.z - ray.origin.z) / ray.direction.z;
			float tz2 = (mesh.transformedMaxAABB.z - ray.origin.z) / ray.direction.z;

			tmin = std::max(tmin, std::min(tz1, tz2));
			tmax = std::min(tmax, std::max(tz1, tz2));

			return tmax > 0 && tmax >= tmin;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			if (!SlabTest_TriangleMesh(mesh, ray))
			{
				return false;
			}

			bool didHit = false;
			float closestT = INFINITY;

			for (size_t i{ 0 }; i < mesh.indices.size(); i += 3)
			{
				int index0 = mesh.indices[i];
				int index1 = mesh.indices[i + 1];
				int index2 = mesh.indices[i + 2];

				Triangle triangle;
				triangle.v0 = mesh.transformedPositions[index0];
				triangle.v1 = mesh.transformedPositions[index1];
				triangle.v2 = mesh.transformedPositions[index2];
				triangle.normal = mesh.transformedNormals[i / 3];
				triangle.cullMode = mesh.cullMode;
				triangle.materialIndex = mesh.materialIndex;

				if (HitTest_Triangle(triangle, ray, hitRecord, ignoreHitRecord))
				{
					if (hitRecord.t < closestT)
					{
						closestT = hitRecord.t;
						didHit = true;

						if (!ignoreHitRecord)
						{
							hitRecord = hitRecord;
						}
					}
				}
			}

			return didHit;
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}

#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			switch (light.type)
			{
				case LightType::Directional:
					return light.direction * -1.f;
				case LightType::Point:
					return { Vector3{origin, light.origin} };
				default: return{};
			}
			return {};
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			switch (light.type)
			{
				case LightType::Directional:
					return light.color * light.intensity;
					break;
				case LightType::Point:
					return light.color * (light.intensity /
						Vector3::Dot((light.origin - target), (light.origin - target)));
					break;
				default: 
					return{};
					break;
			}
			return {};
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof())
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if (std::isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (std::isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}