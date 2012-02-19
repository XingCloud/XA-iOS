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
        XASendData::XASendData()
        {
            
        }
        XASendData::~XASendData()
        {
	
        }
        
        bool    XASendData::getMethodSend(const char *buffer)
        {
            std::string request("http://analytic.337.com/index.php?json=$");
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
        
        bool    XASendData::postMethodSend( char *buffer)
        {
            XAPRINT(buffer);
            XingCloud::XAThreadPool::ExecuteTask::addTask(new XingCloud::XAThreadPool::XATask(postPerform,buffer,&taskGroup));
            return true;
        }
        unsigned int  postPerform(void *param)
        {
            bool receiveOK=false;
            bool dataSendSuccess=false;
            CURL* easy_handle=curl_easy_init();
            curl_easy_setopt(easy_handle,CURLOPT_URL,"http://analytic.337.com/index.php");
            curl_easy_setopt(easy_handle, CURLOPT_HEADERFUNCTION, Getcontentlengthfunc);
            curl_easy_setopt(easy_handle, CURLOPT_HEADERDATA,&receiveOK);
            curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDS,(char*)param);
            curl_easy_setopt(easy_handle,CURLOPT_WRITEFUNCTION,postWriteData);//receive callback function
            curl_easy_setopt(easy_handle,CURLOPT_WRITEDATA,&dataSendSuccess);
            curl_easy_setopt(easy_handle,CURLOPT_WRITEDATA,easy_handle);
            curl_easy_setopt(easy_handle,CURLOPT_POST,1);
            //curl_easy_setopt(easy_handle,CURLOPT_VERBOSE,1); /* open comment when debug mode.*/
            CURLcode code=curl_easy_perform(easy_handle);
            if(!(code==CURLE_OK && receiveOK && dataSendSuccess))
            {
                XASendData::cache.push_back((char*)param);
            }
            curl_easy_cleanup(easy_handle);
            return code==CURLE_OK;
        }
        
        static size_t postWriteData(void *recvBuffer,size_t size,size_t nmemb,void *userParam)
        {
            // /*1.0ms*/
            char *temp=strstr((char*)recvBuffer,"/*");
            if(temp!=NULL)
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