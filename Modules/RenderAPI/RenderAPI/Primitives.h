#pragma once

#include <RenderAPI/Mesh.h>

namespace Mani
{
	namespace Primitives
	{
		inline Mesh makeCube()
		{
			return 
			{
				.name = "Cube",
				.vertices = {
						// Front face
						{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
						{{ 0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
						{{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
						{{ 0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
						{{-0.5f,  0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
						{{-0.5f, -0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},

						// Back face
						{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
						{{-0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
						{{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
						{{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
						{{ 0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
						{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},

						// Left face
						{{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
						{{-0.5f,  0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
						{{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
						{{-0.5f, -0.5f, -0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
						{{-0.5f, -0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},
						{{-0.5f,  0.5f,  0.5f}, {-1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},

						// Right face
						{{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
						{{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
						{{ 0.5f,  0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 1.0f}},
						{{ 0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f}},
						{{ 0.5f,  0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
						{{ 0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 0.0f}, {0.0f, 0.0f}},

						// Bottom face
						{{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},
						{{ 0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 1.0f}},
						{{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
						{{ 0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {1.0f, 0.0f}},
						{{-0.5f, -0.5f,  0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f}},
						{{-0.5f, -0.5f, -0.5f}, {0.0f, -1.0f, 0.0f}, {0.0f, 1.0f}},

						// Top face
						{{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
						{{-0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
						{{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
						{{ 0.5f,  0.5f,  0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 0.0f}},
						{{ 0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {1.0f, 1.0f}},
						{{-0.5f,  0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 1.0f}},
				},
				.indices = { 0, 1, 2, 3, 4, 5,
							  6, 7, 8, 9, 10,11,
							  12,13,14,15,16,17,
							  18,19,20,21,22,23,
							  24,25,26,27,28,29,
							  30,31,32,33,34,35 }
			};
		}

		inline Mesh makeSphere(unsigned int segments = 16, unsigned int rings = 16)
		{
			Mesh mesh;
			mesh.name = "Sphere";

			for (unsigned int y = 0; y <= rings; ++y) {
				float v = static_cast<float>(y) / rings;
				float phi = v * Math::PIf;

				for (unsigned int x = 0; x <= segments; ++x) {
					float u = static_cast<float>(x) / segments;
					float theta = u * 2.0f * Math::PIf;

					float xPos = Math::sin(phi) * Math::cos(theta);
					float yPos = Math::cos(phi);
					float zPos = Math::sin(phi) * Math::sin(theta);

					Vec3f position = { xPos * 0.5f, yPos * 0.5f, zPos * 0.5f };
					Vec3f normal = { xPos, yPos, zPos };
					Vec2f uv = { u, v };

					mesh.vertices.push_back({ position, normal, uv });
				}
			}

			for (unsigned int y = 0; y < rings; ++y) {
				for (unsigned int x = 0; x < segments; ++x) {
					unsigned int i0 = y * (segments + 1) + x;
					unsigned int i1 = i0 + segments + 1;

					mesh.indices.push_back(i0);
					mesh.indices.push_back(i1);
					mesh.indices.push_back(i0 + 1);

					mesh.indices.push_back(i0 + 1);
					mesh.indices.push_back(i1);
					mesh.indices.push_back(i1 + 1);
				}
			}

			return mesh;
		}

		inline Mesh makeCylinder(unsigned int segments = 32, float height = 1.0f, float radius = 0.5f)
		{
			Mesh mesh;
			mesh.name = "Cylinder";

			// Side vertices
			for (unsigned int i = 0; i <= segments; ++i) {
				float theta = static_cast<float>(i) / segments * 2.0f * Math::PIf;
				float x = Math::cos(theta);
				float z = Math::sin(theta);

				Vec3f normal = { x, 0.0f, z };
				Vec2f uvLow = { static_cast<float>(i) / segments, 0.0f };
				Vec2f uvHigh = { static_cast<float>(i) / segments, 1.0f };

				mesh.vertices.push_back({ { x * radius, -height / 2.0f, z * radius }, normal, uvLow });
				mesh.vertices.push_back({ { x * radius,  height / 2.0f, z * radius }, normal, uvHigh });
			}

			// Side indices
			for (unsigned int i = 0; i < segments; ++i) {
				unsigned int start = i * 2;
				mesh.indices.push_back(start);
				mesh.indices.push_back(start + 1);
				mesh.indices.push_back(start + 2);

				mesh.indices.push_back(start + 1);
				mesh.indices.push_back(start + 3);
				mesh.indices.push_back(start + 2);
			}

			// Top/Bottom center vertices
			unsigned int baseIndex = static_cast<unsigned int>(mesh.vertices.size());
			mesh.vertices.push_back({ {0,  height / 2.0f, 0}, {0, 1, 0}, {0.5f, 0.5f} }); // top center
			mesh.vertices.push_back({ {0, -height / 2.0f, 0}, {0, -1, 0}, {0.5f, 0.5f} }); // bottom center

			// Top/Bottom caps
			for (unsigned int i = 0; i < segments; ++i) {
				unsigned int next = (i + 1) % segments;
				unsigned int viTop = i * 2 + 1;
				unsigned int viBottom = i * 2;

				// Top cap
				mesh.indices.push_back(baseIndex); // center
				mesh.indices.push_back(next * 2 + 1);
				mesh.indices.push_back(viTop);

				// Bottom cap
				mesh.indices.push_back(baseIndex + 1); // center
				mesh.indices.push_back(viBottom);
				mesh.indices.push_back(next * 2);
			}

			return mesh;
		}
	
		inline Mesh makeQuad()
		{
			Mesh mesh;
			mesh.name = "Quad";

			mesh.vertices = 
			{
				{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // bottom-left
				{{ 0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // bottom-right
				{{ 0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // top-right
				{{-0.5f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // top-left
			};

			mesh.indices = 
			{
				0, 1, 2, // first triangle
				2, 3, 0  // second triangle
			};

			return mesh;
		}
	}
}