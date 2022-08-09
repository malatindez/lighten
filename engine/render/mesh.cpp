#include "mesh.hpp"
#include "core/utils.hpp"
#include <cstdlib>
#include <string_view>
#include <system_error>

namespace engine::render
{
    using namespace core;
    using namespace math;
    struct ParserContext
    {
        std::vector<unsigned int> vertex_indices;
        std::vector<unsigned int> uv_indices;
        std::vector<unsigned int> normal_indices;
        std::vector<vec3> temp_vertices;
        std::vector<vec2> temp_uvs;
        std::vector<vec3> temp_normals;
    };

    [[nodiscard]] inline bool ParsePrimitiveNoexcept(std::string_view &view,
                                                     float &rv) noexcept
    {
        char *end = nullptr;
        float f = std::strtof(view.data(), &end);
        if (errno != 0)
        {
            errno = 0;
            return false;
        }
        rv = f;
        view = view.substr(end - view.data());
        return true;
    }
    [[nodiscard]] inline bool ParsePrimitiveNoexcept(std::string_view &view,
                                                     int &rv) noexcept
    {
        char *end = nullptr;
        int i = std::strtol(view.data(), &end, 10);
        if (errno != 0)
        {
            errno = 0;
            return false;
        }
        rv = i;
        view = view.substr(end - view.data());
        return true;
    }
    template <Primitive T>
    inline void ParsePrimitive(std::string_view &view, T &f)
    {
        if (!ParsePrimitiveNoexcept(view, f))
        {
            throw std::invalid_argument("Failed to parse primitive");
        }
    }

    inline void ParseVertex(std::string_view view, ParserContext &context)
    {
        view = utils::ltrimview(view);
        vec3 vertex;
        ParsePrimitive(view, vertex.x);
        ParsePrimitive(view, vertex.y);
        ParsePrimitive(view, vertex.z);
        context.temp_vertices.emplace_back(std::move(vertex));
    }
    inline void ParseUV(std::string_view view, ParserContext &context)
    {
        view = utils::ltrimview(view);
        vec2 uv;
        ParsePrimitive(view, uv.x);
        ParsePrimitive(view, uv.y);
        context.temp_uvs.emplace_back(std::move(uv));
    }
    inline void ParseNormal(std::string_view view, ParserContext &context)
    {
        view = utils::ltrimview(view);
        vec3 normal;
        ParsePrimitive(view, normal.x);
        ParsePrimitive(view, normal.y);
        ParsePrimitive(view, normal.z);
        context.temp_normals.emplace_back(std::move(normal));
    }
    inline void ParseFace(std::string_view view, ParserContext &context)
    {
        view = utils::ltrimview(view);
        ivec3 vertex_index, uv_index, normal_index;
        ParsePrimitive(view, vertex_index.x);
        view = view.substr(1);
        ParsePrimitive(view, uv_index.x);
        view = view.substr(1);
        ParsePrimitive(view, normal_index.x);
        view = view.substr(1);
        ParsePrimitive(view, vertex_index.y);
        view = view.substr(1);
        ParsePrimitive(view, uv_index.y);
        view = view.substr(1);
        ParsePrimitive(view, normal_index.y);
        view = view.substr(1);
        ParsePrimitive(view, vertex_index.z);
        view = view.substr(1);
        ParsePrimitive(view, uv_index.z);
        view = view.substr(1);
        ParsePrimitive(view, normal_index.z);
        context.vertex_indices.push_back(vertex_index.x);
        context.vertex_indices.push_back(vertex_index.y);
        context.vertex_indices.push_back(vertex_index.z);
        context.uv_indices.push_back(uv_index.x);
        context.uv_indices.push_back(uv_index.y);
        context.uv_indices.push_back(uv_index.z);
        context.normal_indices.push_back(normal_index.x);
        context.normal_indices.push_back(normal_index.y);
        context.normal_indices.push_back(normal_index.z);
    }
    inline void ParseLine(std::string_view view, ParserContext &context)
    {
        view = utils::ltrimview(view);
        if (view.starts_with('#'))
        {
            return;
        }
        else if (view.starts_with("vt"))
        {
            ParseUV(view.substr(3), context);
        }
        else if (view.starts_with("vn"))
        {
            ParseNormal(view.substr(3), context);
        }
        else if (view.starts_with('v'))
        {
            ParseVertex(view.substr(2), context);
        }
        else if (view.starts_with('f'))
        {
            ParseFace(view.substr(2), context);
        }
    }
    std::shared_ptr<const Mesh>
        LoadMeshFromObj(std::filesystem::path const &path)
    {

        ParserContext context;
        {
            std::ifstream file(path.string(), std::ifstream::in);

            std::stringstream str_stream;
            str_stream << file.rdbuf();
            std::string data = str_stream.str();

            file.close();
            size_t begin = 0;
            std::string_view view { data.begin(), data.begin() + data.size() };
            do
            {
                size_t found = data.find_first_of('\n', begin + 1);
                ParseLine(view.substr(begin + 1, found - begin - 1), context);
                begin = found;
            }
            while (begin != std::string::npos);
        }

        auto return_value = std::make_shared<Mesh>();

        {
            for (size_t i = 0; i < context.vertex_indices.size(); ++i)
            {
                return_value->vertices.emplace_back(Vertex {
                    .position = context.temp_vertices[context.vertex_indices[i] - 1],
                    .normal = context.temp_normals[context.normal_indices[i] - 1],
                    .tex_coords = context.temp_uvs[context.uv_indices[i] - 1] });
            }
        }

        return std::const_pointer_cast<const Mesh>(return_value);
    }
} // namespace engine::render