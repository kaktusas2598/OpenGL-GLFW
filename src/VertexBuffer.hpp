#ifndef __VertexBuffer__
#define __VertexBuffer__

class VertexBuffer {
    public:
        VertexBuffer(const void* data, unsigned int size);
        ~VertexBuffer();

        void bind() const;
        void unbind() const;
    private:
        unsigned int rendererID;
};

#endif // __VertexBuffer__
