//
//  XASendData.cpp
//  XA
//
//  Created by mini xingcloud on 12-2-15.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//
#include "XASendData.h"
#include "curl.h"
extern void    XAPRINT(const char *fmt,...);

namespace XingCloud
{
    namespace XA
    {
        
        TaskGroup   XASendData::taskGroup;
        std::vector<std::string> XASendData::cache;//发送失败的缓存
        static size_t postWriteData(void *recvBuffer,size_t size,size_t nmemb,void *userParam);
        size_t Getcontentlengthfunc(void *ptr, size_t size, size_t nmemb, void *stream) ;
        unsigned int  postPerform(void *param);
        Mutex   XASendData::postMutex;
        std::map<int,CURL*> indexCURL;
        XASendData::XASendData()
        {
            
        }
        XASendData::~XASendData()
        {
            //curl_easy_cleanup(easy_handle);
        }
        
        bool    XASendData::getMethodSend(const char *buffer)
        {
            std::string request("http://analytic.xingcloud.com/index.php?json=");
            request.append(buffer);
            CURL* easy_handle=curl_easy_init();
            struct curl_slist *header=NULL;
            header= curl_slist_append(header,request.c_str());
            //curl_easy_setopt(easy_handle, CURLOPT_VERBOSE,1L);/*open comment when debug mode.*/
            curl_easy_setopt(easy_handle,CURLOPT_HTTPHEADER,header);
            CURLcode code=curl_easy_perform(easy_handle);//
            curl_slist_free_all(header);
            XingCloud::XAThreadPool::ExecuteTask::addTask(new XingCloud::XAThreadPool::XATask(postPerform,easy_handle,&taskGroup));
            curl_easy_cleanup(easy_handle);
            
            return code==CURLE_OK;
        }
        
        bool    XASendData::postMethodSend(char *buffer)
        {
            XAPRINT(buffer);
           
            XingCloud::XAThreadPool::ExecuteTask::addTask(new XingCloud::XAThreadPool::XATask(postPerform,buffer,&taskGroup));
            return true;
        }
        unsigned int  postPerform(void *param)
        {
            Lock lock(XASendData::postMutex);
            bool receiveOK=false;
            bool *dataSendSuccess=&receiveOK;
            static int index=0;
            CURL* easy_handle = curl_easy_init();
            indexCURL[index++]=easy_handle;
            char *encodedURL = curl_easy_escape(easy_handle,(char*)param, strlen((char*)param));
            curl_easy_setopt(easy_handle,CURLOPT_URL,"http://analytic.xingcloud.com/index.php?");
            curl_easy_setopt(easy_handle, CURLOPT_HEADERFUNCTION, Getcontentlengthfunc);
            curl_easy_setopt(easy_handle, CURLOPT_HEADERDATA,&receiveOK);
            char *temp= new char[strlen(encodedURL)+5];
            sprintf(temp,"json=%s",encodedURL);
            curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDS,temp);
            curl_easy_setopt(easy_handle,CURLOPT_WRITEFUNCTION,postWriteData);//receive callback function
            curl_easy_setopt(easy_handle,CURLOPT_WRITEDATA,dataSendSuccess);
            curl_easy_setopt(easy_handle,CURLOPT_WRITEDATA,easy_handle);
            curl_easy_setopt(easy_handle,CURLOPT_POST,1);
            curl_easy_setopt(easy_handle,CURLOPT_VERBOSE,1); /* open comment when debug mode.*/
            CURLcode code=curl_easy_perform(easy_handle);
            
            if(!(code==CURLE_OK && receiveOK && dataSendSuccess))
            {
                XASendData::cache.push_back((char*)param);
            }
            
            curl_easy_cleanup(indexCURL[index]);
            delete temp;
            return code==CURLE_OK;
        }
        
        static size_t postWriteData(void *recvBuffer,size_t size,size_t nmemb,void *userParam)
        {
            // /*1.0ms*/
            char *temp = (char*)recvBuffer;
            if(temp == NULL)
                return size*nmemb;
            temp=strstr((char*)recvBuffer,"/*");
            if(temp!=NULL && strstr((char*)recvBuffer,"*/")!=NULL)
            {
                *(bool*)userParam=true;
                
            }
            return size*nmemb;
        }
        size_t Getcontentlengthfunc(void *ptr, size_t size, size_t nmemb, void *stream) 
        {
            char *temp=strstr((char*)ptr,"200");
            if(temp!=NULL)
            {
                *(bool*)stream=true;
                
            }
            return size * nmemb;
        }
    }
}