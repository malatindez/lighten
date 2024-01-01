#pragma once
#define LIGHTEN_REFLECTION_DISCARD_FILE_FROM_UNITY_BUILD

// classes
/**
 * @brief Declare a class that can be serialized and shown on GUI. It cannot be used as a component.
 */
// Input: name, category
#define LIGHTEN_DATACLASS(...)
/**
 * @brief Declare a class that is used as a component.
 */
// Input: name, category, editor hide, editor readonly, save game
#define LIGHTEN_COMPONENT(...)
/**
 * @brief Declare a system which processes component(or components)
 * @details This macro is used to declare a system class which has methods
 */
// Input: name, category
#define LIGHTEN_SYSTEM(...)
// Input: name, category, ctx_menu, button, execute_in_editor, mutator
#define LIGHTEN_METHOD(...)
// Input: name, category, serialize, editor_hide, editor_readonly, save_game
#define LIGHTEN_PROPERTY(...)
// Input: name, category, serialize, editor_hide, editor_readonly, save_game, mutator
#define LIGHTEN_PROPERTY_GETTER(...)
// Input: name, category, mutator
#define LIGHTEN_PROPERTY_SETTER(...)

/* ideas:
// Maximum amount of categories: 64
#define LIGHTEN_DECLARE_EVENT_CATEGORY(categoryname)
// Declare an event which can be processed by systems.
// Syntax:
// LIGHTEN_EVENT_HANDLER(EventCategory, EventName)
// void OnEventName(EventName const &event);
#define LIGHTEN_DECLARE_EVENT(CategoryFlags, EventName, body, data_members)
// Declare an event which can be processed by systems.
// Syntax:
// LIGHTEN_EVENT_HANDLER(EventCategory, EventName)
// void OnEventName(type1 const &data1, type2 const &data2, ...);
#define LIGHTEN_DECLARE_EVENT_NO_BODY(CategoryFlags, EventName, data_members)


// For gameobjects
#define LIGHTEN_REQUIRE_GAMEOBJECT(typename)
#define LIGHTEN_DISALLOW_GAMEOBJECT(typename)
#define LIGHTEN_ALLOW_SINGLE_GAMEOBJECT(typename)

// for system methods

// Syntax:
// R|component, RW|component, W|component, PW|component, ...
// R - read-only access, allows parallelization for this component
// RW - read-write access, atomic access to the component
// W - read-write access, atomic access to the component
// PW - read-write access, allows parallelization for this component
// PRW - read-write access, allows parallelization for this component
// In case of PW or PRW, the system should correctly handle parallelization itself.
#define LIGHTEN_SYSTEM_GROUP(access_typename, ...)

// Event processor:
// LIGHTEN_EVENT_HANDLER(EventCategory, EventName)
// void OnEventName(EventName const &event);
// Or, for events without body:
// LIGHTEN_DECLARE_EVENT_NO_BODY(EventCategory, EventName)
// void OnEventName(type1 const &data1, type2 const &data2, ...);
#define LIGHTEN_EVENT_HANDLER(EventCategory, EventName, Priority)


// Method should be executed only on server
#define LIGHTEN_ONLY_SERVER
// Method should be executed only on client
#define LIGHTEN_ONLY_CLIENT

*/
