#pragma once
#include "include/d3d.hpp"
#include "include/library-pch.hpp"

namespace engine::direct3d
{
    template <typename T>
    class d3d_resource_wrapper
    {
    public:
        using type = T;
        constexpr operator T const *() const noexcept { return ptr_; }
        constexpr operator T *() noexcept { return ptr_; }
        constexpr T const *operator->() const noexcept { return ptr_; }
        constexpr T *operator->() noexcept { return ptr_; }

        [[nodiscard]] constexpr T const *const &ptr() const noexcept { return ptr_; }
        [[nodiscard]] constexpr T *&ptr() noexcept { return ptr_; }

        [[nodiscard]] constexpr bool valid() { return ptr_ != nullptr; }

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

        constexpr d3d_resource_wrapper<T> &operator=(T *ptr)
        {
            this->reset(ptr);
            if (ptr)
            {
                ptr->Release();
            }
            return *this;
        }

        ~d3d_resource_wrapper()
        {
            reset();
        }

        d3d_resource_wrapper(d3d_resource_wrapper<T> &&other) noexcept
        {
            reset(other.ptr_);
        }
        d3d_resource_wrapper(d3d_resource_wrapper<T> const &other) noexcept
        {
            reset(other.ptr_);
        }
        d3d_resource_wrapper &operator=(d3d_resource_wrapper<T> &&other) noexcept
        {
            reset(other.ptr_);
            return *this;
        }
        d3d_resource_wrapper &operator=(d3d_resource_wrapper<T> const &other) noexcept
        {
            reset(other.ptr_);
            return *this;
        }

        constexpr d3d_resource_wrapper() = default;
        constexpr d3d_resource_wrapper(T *ptr)
        {
            reset(ptr);
            if (ptr)
            {
                ptr->Release();
            }
        }

    private:
        T *ptr_ = nullptr;

    public:
        template <class... Types>
        constexpr auto operator()(Types &&...args) const
            noexcept(noexcept(std::invoke(*ptr_, static_cast<Types &&>(args)...)))
            -> decltype(std::invoke(*ptr_, static_cast<Types &&>(args)...))
        {
            return std::invoke(*ptr_, static_cast<Types &&>(args)...);
        }
    };

    template <typename T>
    class readonly_d3d_resource_wrapper : protected d3d_resource_wrapper<T>
    {
    public:
        constexpr operator T const *() const noexcept { return d3d_resource_wrapper<T>::ptr(); }
        constexpr operator T *() noexcept { return d3d_resource_wrapper<T>::ptr(); }
        constexpr T const *operator->() const noexcept { return d3d_resource_wrapper<T>::ptr(); }
        constexpr T *operator->() noexcept { return d3d_resource_wrapper<T>::ptr(); }
        constexpr operator d3d_resource_wrapper<T>() { return d3d_resource_wrapper<T>() = *this; }
        constexpr d3d_resource_wrapper<T> as_default_wrapper() { return d3d_resource_wrapper<T>() = *this; }
        [[nodiscard]] constexpr T *const &ptr() noexcept { return d3d_resource_wrapper<T>::ptr(); }
        [[nodiscard]] constexpr T const *const &ptr() const noexcept { return d3d_resource_wrapper<T>::ptr(); }

        using d3d_resource_wrapper<T>::valid;
        using d3d_resource_wrapper<T>::operator();
    protected:
        using d3d_resource_wrapper<T>::operator=;
        using d3d_resource_wrapper<T>::d3d_resource_wrapper;
    };
} // namespace engine::direct3d

#include "typedefs.hpp"