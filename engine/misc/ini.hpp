#pragma once
#include "utils/utils.hpp"
namespace engine::ini
{
    class InvalidSyntaxException : public std::invalid_argument
    {
    public:
        using std::invalid_argument::invalid_argument;
    };
    class TypeConversionException : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
    class KeyErrorException : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
    class Entry
    {
    public:
        enum class Type
        {
            kInt64,
            kLongDouble,
            kString,
            kNull
        };
        std::string const &to_string() const noexcept { return value_; }
        std::string const &str() const noexcept { return value_; }
        [[nodiscard]] bool as_boolean() const noexcept
        {
            if (type_ == Type::kInt64 && to_int() != 0)
            {
                return true;
            }
            else if (type_ == Type::kString)
            {
                std::string temp = utils::as_lowercase(value_);
                if (temp == "yes" || temp == "true" || temp == "on")
                {
                    return true;
                }
            }
            return false;
        }

        [[nodiscard]] int64_t to_int() const
        {
            if (type_ != Type::kInt64)
            {
                throw TypeConversionException("This entry is not an integer!");
            }
            return std::get<int64_t>(data_);
        }
        [[nodiscard]] long double to_double() const
        {
            if (type_ != Type::kLongDouble)
            {
                throw TypeConversionException("This entry is not a long double!");
            }
            return std::get<long double>(data_);
        }
        explicit Entry(std::string_view const value, Type type) noexcept : type_(type), value_(value) {}

        Entry() = default;
        virtual ~Entry() = default;

        template <std::integral T>
        constexpr Entry &operator=(T t) noexcept
        {
            value_ = std::to_string(t);
            type_ = Type::kInt64;
            data_ = (int64_t)t;
            return *this;
        }
        template <std::floating_point T>
        constexpr Entry &operator=(T t) noexcept
        {
            value_ = std::to_string(t);
            type_ = Type::kLongDouble;
            data_ = (long double)t;
            return *this;
        }
        template <typename T>
        constexpr Entry &operator=(T t) requires(std::is_constructible_v<std::string, T>)
        {
            if (utils::trim(t) != t)
            {
                throw std::invalid_argument("The input string should be trimmed!");
            }
            value_ = std::string(t);
            type_ = Type::kString;
            data_ = std::monostate {};
            return *this;
        }
        template <typename T>
        [[nodiscard]] constexpr auto operator<=>(T t) const
        {
            if (type_ == Type::kString)
            {
                throw TypeConversionException("This entry is a string!");
            }
            if (type_ == Type::kInt64)
            {
                return t <=> std::get<int64_t>(data_);
            }
            else if (type_ == Type::kLongDouble)
            {
                return t <=> std::get<long double>(data_);
            }
        }
        template <typename T>
        [[nodiscard]] constexpr bool operator==(T t) const requires(std::is_floating_point_v<T> || std::is_integral_v<T>)
        {
            if (type_ == Type::kInt64)
            {
                return std::get<int64_t>(data_) == t;
            }
            else if (type_ == Type::kLongDouble)
            {
                return std::get<long double>(data_) == t;
            }
            return false;
        }
        template <typename T>
        [[nodiscard]] constexpr bool operator==(T t) const requires(std::is_constructible_v<std::string, T>) { return value_ == t; }
        [[nodiscard]] bool operator==(Entry const &t) const noexcept { return type_ == t.type_ && value_ == t.value_; }

    private:
        Type type_ = Type::kNull;
        std::string value_ = "";
        std::variant<std::monostate, int64_t, long double> data_;
    };
    class Section
    {
    public:
        friend class Ini;
        template <typename T>
        [[nodiscard]] T const &GetValue(std::string const &key) const;
        [[nodiscard]] Entry &operator[](std::string_view const key);
        [[nodiscard]] Entry const &at(std::string_view const key) const;
        // Always returns the string value, even of the object of integer or double type
        [[nodiscard]] std::string_view GetString(std::string const &key);
        [[nodiscard]] long double GetDouble(std::string const &key);
        [[nodiscard]] int64_t GetInt(std::string const &key);
        template <typename T>
        void SetValue(std::string const &key, T value) noexcept { (*this)[key] = value; }
        [[nodiscard]] std::string Serialize() const noexcept;
        [[nodiscard]] auto begin() noexcept { return dict_.begin(); }
        [[nodiscard]] auto end() noexcept { return dict_.end(); }
        [[nodiscard]] auto begin() const noexcept { return dict_.begin(); }
        [[nodiscard]] auto end() const noexcept { return dict_.end(); }
        [[nodiscard]] auto cbegin() const noexcept { return dict_.cbegin(); }
        [[nodiscard]] auto cend() const noexcept { return dict_.cend(); }
        [[nodiscard]] bool EntryExists(std::string_view const key) const noexcept { return Contains(key); }
        [[nodiscard]] bool Contains(std::string_view const key) const noexcept { return dict_.contains(std::string(key)); }
        [[nodiscard]] size_t size() const noexcept { return dict_.size(); }

    protected:
        Section() = default;

    private:
        std::map<std::string, Entry, std::less<>> dict_;
    };
    class Ini
    {
    public:
        Ini() = default;
        explicit Ini(std::string_view const str);

        [[nodiscard]] Section &operator[](std::string_view key);

        Section &CreateSection(std::string const &key);

        [[nodiscard]] std::string Serialize() const noexcept;
        [[nodiscard]] static Ini Deserialize(std::string_view const data);

        [[nodiscard]] bool SectionExists(std::string_view const key) const noexcept { return Contains(key); }
        [[nodiscard]] bool Contains(std::string_view const key) const noexcept { return dict_.contains(std::string(key)); }

        [[nodiscard]] size_t size() const noexcept { return dict_.size(); }

        [[nodiscard]] auto begin() noexcept { return dict_.begin(); }
        [[nodiscard]] auto end() noexcept { return dict_.end(); }
        [[nodiscard]] auto begin() const noexcept { return dict_.begin(); }
        [[nodiscard]] auto end() const noexcept { return dict_.end(); }
        [[nodiscard]] auto cbegin() const noexcept { return dict_.begin(); }
        [[nodiscard]] auto cend() const noexcept { return dict_.end(); }

    private:
        inline void DeserializeLine(std::string const &section, std::string &line);
        std::map<std::string, Section, std::less<>> dict_;
    };
} // namespace ini