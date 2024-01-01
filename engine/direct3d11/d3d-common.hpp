#pragma once
#include "include/d3d.hpp"
#include "include/library-pch.hpp"

namespace engine::direct3d
{
    /// @brief The d3d resource wrapper class
    /// @tparam T The type of the resource
    template <typename T>
    class d3d_resource_wrapper
    {
    public:
        using type = T;
        /// @brief convert this to T const *
        constexpr operator T const *() const noexcept { return ptr_; }
        /// @brief convert this to T *
        constexpr operator T *() noexcept { return ptr_; }
        /// @brief const arrow operator to the underlying T const *
        /// @return T const *
        constexpr T const *operator->() const noexcept { return ptr_; }
        /// @brief arrow operator to the underlying T *
        /// @return T *
        constexpr T *operator->() noexcept { return ptr_; }
        /// @brief Returns const & to the underlying T const *
        /// @return T const &
        [[nodiscard]] constexpr T const *const &ptr() const noexcept { return ptr_; }
        /// @brief Returns & to the underlying T *
        /// @return T &
        [[nodiscard]] constexpr T *&ptr() noexcept { return ptr_; }
        /// @brief Returns boolean value if the underlying data is valid
        /// @return true if ptr != nullptr
        /// @return false if ptr == nullptr
        [[nodiscard]] constexpr bool valid() { return ptr_ != nullptr; }

        /// @brief Resets the underlying pointer and releases the resource
        inline T *&reset(T *ptr = nullptr)
        {
            if (ptr_)
            {
                ptr_->Release();
            }
            ptr_ = ptr;
            if (ptr_)
            {
                ptr_->AddRef();
            }
            return ptr_;
        }
        /// @brief Assign operator, resets and releases underlying data and assigns the provided pointer
        /// @note This method will not release the provided pointer, it takes control of it.
        /// @note It won't `AddRef()` it.
        constexpr d3d_resource_wrapper<T> &operator=(T *ptr)
        {
            this->reset(ptr);
            if (ptr)
            {
                ptr->Release();
            }
            return *this;
        }
        /// @brief Destructor. Releases the underlying resource if it is valid
        ~d3d_resource_wrapper()
        {
            reset();
        }
        /// @brief Move constructor
        /// @param other The other d3d_resource_wrapper to move from
        d3d_resource_wrapper(d3d_resource_wrapper<T> &&other) noexcept
        {
            reset(other.ptr_);
        }
        /// @brief Copy constructor
        /// @param other The other d3d_resource_wrapper to copy from
        d3d_resource_wrapper(d3d_resource_wrapper<T> const &other) noexcept
        {
            reset(other.ptr_);
        }
        /// @brief Move assignment operator
        /// @param other The other d3d_resource_wrapper to move from
        d3d_resource_wrapper &operator=(d3d_resource_wrapper<T> &&other) noexcept
        {
            reset(other.ptr_);
            return *this;
        }
        /// @brief Copy assignment operator
        /// @param other The other d3d_resource_wrapper to copy from
        d3d_resource_wrapper &operator=(d3d_resource_wrapper<T> const &other) noexcept
        {
            reset(other.ptr_);
            return *this;
        }
        /// @brief Default constructor
        /// Sets the underlying pointer to nullptr
        constexpr d3d_resource_wrapper() = default;
        /// @brief Constructor
        /// @param ptr The pointer to take control of
        /// @note This method will not release the provided pointer, it takes control of it.
        /// @note It won't `AddRef()` it.
        constexpr d3d_resource_wrapper(T *ptr)
        {
            reset(ptr);
            if (ptr)
            {
                ptr->Release();
            }
        }

    private:
        /// @brief The underlying pointer
        T *ptr_ = nullptr;

    public:
        /// @brief The invoke operator
        /// @details This is proxy to the pointer's member functions
        /// @tparam ...Types
        /// @param ...args
        /// @return
        template <class... Types>
        constexpr auto operator()(Types &&...args) const
            noexcept(noexcept(std::invoke(*ptr_, static_cast<Types &&>(args)...)))
                -> decltype(std::invoke(*ptr_, static_cast<Types &&>(args)...))
        {
            return std::invoke(*ptr_, static_cast<Types &&>(args)...);
        }
    };

    /// @brief The readonly d3d resource wrapper class
    /// @tparam T The type of the resource
    template <typename T>
    class readonly_d3d_resource_wrapper : protected d3d_resource_wrapper<T>
    {
    public:
        /// @brief convert this to T const *
        constexpr operator T const *() const noexcept { return d3d_resource_wrapper<T>::ptr(); }
        /// @brief convert this to T *
        constexpr operator T *() noexcept { return d3d_resource_wrapper<T>::ptr(); }
        /// @brief const arrow operator to the underlying T const *
        /// @return T const *
        constexpr T const *operator->() const noexcept { return d3d_resource_wrapper<T>::ptr(); }
        /// @brief arrow operator to the underlying T *
        /// @return T *
        constexpr T *operator->() noexcept { return d3d_resource_wrapper<T>::ptr(); }
        /// @brief converts readonly wrapper to default wrapper
        constexpr operator d3d_resource_wrapper<T>() { return d3d_resource_wrapper<T>() = *this; }
        /// @brief converts readonly wrapper to default wrapper
        /// @return d3d_resource_wrapper<T>
        constexpr d3d_resource_wrapper<T> as_default_wrapper() { return d3d_resource_wrapper<T>() = *this; }
        /// @brief Returns const & to the underlying T *
        [[nodiscard]] constexpr T *const &ptr() noexcept { return d3d_resource_wrapper<T>::ptr(); }
        /// @brief Returns const & to the underlying T const *
        [[nodiscard]] constexpr T const *const &ptr() const noexcept { return d3d_resource_wrapper<T>::ptr(); }

        using d3d_resource_wrapper<T>::valid;
        using d3d_resource_wrapper<T>::operator();

    protected:
        using d3d_resource_wrapper<T>::operator=;
        using d3d_resource_wrapper<T>::d3d_resource_wrapper;
    };
} // namespace engine::direct3d

#include "typedefs.hpp"