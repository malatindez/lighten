#pragma once
#include "utils/debug.hpp"
#include "utils/win-utils.hpp"
#include <system_error>
namespace engine::direct3d
{
    namespace _detail
    {
        inline std::string GetLastErrorInfo(HRESULT hr)
        {
            DWORD last_error = ::GetLastError();
            return std::system_category().message(hr) + ". Last error: " + utils::FormatErrorAsString(last_error) + " or " + std::to_string(last_error);
        }
    }
#ifdef ENGINE_NO_SOURCE_LOCATION
    inline void Assert(HRESULT hr, std::string_view message = "")
    {
        if constexpr (!DEBUG_UTILS_DEBUG_ENABLED)
        {
            return;
        }
        if (SUCCEEDED(hr)) [[likely]]
        {
            return;
        }
        utils::Assert(SUCCEEDED(hr), std::basic_string(message) + " " + _detail::GetLastErrorInfo(hr));
    }

    inline void AlwaysAssert(HRESULT hr, std::string_view message = "")
    {
        if (SUCCEEDED(hr)) [[likely]]
        {
            return;
        }
        utils::Assert(SUCCEEDED(hr), std::basic_string(message) + " " + _detail::GetLastErrorInfo(hr));
    }
#else
    inline void LogDebugInfoQueue()
    {
#if !defined(_DEBUG)
        return;
#else
        UINT64 message_count = api::debug_info_queue->GetNumStoredMessages();

        for (UINT64 i = 0; i < message_count; i++)
        {
            SIZE_T message_size = 0;
            api::debug_info_queue->GetMessage(i, nullptr, &message_size); //get the size of the message

            D3D11_MESSAGE *message = (D3D11_MESSAGE *)malloc(message_size); //allocate enough space
            if (FAILED(api::debug_info_queue->GetMessage(i, message, &message_size)))
            {
                continue;
            }

            spdlog::debug("[DirectX11] " + std::string(message->pDescription, message->pDescription + message->DescriptionByteLength));

            free(message);
        }

        api::debug_info_queue->ClearStoredMessages();
#endif
    }
    inline void Assert(HRESULT hr, std::string_view message = "", std::source_location location = std::source_location::current())
    {
        if constexpr (!DEBUG_UTILS_DEBUG_ENABLED)
        {
            return;
        }
        if (SUCCEEDED(hr)) [[likely]]
        {
            return;
        }
        LogDebugInfoQueue();
        utils::Assert(SUCCEEDED(hr), std::basic_string(message) + " " + _detail::GetLastErrorInfo(hr), location);
    }

    inline void AlwaysAssert(HRESULT hr, std::string_view message = "", std::source_location location = std::source_location::current())
    {
        if (SUCCEEDED(hr)) [[likely]]
        {
            return;
        }
        LogDebugInfoQueue();
        utils::AlwaysAssert(SUCCEEDED(hr), std::basic_string(message) + " " + _detail::GetLastErrorInfo(hr), location);
    }

#endif

}