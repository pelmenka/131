#pragma once

namespace render
{
    class _object
    {
    public:
        virtual void free() = 0;

    protected:
        void _addToList();
        void _removeFromList();
        virtual void _alloc() = 0;
    };
}
