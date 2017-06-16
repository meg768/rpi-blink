
class Memory {

    public:
        Memory() {
            _bytes = NULL;
        }

        ~Memory() {
            free();
        }

        void free() {
            if (_bytes != NULL)
                ::free(_bytes);

            _bytes = NULL;
        }

        void *alloc(int size) {
            void *bytes = ::malloc(size);

            if (bytes == NULL)
                return NULL;

            free();

            return _bytes = bytes;
        }

    private:
        void *_bytes;
};
