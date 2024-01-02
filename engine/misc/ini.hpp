#pragma once
#include "mal-toolkit/mal-toolkit.hpp"
namespace lighten::ini
{
    /**
     * @brief Exception thrown when the syntax of the ini file is invalid
     *
     * @note This exception is derived from std::invalid_argument
     *
     */
    class InvalidSyntaxException : public std::invalid_argument
    {
    public:
        using std::invalid_argument::invalid_argument;
    };

    /**
     * @brief Exception thrown when the type of the entry is not convertible to the requested type
     *
     * @note This exception is derived from std::runtime_error
     *
     */
    class TypeConversionException : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };

    /**
     * @brief Exception thrown when the key is not found in the ini file
     *
     * @note This exception is derived from std::runtime_error
     *
     */
    class KeyErrorException : public std::runtime_error
    {
    public:
        using std::runtime_error::runtime_error;
    };
    /**
     * @brief Class representing an entry in the ini file
     *
     * @note This class is not supposed to be used directly by the user, but only through the Ini class
     *
     * @see Ini
     */
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
        /**
         * @brief Construct a new Entry object
         *
         * @param value The name of the value of the entry in the ini file
         * @param type The type of the entry, can be int64_t, long double, std::string or null
         *
         */
        explicit Entry(std::string_view const value, Type type) noexcept : type_(type), value_(value) {}

        /**
         * @brief Construct a new Entry object
         *
         * @note This constructor is used to create a null entry
         *
         * @param value The name of the value of the entry in the ini file
         *
         */

        Entry() = default;
        virtual ~Entry() = default;
        /**
         * @brief Assign an integer value to the entry
         *
         *
         * @tparam T Type of the integer. Must be std::integral
         * @param t The value to assign to the entry
         * @return Entry &
         */
        template <std::integral T>
        constexpr Entry &operator=(T t) noexcept
        {
            value_ = std::to_string(t);
            type_ = Type::kInt64;
            data_ = (int64_t)t;
            return *this;
        }
        /**
         * @brief Assign a floating point value to the entry
         *
         * @tparam T Type of the floating point. Must be std::floating_point
         * @param t The value to assign to the entry
         * @return Entry &
         */
        template <std::floating_point T>
        constexpr Entry &operator=(T t) noexcept
        {
            value_ = std::to_string(t);
            type_ = Type::kLongDouble;
            data_ = (long double)t;
            return *this;
        }
        /**
         * @brief Assign a string to the entry
         *
         * @tparam T Type of the string. Must be std::is_constructible_v<std::string, T>
         * @param t The value to assign to the entry
         * @return Entry &
         */
        template <typename T>
        constexpr Entry &operator=(T t)
            requires(std::is_constructible_v<std::string, T>)
        {
            if (mal_toolkit::trim(t) != t)
            {
                throw std::invalid_argument("The input string should be trimmed!");
            }
            value_ = std::string(t);
            type_ = Type::kString;
            data_ = std::monostate{};
            return *this;
        }
        /**
         * @brief Comparison operator for the entry
         *
         * @note This operator is used to compare the entry with an integer or long double.
         * If you'll try to call it with a string or null, it will throw a TypeConversionException
         *
         * @tparam T
         */
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
            else
            {
                throw TypeConversionException("This entry is null!");
            }
        }
        /**
         * @brief Comparison operator for the entry
         *
         * This operator overload is used to compare the entry with numerical value
         *
         * If the entry is not numerical, it will return false
         *
         * @tparam T
         */
        template <typename T>
        [[nodiscard]] constexpr bool operator==(T t) const
            requires(std::is_floating_point_v<T> || std::is_integral_v<T>)
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
        /**
         * @brief Comparison operator for the entry
         *
         * This operator overload is used to compare the entry with a string
         *
         * @tparam T
         */
        template <typename T>
        [[nodiscard]] constexpr bool operator==(T t) const
            requires(std::is_constructible_v<std::string, T>)
        {
            return value_ == t;
        }
        /**
         * @brief Comparison operator for the entry
         *
         * @param t other entry to compare with
         * @return true if the entries are equal
         */
        [[nodiscard]] bool operator==(Entry const &t) const noexcept { return type_ == t.type_ && value_ == t.value_; }

        /**
         * @brief Returns an underlying string value of the entry
         *
         * @return std::string const&
         */
        std::string const &to_string() const noexcept { return value_; }
        /**
         * @brief Returns an underlying string value of the entry
         *
         * @return std::string const&
         */

        [[nodiscard]] bool to_bool() const noexcept { return as_boolean(); }

        std::string const &str() const noexcept { return value_; }
        /**
         * @brief Returns an underlying string value of the entry
         *
         * @return true if integer value is not 0, false otherwise
         * If the entry is a string, and the string is equal to "yes", "true" or "on" (case insensitive), it will return true
         * @return false
         */
        [[nodiscard]] bool as_boolean() const noexcept
        {
            if (type_ == Type::kInt64 && to_int() != 0)
            {
                return true;
            }
            else if (type_ == Type::kString)
            {
                std::string temp = mal_toolkit::as_lowercase(value_);
                if (temp == "yes" || temp == "true" || temp == "on")
                {
                    return true;
                }
            }
            return false;
        }

        /**
         * @brief Returns an underlying integer value of the entry
         *
         * @note If the entry is not an integer, it will throw a TypeConversionException
         *
         * @return int64_t
         */
        [[nodiscard]] int64_t to_int() const
        {
            if (type_ != Type::kInt64)
            {
                throw TypeConversionException("This entry is not an integer!");
            }
            return std::get<int64_t>(data_);
        }
        /**
         * @brief Returns an underlying long double value of the entry
         *
         * @note If the entry is not a long double, it will throw a TypeConversionException
         *
         * @return long double
         */
        [[nodiscard]] long double to_double() const
        {
            if (type_ != Type::kLongDouble)
            {
                throw TypeConversionException("This entry is not a long double!");
            }
            return std::get<long double>(data_);
        }

    private:
        /**
         * @brief The type of an entry
         *
         */
        Type type_ = Type::kNull;
        /**
         * @brief The string value of the entry
         *
         */
        std::string value_ = "";
        /**
         * @brief The numerical value of the entry
         *
         */
        std::variant<std::monostate, int64_t, long double> data_;
    };

    /**
     * @brief The section of the ini file
     *
     * @note This class is not supposed to be used directly by the user, but only through the Ini class
     *
     * @see Ini
     *
     */
    class Section
    {
    public:
        friend class Ini;
        template <typename T>
        [[nodiscard]] T const &GetValue(std::string const &key) const;
        /**
         * @brief Returns a reference to the entry with the given key
         *
         * @note If the entry does not exist, it will be created
         * @note If the key is not trimmed (no spaces at the beginning and at the end), std::invalid_argument will be thrown
         *
         * @param key string key of the entry
         * @return Entry&
         */
        [[nodiscard]] Entry &operator[](std::string_view const key);
        /**
         * @brief Returns a reference to the entry with the given key
         *
         * @param key string key of the entry
         * @return Entry const&
         */
        [[nodiscard]] Entry const &at(std::string_view const key) const;
        /**
         * @brief Get the String object
         *
         * @note If the entry does not exist, it will throw an KeyErrorException
         * Always returns the string value, even of the object of integer or double type
         *
         * @note If the entry does not exist, it will throw an KeyErrorException
         *
         * @param key string key of the entry
         * @return std::string_view string value
         */
        [[nodiscard]] std::string_view GetString(std::string const &key);
        /**
         * @brief Get the Double object
         *
         * @note If the entry does not exist, it will throw an KeyErrorException
         *
         * @param key string key of the entry
         * @return long double double value
         */
        [[nodiscard]] long double GetDouble(std::string const &key);
        /**
         * @brief Get the Int object
         *
         * @note If the entry does not exist, it will throw an KeyErrorException
         *
         * @param key string key of the entry
         * @return int64_t integer value
         */
        [[nodiscard]] int64_t GetInt(std::string const &key);
        /**
         * @brief Get the Bool object
         *
         * @note If the entry does not exist, it will throw an KeyErrorException
         *
         * @param key string key of the entry
         * @return true if integer value is not 0, false otherwise
         * If the entry is a string, and the string is equal to "yes", "true" or "on" (case insensitive), it will return true
         * @return false
         */
        [[nodiscard]] bool GetBool(std::string const &key);
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