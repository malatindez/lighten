namespace engine::utils
{
    template<typename T, typename... U>
    size_t GetFunctionAddress(std::function<T(U...)> f) {
        typedef T(FnType)(U...);
        FnType ** fnPointer = f.template target<FnType*>();
        return reinterpret_cast<size_t>(*fnPointer);
    }
    // TODO (malatindez): improve this later
    // add support for class methods
    template <typename F>
    class CallbackSystem
    {
    public:
        using CallbackFunction = F;
        using CallbackId = uint32_t;
        using CallbackAddress = size_t;
        CallbackId AddCallback(CallbackFunction const &callback)
        {
            auto id = next_id_++;
            callbacks_.emplace(id, std::move(callback));
            callbacks_adresses_.emplace(id, GetFunctionAddress(callback));
            return id;
        }
        void RemoveCallback(CallbackId id)
        {
            callbacks_.erase(id);
            callbacks_adresses_.erase(id);
        }
        void RemoveCallback(CallbackFunction const &callback)
        {
            auto address = GetFunctionAddress(callback);
            for (auto const &[id, callback_address] : callbacks_adresses_)
            {
                if (callback_address == address)
                {
                    RemoveCallback(id);
                    return;
                }
            }
        }
        void InvokeCallbacks(T const &data)
        {
            for (auto const &[id, callback] : callbacks_)
            {
                callback(data);
            }
        }

    private:
        std::unordered_map<CallbackId, Callback> callbacks_;
        std::unordered_map<CallbackId, CallbackAddress> callback_adresses_;
        CallbackId next_id_ = 0;
    };
}