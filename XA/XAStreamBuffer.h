
#ifndef XASTREAMBUFFER_H_
#define XASTREAMBUFFER_H_
#include<string>
namespace XingCloud
{ 
    namespace XA
    {
        class XAStreamBuffer
        {
        public:
            XAStreamBuffer();
            ~XAStreamBuffer();

            int read(void *buffer,size_t size);
            void write(const void *buffer,size_t size);

            unsigned int get_length()
            {
                return length;
            }
            unsigned char *getBuffer()
            {
                return dataBuffer;
            }
        private:
            unsigned char *relloc(size_t size);
            unsigned char *dataBuffer;
            unsigned int position;
            unsigned int length;
            size_t       totalLength;
        };
}/*XingCloud*/}/*XA*/
#endif
