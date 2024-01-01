import os
import os.path
import yaml


def generate_cpp_serialization_code(data):
    serialization_code = ""
    includes = []
    for component_name, component_data in data.items():
        includes.append(f"#include \"{component_data['file']}\"")
    includes = list(set(includes))
    serialization_code += "\n".join(includes) + "\n\n"

    for component_name, component_data in data.items():
        serialization_code += f"namespace cereal {{\n"
        serialization_code += f"    template<class Archive>\n"
        serialization_code += f"    void serialize(Archive& archive, ::{component_data['namespace']}::{component_name} & t) {{\n"
        for prop_name, prop_data in component_data.get("properties", {}).items():
            serialization_code += f"       archive(cereal::make_nvp(\"{prop_name}\", t.{prop_name}));\n"
        serialization_code += "    }\n"
        serialization_code += "}\n\n"

    return serialization_code

def type_to_imgui_function(prop_data):
    prop_type = prop_data["type"]
    prop_name = prop_data["name"]
    # This function maps C++ types to ImGUI functions
    if prop_type.startswith("float"):
        return f"ImGui::InputFloat(\"{prop_data['settings']['name']}\", &t.{prop_name});"
    elif prop_type == "bool":
        return f"ImGui::Checkbox(\"{prop_data['settings']['name']}\", &t.{prop_name});"
    elif prop_type.startswith("int"):
        return f"ImGui::InputInt(\"{prop_data['settings']['name']}\", &t.{prop_name});"
    else:
        return f"Draw<{prop_type}>(t.{prop_name}, \"{prop_data['settings']['name']}\");"

def generate_imgui_code(data):
    gui_code = "namespace lighten::gui {\n"

    for component_name, component_data in data.items():
        settings = component_data["settings"]
        gui_code += f"    template<>\n"
        gui_code += f"    void Draw({component_data['namespace']}::{component_name} const &t, std::string_view const name = \"{settings['name']}\") {{\n"
        gui_code += f"        if (ImGui::TreeNode(name.data())) {{\n"
        for prop_name, prop_data in component_data.get("properties", {}).items():
            gui_code += "            " + type_to_imgui_function(prop_data) + '\n'

        gui_code += "            ImGui::TreePop();\n"
        gui_code += "        }\n"
        gui_code += "    }\n"
    gui_code += "}\n"

    return gui_code

def extract_visual_attributes(item):
    # Extract only the attributes relevant for visual appearance
    keys = ['name', 'category', 'editor_hide', 'editor_readonly', 'save_game', 'ctx_menu', 'button', 'execute_in_editor', 'mutator', 'serialize']
    return {k: v for k, v in item.items() if k in keys}

def generate_yaml_files(data, output_dir):
    for component_name, component_data in data.items():
        yaml_data = {}

        # Process properties, getters, setters, and methods for visual configuration
        for category in ["properties", "getters", "setters", "methods"]:
            yaml_data[category] = {}
            for item_name, item_data in component_data.get(category, {}).items():
                yaml_data[category][item_name] = extract_visual_attributes(item_data["settings"])

        # Generate YAML file for each component
        file_name = component_data['namespace'].replace("::", "_") + "_" + component_name + ".yaml"
        file_path = os.path.join(output_dir, file_name)
        with open(file_path, 'w') as file:
            yaml.dump(yaml_data, file, sort_keys=False)

    print(f"YAML configuration files generated in '{output_dir}'.")