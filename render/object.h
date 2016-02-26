#pragma once

namespace render
{
    class _object
    {
    public:
        virtual void free() = 0;
        void _addToList();
        void _removeFromList();
    protected:
        virtual void _alloc() = 0;
    };
}
