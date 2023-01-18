#pragma once
namespace engine::utils
{
    namespace _template_detail
    {
        template <typename A>
        constexpr size_t CalculateSize()
        {
            return sizeof(A);
        }
        template <typename A, typename B, typename... Args>
        constexpr size_t CalculateSize()
        {
            return sizeof(A) + CalculateSize<B, Args...>();
        }
        template <typename A>
        constexpr size_t CalculateAmount()
        {
            return 1;
        }
        template <typename A, typename B, typename... Args>
        constexpr size_t CalculateAmount()
        {
            return 1 + CalculateAmount<B, Args...>();
        }
    } // namespace _template_detail
    template <typename... Args>
    struct parameter_pack_info
    {
        static constexpr size_t size = _template_detail::CalculateSize<Args...>();
        static constexpr size_t amount = _template_detail::CalculateAmount<Args...>();
        template<size_t index>
        struct type_at
        {
            static_assert(index < size);
            using type = std::tuple_element_t<index, std::tuple<Args...>>;
        };
    };
    // based on https://artificial-mind.net/blog/2020/10/31/constexpr-for

    template <auto begin, auto end, auto inc, class F>
    constexpr void constexpr_for(F &&f)
    {
        if (not f(std::integral_constant<decltype(begin), begin>{}))
        {
            return;
        }
        if constexpr (begin + inc >= end)
        {
            return;
        }
        else // else should be constexpr as well
        {
            constexpr_for<begin + inc, end, inc>(std::forward<F>(f));
        }
    }

    template <class iterator, class F>
    constexpr bool for_each_true(iterator begin, iterator const &end, F &&f)
    {
        bool rv = true;
        for (; begin != end;)
        {
            rv &= f(begin++);
        }
        return rv;
    }

    template <typename T>
    struct return_type;

    template <typename R, typename... Args>
    struct return_type<R(Args...)>
    {
        using type = R;
    };

    template <typename R, typename... Args>
    struct return_type<R(*)(Args...)>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R(C:: *)(Args...)>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R(C:: *)(Args...) &>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R(C:: *)(Args...) &&>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R(C:: *)(Args...) const>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R(C:: *)(Args...) const &>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R(C:: *)(Args...) const &&>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R(C:: *)(Args...) volatile>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R(C:: *)(Args...) volatile &>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R(C:: *)(Args...) volatile &&>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R(C:: *)(Args...) const volatile>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R(C:: *)(Args...) const volatile &>
    {
        using type = R;
    };

    template <typename R, typename C, typename... Args>
    struct return_type<R(C:: *)(Args...) const volatile &&>
    {
        using type = R;
    };

    template <typename T>
    using return_type_t = typename return_type<T>::type;
}