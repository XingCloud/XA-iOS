#include "XAStreamBuffer.h"
namespace XingCloud
{
    namespace XA
    {
        XAStreamBuffer::XAStreamBuffer()
        {
            dataBuffer =new unsigned char[4096];
            if(dataBuffer==0)
                throw std::string("can not allocate");
            length=0;
            position=0;
            totalLength=4096;
        }
        XAStreamBuffer::~XAStreamBuffer()
        {
            delete dataBuffer;
        }
        int XAStreamBuffer::read(void *buffer,size_t size)
        {
            if(length==0||position==length||(size >length-position))
                return -1;
            memcpy(buffer,dataBuffer+position,size);
    
            position+=size;
            return 0;
        }
        void XAStreamBuffer::write(const void *buffer,size_t size)
        {
            relloc(size);
            memcpy(dataBuffer+length,buffer,size);
    
            length+=size;
        }
        unsigned char* XAStreamBuffer::relloc(size_t size)
        {
            if(dataBuffer==NULL)
            {
                length=size;
                dataBuffer=new uint8_t[size];
            }
            else if(size>4096-length)
            {
                uint8_t *newBytes=new uint8_t[totalLength*2];
                totalLength*=2;
                memcpy(newBytes,dataBuffer,length);
                delete dataBuffer;
                dataBuffer=newBytes;
            }
            return dataBuffer;
        }
}/*XingCloud*/}//namespace XA
