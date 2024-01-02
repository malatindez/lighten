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
    if prop_type == "float":
        return f"ImGui::InputFloat(\"{prop_data['settings']['name']}\", &t.{prop_name});"
    elif prop_type == "bool":
        return f"ImGui::Checkbox(\"{prop_data['settings']['name']}\", &t.{prop_name});"
    elif prop_type == "int":
        return f"ImGui::InputInt(\"{prop_data['settings']['name']}\", &t.{prop_name});"
    elif prop_type == "unsinged int":
        return f"ImGui::InputScalar(\"{prop_data['settings']['name']}\", ImGuiDataType_U32, &t.{prop_name});"
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

def update_yaml_files(data, output_dir):
    index = {}
    index["ui_files"] = []
    for component_name, component_data in data.items():
        yaml_data = {}

        yaml_data["name"] = component_data["settings"]["name"]
        yaml_data["category"] = component_data["settings"]["category"]
        # Generate YAML file path for each component
        file_name = component_data['namespace'].replace("::", "_") + "_" + component_name + ".yaml"
        file_path = os.path.join(output_dir, "gui/yaml", file_name)
        index["ui_files"].append(file_path)
        
        # Read existing YAML file if it exists
        if os.path.exists(file_path):
            try:
                with open(file_path, 'r') as file:
                    existing_data = yaml.safe_load(file) or {}
            except:
                existing_data = {}
        else:
            existing_data = {}
        
        for category in ["properties", "getters", "setters", "methods"]:
            if category in existing_data:
                if existing_data[category] is None:
                    existing_data[category] = {}
                items_to_delete = [item_name for item_name in existing_data[category].keys()
                                if item_name not in component_data.get(category, {})]
                for item_name in items_to_delete:
                    del existing_data[category][item_name]

        for category in ["properties", "getters", "setters", "methods"]:
            yaml_data[category] = {}
            for item_name, item_data in component_data.get(category, {}).items():
                # Check if name and type of properties match, if not, overwrite completely
                if category == "properties" and (existing_data.get(category, {}).get(item_name, {}).get('type', None) == item_data.get('type')):
                    yaml_data[category][item_name] = existing_data[category][item_name]
                elif category != "properties" and (existing_data.get(category, {}).get(item_name, {}).get('return_type', None) == item_data.get('return_type')):
                    yaml_data[category][item_name] = existing_data[category][item_name]
                else:
                    yaml_data[category][item_name] = item_data["settings"]
                    if category == "properties":
                        type_hint = item_data["type"]
                        yaml_data[category][item_name]['type'] = type_hint
                    else:
                        type_hint = item_data["return_type"]    
                        yaml_data[category][item_name]['return_type'] = type_hint
        # Merge existing data with new data
        merged_data = {**existing_data, **yaml_data}

        # Write updated data back to YAML file
        os.makedirs(os.path.dirname(file_path), exist_ok=True)
        with open(file_path, 'w') as file:
            yaml_content = yaml.dump(merged_data, sort_keys=False)
            file.write(yaml_content)
    index["count"] = len(index["ui_files"])
    # Delete YAML files that are not used anymore
    for file_path in os.listdir(os.path.join(output_dir, "gui/yaml")):
        if os.path.join(output_dir, "gui/yaml", file_path) not in index["ui_files"]:
            os.remove(os.path.join(output_dir, "gui/yaml", file_path))
        
    with open(os.path.join(output_dir, "gui/yaml/index.yaml"), 'w') as file:
        yaml_content = yaml.dump(index, sort_keys=False)
        file.write(yaml_content)
    print(f"YAML configuration files updated in '{output_dir}'.")