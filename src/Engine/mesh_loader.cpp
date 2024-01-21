#include "mesh_loader.h"

#include "Engine/texture.h"
#include "Engine/ColorMaterial.h"
#include "Engine/PhongMaterial.h"
#include "ObjectReader/obj_reader.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <format>
#include <iostream>

namespace
{
    xe::ColorMaterial* make_color_material(const xe::mtl_material_t& mat, std::string mtl_dir)
    {
        glm::vec4 color;
        for (auto i{0}; i < 3; ++i)
        {
            color[i] = mat.diffuse[i];
        }
        color[3] = 1.0;

        std::cout << std::format("Adding ColorMaterial {}\n", to_string(color));

        auto* material = new xe::ColorMaterial(color);
        if (!mat.diffuse_texname.empty())
        {
            auto texture = xe::create_texture(mtl_dir + "/" + mat.diffuse_texname);
            std::cout << std::format("Adding Texture {} {:1d}\n", mat.diffuse_texname, texture);
            if (texture > 0)
            {
                material->set_texture(texture);
            }
        }

        return material;
    }

    xe::PhongMaterial* make_phong_material(const xe::mtl_material_t& mat, std::string mtl_dir)
    {
        glm::vec4 color;
        for (auto i{ 0 }; i < 3; ++i)
        {
            color[i] = mat.diffuse[i];
        }

        color[3] = 1.0;

        std::cout << std::format("Adding ColorMaterial {}", to_string(color));

        auto* material = new xe::PhongMaterial(color);

        if (!mat.diffuse_texname.empty())
        {
            auto texture = xe::create_texture(mtl_dir + "/" + mat.diffuse_texname);
            std::cout << std::format("Adding Texture {} {:1d}", mat.diffuse_texname, texture);
            if (texture > 0) {
                material->set_texture(texture);
            }
        }
        material->set_ambient(glm::vec3(mat.ambient[0], mat.ambient[1], mat.ambient[2]));
        material->set_specular(glm::vec3(mat.specular[0], mat.specular[1], mat.specular[2]), mat.shininess);

        return material;
    }
}

namespace xe
{
    std::shared_ptr<Mesh> load_mesh_from_obj(const std::string& path, const std::string& mtl_dir)
    {
        auto smesh = load_smesh_from_obj(path, mtl_dir);
        if (smesh.vertex_coords.empty())
        {
            return nullptr;
        }

        const auto vertices = smesh.vertex_coords.size();
        const auto indices = 3 * smesh.faces.size();

        auto floats_per_vertex = 3;
        for (auto&& t: smesh.has_texcoords)
        {
            if (t)
            {
                floats_per_vertex += 2;
            }
        }

        if (smesh.has_normals)
        {
            floats_per_vertex += 3;
        }

        if (smesh.has_tangents)
        {
            floats_per_vertex += 4;
        }

        const auto stride = floats_per_vertex * sizeof(GLfloat);

        auto mesh = std::make_shared<Mesh>();
        mesh->allocate_index_buffer(indices * sizeof(uint16_t), GL_STATIC_DRAW);
        mesh->load_indices(0, indices * sizeof(uint16_t), smesh.faces.data());

        mesh->allocate_vertex_buffer(vertices * floats_per_vertex * sizeof(float), GL_STATIC_DRAW);
        mesh->vertex_attrib_pointer(0, 3, GL_FLOAT, floats_per_vertex * sizeof(GLfloat), 0);

        const auto vertex_pointer = static_cast<uint8_t*>(mesh->map_vertex_buffer());

        size_t offset{ 0 };
        for (auto i = 0; i < smesh.vertex_coords.size(); i++, offset += stride)
        {
            std::cout << std::format("vertex[{}] {} \n", i, to_string(smesh.vertex_coords[i]));
            std::memcpy(vertex_pointer + offset, value_ptr(smesh.vertex_coords[i]), sizeof(glm::vec3));
        }

        offset = 3 * sizeof(GLfloat);

        for (uint32_t i{0}; i < sMesh::MAX_TEXCOORDS; ++i)
        {
            if (smesh.has_texcoords[i])
            {
                mesh->vertex_attrib_pointer(1 + i, 2, GL_FLOAT, stride, offset);

                auto vertex_offset = offset;
                for (size_t j{0}; j < smesh.vertex_texcoords[i].size(); ++j, vertex_offset += stride)
                {
                    std::memcpy(vertex_pointer + vertex_offset, value_ptr(smesh.vertex_texcoords[0][j]), sizeof(glm::vec2));
                }

                offset += 2 * sizeof(GLfloat);
            }
        }

        if (smesh.has_normals)
        {
            mesh->vertex_attrib_pointer(sMesh::MAX_TEXCOORDS + 1, 3, GL_FLOAT, stride, offset);
            auto v_offset = offset;
            for (auto i = 0; i < smesh.vertex_normals.size(); i++, v_offset += stride) {
                SPDLOG_TRACE("normal[{}] {} ", i, glm::to_string(smesh.vertex_normals[i]));
                std::memcpy(vertex_pointer + v_offset, glm::value_ptr(smesh.vertex_normals[i]), sizeof(glm::vec3));
            }
            offset += 3 * sizeof(GLfloat);
        }

        if (smesh.has_tangents)
        {
            mesh->vertex_attrib_pointer(sMesh::MAX_TEXCOORDS + 2, 4, GL_FLOAT, stride, offset);
            offset += 4 * sizeof(GLfloat);
        }

        mesh->unmap_vertex_buffer();

        for (int i = 0; i < smesh.submeshes.size(); ++i)
        {
            auto sub_mesh = smesh.submeshes[i];
            std::cout << std::format("Adding sub-mesh {:4d} {:4d} {:4d}\n", i, sub_mesh.start, sub_mesh.end);
            Material* material = nullptr;
            if (sub_mesh.mat_idx >= 0)
            {
                auto mat = smesh.materials[sub_mesh.mat_idx];
                switch (mat.illum)
                {
                case 0:
                    material = make_color_material(mat, mtl_dir);
                    break;
                case 1:
                    material = make_phong_material(mat, mtl_dir);
                    break;
                }

                mesh->add_submesh(sub_mesh.start, sub_mesh.end, material);
            }
        }

        return mesh;
    }
}
