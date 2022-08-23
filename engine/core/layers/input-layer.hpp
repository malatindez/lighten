#pragma once
#include "layer.hpp"
#include "utils/utils.hpp"
namespace engine::core
{
    namespace Key
    {
        enum Key : int16_t
        {
            KEY_UNKNOWN = -1,
            KEY_LBUTTON = 1,
            KEY_RBUTTON = 2,
            KEY_CANCEL = 3,
            KEY_MBUTTON = 4,
            KEY_BACK = 8,
            KEY_TAB = 9,
            KEY_CLEAR = 12,
            KEY_RETURN = 13,
            KEY_SHIFT = 16,
            KEY_CONTROL = 17,
            KEY_MENU = 18,
            KEY_PAUSE = 19,
            KEY_CAPITAL = 20,
            KEY_KANA = 21,
            KEY_HANGEUL = 21,
            KEY_HANGUL = 21,
            KEY_IME_ON = 22,
            KEY_JUNJA = 23,
            KEY_FINAL = 24,
            KEY_HANJA = 25,
            KEY_KANJI = 25,
            KEY_IME_OFF = 26,
            KEY_ESCAPE = 27,
            KEY_CONVERT = 28,
            KEY_NONCONVERT = 29,
            KEY_ACCEPT = 30,
            KEY_MODECHANGE = 31,
            KEY_SPACE = 32,
            KEY_PRIOR = 33,
            KEY_NEXT = 34,
            KEY_END = 35,
            KEY_HOME = 36,
            KEY_LEFT = 37,
            KEY_UP = 38,
            KEY_RIGHT = 39,
            KEY_DOWN = 40,
            KEY_SELECT = 41,
            KEY_PRINT = 42,
            KEY_EXEC = 43,
            KEY_SNAPSHOT = 44,
            KEY_INSERT = 45,
            KEY_DELETE = 46,
            KEY_HELP = 47,
            KEY_0 = 48,
            KEY_1 = 49,
            KEY_2 = 50,
            KEY_3 = 51,
            KEY_4 = 52,
            KEY_5 = 53,
            KEY_6 = 54,
            KEY_7 = 55,
            KEY_8 = 56,
            KEY_9 = 57,
            KEY_A = 65,
            KEY_B = 66,
            KEY_C = 67,
            KEY_D = 68,
            KEY_E = 69,
            KEY_F = 70,
            KEY_G = 71,
            KEY_H = 72,
            KEY_I = 73,
            KEY_J = 74,
            KEY_K = 75,
            KEY_L = 76,
            KEY_M = 77,
            KEY_N = 78,
            KEY_O = 79,
            KEY_P = 80,
            KEY_Q = 81,
            KEY_R = 82,
            KEY_S = 83,
            KEY_T = 84,
            KEY_U = 85,
            KEY_V = 86,
            KEY_W = 87,
            KEY_X = 88,
            KEY_Y = 89,
            KEY_Z = 90,
            KEY_NUMPAD0 = 96,
            KEY_NUMPAD1 = 97,
            KEY_NUMPAD2 = 98,
            KEY_NUMPAD3 = 99,
            KEY_NUMPAD4 = 100,
            KEY_NUMPAD5 = 101,
            KEY_NUMPAD6 = 102,
            KEY_NUMPAD7 = 103,
            KEY_NUMPAD8 = 104,
            KEY_NUMPAD9 = 105,
            KEY_NUMPAD_MULTIPLY = 106,
            KEY_NUMPAD_PLUS = 107,
            KEY_NUMPAD_SEPARATOR = 108,
            KEY_NUMPAD_MINUS = 109,
            KEY_NUMPAD_DECIMAL = 110,
            KEY_NUMPAD_DIVIDE = 111,
            KEY_F1 = 112,
            KEY_F2 = 113,
            KEY_F3 = 114,
            KEY_F4 = 115,
            KEY_F5 = 116,
            KEY_F6 = 117,
            KEY_F7 = 118,
            KEY_F8 = 119,
            KEY_F9 = 120,
            KEY_F10 = 121,
            KEY_F11 = 122,
            KEY_F12 = 123,
            KEY_F13 = 124,
            KEY_F14 = 125,
            KEY_F15 = 126,
            KEY_F16 = 127,
            KEY_F17 = 128,
            KEY_F18 = 129,
            KEY_F19 = 130,
            KEY_F20 = 131,
            KEY_F21 = 132,
            KEY_F22 = 133,
            KEY_F23 = 134,
            KEY_F24 = 135,
            KEY_LSHIFT = 160,
            KEY_RSHIFT = 161,
            KEY_LCONTROL = 162,
            KEY_RCONTROL = 163,
            KEY_LMENU = 164,
            KEY_RMENU = 165,
            KEY_OEM_1 = 186,  // ';:' for US
            KEY_PLUS = 187,   // '+' any country
            KEY_COMMA = 188,  // ',' any country
            KEY_MINUS = 189,  // '-' any country
            KEY_PERIOD = 190, // '.' any country
            KEY_OEM_2 = 191,  // '/?' for US
            KEY_OEM_3 = 192,  // '`~' for US
            KEY_OEM_4 = 219,  //  '[{' for US
            KEY_OEM_5 = 220,  //  '\|' for US
            KEY_OEM_6 = 221,  //  ']}' for US
            KEY_OEM_7 = 222,  //  ''"' for US
            KEY_OEM_8 = 223,
        };
    }
    class Engine;
    class InputLayer
        : public Layer,
        public Layer::HandleEvent,
        public Layer::HandleTick,
        public Layer::HandleUpdate
    {
    public:
        virtual ~InputLayer() = default;

        using KeySeq = std::vector<uint32_t>;
        // this function is being called each tick and whenever the KeySeq is being held
        // count is equal to amount of ticks that the sequence was held (starts at 1)
        // if count is equal to UINT32_MAX uint32_t(-1) then the KeySeq was released
        // float delta_time, key sequence, count
        using OnTickKeyCallbackFn = std::function<void(float, KeySeq const &, uint32_t)>;
        // this function is being called each update and whenever the KeySeq is being held
        // count is equal to the amount of calls of that function (starts at 1)
        // if count is equal to UINT32_MAX uint32_t(-1) then the KeySeq was released
        // key sequence, count
        using OnUpdateKeyCallbackFn = std::function<void(KeySeq const &, uint32_t)>;

        void OnTick(float dt) override;
        void OnUpdate() override;
        void OnEvent(events::Event &event) override;

        void flush() noexcept
        {
            mouse_scrolled_ = false;
            scroll_delta_ = 0;
        }

        // if on_hold is true then the function will be called each tick/update that the KeySeq is being held
        void AddUpdateKeyCallback(KeySeq const &sequence, OnUpdateKeyCallbackFn const &fn, bool on_hold = true);
        // if on_hold is true then the function will be called each tick/update that the KeySeq is being held
        void AddTickKeyCallback(KeySeq const &sequence, OnTickKeyCallbackFn const &fn, bool on_hold = true);
        void ResetUpdateKeyCallback(KeySeq const &sequence) { on_update_callbacks_.erase(on_update_callbacks_.find(sequence)); }
        void ResetTickKeyCallback(KeySeq const &sequence) { on_tick_callbacks_.erase(on_tick_callbacks_.find(sequence)); }

        [[nodiscard]] inline bool key_state(uint32_t key) const noexcept
        {
            if (!key_states_.contains(key))
            {
                return false;
            }
            return key_states_.at(key);
        }
        [[nodiscard]] inline bool lbutton_down() const noexcept { return key_state(Key::KEY_LBUTTON); }
        [[nodiscard]] inline bool rbutton_down() const noexcept { return key_state(Key::KEY_RBUTTON); }
        [[nodiscard]] constexpr math::ivec2 const &mouse_position() const noexcept { return mouse_position_; }
        [[nodiscard]] constexpr bool mouse_scrolled() const noexcept { return mouse_scrolled_; }
        [[nodiscard]] constexpr int16_t scroll_delta() const noexcept { return scroll_delta_; }

        static std::shared_ptr<InputLayer> instance() { return instance_; }
        void OnAttach() override
        {
            utils::Assert(!attached_);
            attached_ = true;
        }
        void OnDetach() override
        {
            utils::Assert(attached_);
            attached_ = false;
        }

    private:
        friend class ::engine::core::Engine;

        static void Init()
        {
            utils::Assert(instance_ == nullptr);
            instance_ = std::shared_ptr<InputLayer>(new InputLayer());
        }
        static void Deinit() { instance_ = nullptr; }

    private:
        bool attached_ = false;
        InputLayer() = default;
        InputLayer(InputLayer &&) = delete;
        InputLayer(InputLayer const &) = delete;
        InputLayer &operator=(InputLayer &&) = delete;
        InputLayer &operator=(InputLayer const &) = delete;

    private:
        static std::shared_ptr<InputLayer> instance_;

    private:
        std::unordered_map<uint32_t, bool> key_states_;
        bool mouse_scrolled_ = false;
        int16_t scroll_delta_ = 0;
        math::ivec2 mouse_position_ { 0 };
        std::map<KeySeq, std::tuple<bool, uint32_t, OnTickKeyCallbackFn>> on_tick_callbacks_;
        std::map<KeySeq, std::tuple<bool, uint32_t, OnUpdateKeyCallbackFn>> on_update_callbacks_;
        // TODO(add binds by name + description so we can bind them in the console)
        std::map<std::string, std::tuple<KeySeq, bool, OnTickKeyCallbackFn>> on_tick_;
        std::map<std::string, std::tuple<KeySeq, bool, OnTickKeyCallbackFn>> on_update_;
        std::map<std::string, std::string> desriptions;
    };
}