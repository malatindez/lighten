#pragma once
#include "api.hpp"
#include "mal-toolkit/debug.hpp"
#include "mal-toolkit/win-utils.hpp"
#include <system_error>
namespace lighten::direct3d
{
    namespace _detail
    {
        /// @brief Get the last error info as std::string
        /// @param hr The HRESULT
        /// @return error information
        inline std::string GetLastErrorInfo(HRESULT hr)
        {
            DWORD last_error = ::GetLastError();
            return std::system_category().message(hr) + ". Last error: " + mal_toolkit::FormatErrorAsString(last_error) + " or " + std::to_string(last_error);
        }
    }
    /// @brief Logs the debug information that is stored in the debug info queue
    /// @details This function is only available in debug mode
    inline void LogDebugInfoQueue()
    {
#if !defined(_DEBUG)
        return;
#else
        UINT64 message_count = api().debug_info_queue->GetNumStoredMessages();

        for (UINT64 i = 0; i < message_count; i++)
        {
            SIZE_T message_size = 0;
            api().debug_info_queue->GetMessage(i, nullptr, &message_size); // get the size of the message

            D3D11_MESSAGE *message = (D3D11_MESSAGE *)malloc(message_size); // allocate enough space
            if (message == nullptr || FAILED(api().debug_info_queue->GetMessage(i, message, &message_size)))
            {
                free(message);
                continue;
            }

            spdlog::debug("[DirectX11] " + std::string(message->pDescription, message->pDescription + message->DescriptionByteLength));

            free(message);
        }

        api().debug_info_queue->ClearStoredMessages();
#endif
    }

#ifdef ENGINE_NO_SOURCE_LOCATION
    /// @brief Asserts that the HRESULT is SUCCEEDED
    /// @detail This function is only available if MAL_TOOLKIT_ASSERT_ENABLED is defined(it is automatically defined in debug mode)
    /// @param hr The HRESULT
    /// @param message The message to print if the assert fails
    inline void Assert(HRESULT hr, std::string_view message = "")
    {
        if constexpr (!MAL_TOOLKIT_ASSERT_ENABLED)
        {
            return;
        }
        if (SUCCEEDED(hr)) [[likely]]
        {
            return;
        }
        LogDebugInfoQueue();
        mal_toolkit::Assert(SUCCEEDED(hr), std::basic_string(message) + " " + _detail::GetLastErrorInfo(hr));
    }
    /// @brief Asserts that the HRESULT is SUCCEEDED
    /// @detail This function Asserts even in release mode. It is used for critical errors.
    /// @param hr The HRESULT
    /// @param message The message to print if the assert fails
    inline void AlwaysAssert(HRESULT hr, std::string_view message = "")
    {
        if (SUCCEEDED(hr)) [[likely]]
        {
            return;
        }
        LogDebugInfoQueue();
        mal_toolkit::Assert(SUCCEEDED(hr), std::basic_string(message) + " " + _detail::GetLastErrorInfo(hr));
    }
#else
    /// @brief Asserts that the HRESULT is SUCCEEDED
    /// @detail This function is only available if MAL_TOOLKIT_ASSERT_ENABLED is defined(it is automatically defined in debug mode)
    /// @param hr The HRESULT
    /// @param message The message to print if the assert fails
    /// @param location The source location of the assert
    inline void Assert(HRESULT hr, std::string_view message = "", std::source_location location = std::source_location::current())
    {
        if constexpr (!MAL_TOOLKIT_ASSERT_ENABLED)
        {
            return;
        }
        if (SUCCEEDED(hr)) [[likely]]
        {
            return;
        }
        LogDebugInfoQueue();
        mal_toolkit::Assert(SUCCEEDED(hr), std::basic_string(message) + " " + _detail::GetLastErrorInfo(hr), location);
    }
    /// @brief Asserts that the HRESULT is SUCCEEDED
    /// @detail This function Asserts even in release mode. It is used for critical errors.
    /// @param hr The HRESULT
    /// @param message The message to print if the assert fails
    /// @param location The source location of the assert
    inline void AlwaysAssert(HRESULT hr, std::string_view message = "", std::source_location location = std::source_location::current())
    {
        if (SUCCEEDED(hr)) [[likely]]
        {
            return;
        }
        LogDebugInfoQueue();
        mal_toolkit::AlwaysAssert(SUCCEEDED(hr), std::basic_string(message) + " " + _detail::GetLastErrorInfo(hr), location);
    }

#endif
}