#pragma once
#include "include/library-pch.hpp"
#include "transform.hpp"
#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 26495) // Variable 'component' is uninitialized. Always initialize a member variable
#endif
#include "transform.hpp"
#include "game-object.hpp"
#include "camera.hpp"
#include "camera.hpp"
#include "force-fields.hpp"
#include "lights.hpp"
#include "particle-emitter.hpp"
/**
 * @brief The components namespace contains all the components that can be used in the engine
 * @todo Every component should inherit from Component class
 * @todo Every component should have static \code{.cpp} constexpr std::string_view name;\endcode member that is used to identify the component by name
 *
 * If the component doesn't have a name, \code{.cpp}static_assert(false, "Component {class name} must have a name declared");\endcode will be generated automatically
 * in generated file to make sure that the component has a name
 *
 * About forward declaring or including the component headers.
 *
 * Header including can cause issues like this:
 * * increasing the compilation time of the engine
 * * increasing the binary size of the engine
 * * causing problems with the precompiled headers
 * * causing problems with the header include order, because the components-info.hpp
 *          will be included in the engine\components\components.hpp
 *
 * Forward declaration can cause issues like this:
 * * not being able to use the components in the engine\components\components.hpp
 * * not being able to recognize the size of the component before including the header
 * * not being able to use the component in the engine\components\generated\components-info.hpp
 * * still it will be necessary to include the headers in serialization/gui files to use them
 *
 * I think we should include the headers, because it will be easier to work with the components
 *
 * If you don't want to mess up your project you should create separate file for component and/or components
 * Otherwise because they will be included in the engine\components\generated\components-info.hpp file
 * this will lead to problems with header including mentioned above
 *
 @todo Add the python pre-build step that will gather all the components identifying them by Component parent class, and then generate the header files that will contain all the components.
 * This header file will contain `mal_toolkit::parameter_pack_info<Component, Component, ...>` as the list of types and corresponding `std::array<std::string, N>` as the list of component names
 * It will modify the `engine\components\generated\components-info.hpp` file
 *
 * To iterate through all the component types you can use the following code:
 \code{.cpp}
     mal_toolkit::for_constexpr<0, components::kAmount, 1>([](auto i) constexpr -> bool {
     using component = components::component_by_id<i>;
     std::cout << component::name << std::endl;
     return true; // if false is returned the loop will break
   });
 \endcode

  * You can use it to substitute the switch statement so you can easily iterate through
  * all the components without worrying about giant and complex switch statement
  * Also, this way, you can easily add new components without modifying the switch statement at all
  * And, most importantly, for_constexpr is equivalent or even faster than the switch statement by 1.5-20% in most cases
  @note But in the most test cases the for_constexpr is better

  @todo Add a python script that will generate the component serialization/deserialization code
  * If you want to generate serialization/deserialization for the component you should add `static constexpr bool kGenerateSerialization = true;` member to the component
  *
  * Private members and members that do not have serialization specification will be ignored.
  *
  * If you want to create custom serializer you should create a specialization of the `engine::components::Serialize::Serializer`
  * The script will try to find it, but if it won't you can include it in the component header
  * This is the example of a serializer:
  \code{.cpp}
namespace engine::components::Serialize
{
  template<class Ty>
  struct Serializer;
}
  \endcode
  \code{.cpp}
namespace engine::components::Serialize
{
    template<>
    struct Serializer<ComponentName>
    {
        static void Serialize(const ComponentName &component, std::ostream &stream)
        {
            // serialize each of the public members of the component
            // using the engine::components::Serialize::Serializer<T>{} specialization
        }
        static void Deserialize(ComponentName &component, std::istream &stream)
        {
            // deserialize each of the public members of the component
            // using the engine::components::Serialize::Serializer<T>{} specialization
        }
};
  \endcode
  * It will modify the engine\components\generated\components-serialization.hpp file

  @todo Add a python script that will generate the ImGui component editor code
  * This script should create the special configuration file located at `engine\components\generated\components-editor.json`.
  *
  * This file will contain the list of rules how to render each component.
  *
  * @warning If the member variable cannot be rendered/cast to the `ImGuiEditor<T>{}` specialization it will be written as an "unknown type" in the `.json` file
  * @note The user will be able to edit the `.json` file and add the rules for the `ImGuiEditor<T>{}` specializations
  * @note The user will be able to add the `ImGuiEditor<T>{}` specializations for the types that are not supported by default
  *
  * Example of `.json` configuration file:
  \code{.json}
{
    "ComponentName": {
        "member1": {
            "name": "member1 name", # name of the member that we should show
            "description": "member1 description",
            "settings": {
               "type": "slider", # type of the editor
               "min": 0.0, # min value for the slider
               "max": 1.0, # max value for the slider
               "step": 0.01, # step value for the slider
               "format": "%.2f", # format string for the slider
               "flags": "ImGuiSliderFlags_Logarithmic" # flags for the slider
            }
        },
        "member2": {	... },
        "member3": { "error": "UNKNOWN TYPE "},
    },
    "ComponentName2": { ... }
}
  \endcode
  * It will work similarly to the serialization/deserialization code
  * If you want to generate editor for the component you should add the
  * `static constexpr bool kGenerateEditor = true;` member to the component
  * This is the example of a component editor:
  \code{.cpp}
namespace engine::components::ImGuiEditor
{
  template<class Ty>
  struct ImGuiEditor;
}
  \endcode
  \code{.cpp}
namespace engine::components::ImGuiEditor
{
    template<>
    struct ImGuiEditor<ComponentName>
    {
        static void Draw(ComponentName &component)
        {
            // draw each of the public members of the component
            // using the engine::components::ImGuiEditor::ImGuiEditor<T>{} specialization
            // It will draw the component in the ImGui window
            // using the ImGui::InputText, ImGui::InputFloat, etc.
            // You can also use the ImGui::DragFloat, ImGui::SliderFloat, etc.
            // to make the component editor more user-friendly
        }
    };
}
  \endcode
  * It will modify the engine\components\generated\components-editor.hpp file
  @todo Add a python script that will generate the `std::hash` extension for each component

  * The generated code will look like this:
  \code{.cpp}
template<>
struct std::hash<engine::components::ComponentName>
{
    std::size_t operator()(const engine::components::ComponentName &component) const noexcept
    {
        std::size_t seed = 0;
        mal_toolkit::hash_combine (member1);
        mal_toolkit::hash_combine (member2);
        ...
        mal_toolkit::hash_combine (memberN);
        // !! each public member should have the std::hash<T>{} specialization
        return seed;
    }
};
  \endcode
  @note Each public member should have the `std::hash<T>{}` specialization to be able to use the hash_combine function
  *
  * If you want to generate hash class for the component you should add the `static constexpr bool kGenerateHash = true;` member to the component.
  *
  * It will modify the engine\components\generated\components-hash.hpp file
*/