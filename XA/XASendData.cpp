//
//  XASendData.cpp
//  XA
//
//  Created by mini xingcloud on 12-2-15.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//
#include "XASendData.h"
#include "curl.h"
#include <string>
namespace XingCloud
{
    namespace XA
    {
        static size_t postWriteData(void *recvBuffer,size_t size,size_t nmemb,void *userParam);
        unsigned int  postPerform(void *param);
        XASendData::XASendData()
        {
            //easy_handle=curl_easy_init();
        }
        XASendData::~XASendData()
        {
            //curl_easy_cleanup(easy_handle);
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
        
        bool    XASendData::postMethodSend(const char *buffer)
        {
            CURL* easy_handle=curl_easy_init();
            curl_easy_setopt(easy_handle,CURLOPT_URL,"http://analytic.337.com/index.php");
            curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDS,buffer);
            curl_easy_setopt(easy_handle,CURLOPT_WRITEFUNCTION,postWriteData);//receive callback function
            curl_easy_setopt(easy_handle,CURLOPT_WRITEDATA,easy_handle);
            curl_easy_setopt(easy_handle,CURLOPT_POST,1);
            //curl_easy_setopt(easy_handle,CURLOPT_VERBOSE,1); /* open comment when debug mode.*/
            XingCloud::XAThreadPool::ExecuteTask::addTask(new XingCloud::XAThreadPool::XATask(postPerform,easy_handle,&taskGroup));
            
            return true;
        }
        unsigned int  postPerform(void *param)
        {
            CURL* easy_handle=(CURL*)param;
            CURLcode code=curl_easy_perform(easy_handle);
            curl_easy_cleanup(easy_handle);
            return code==CURLE_OK;
        }
        static size_t postWriteData(void *recvBuffer,size_t size,size_t nmemb,void *userParam)
        {
            
            return size*nmemb;
        }
    }
}