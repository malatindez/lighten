#pragma once
namespace engine::interfaces
{
    class Updatable
    {
    public:
        // Returns false if this function shouldn't be called anymore
        virtual bool Update(float delta_time) = 0;
        virtual ~Updatable() {}
    };
}