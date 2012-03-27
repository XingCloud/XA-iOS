//
//  XASendData.cpp
//  XA
//
//  Created by mini xingcloud on 12-2-15.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//
#include "XASendData.h"
#include "XADataManager.h"
#include "curl.h"
extern void    XAPRINT(const char *fmt,...);

namespace XingCloud
{
    namespace XA
    {
        
        TaskGroup   XASendData::taskGroup;
       
        size_t postWriteData(void *recvBuffer,size_t size,size_t nmemb,void *userParam);
        size_t Getcontentlengthfunc(void *ptr, size_t size, size_t nmemb, void *stream) ;
        
        unsigned int  postPerform(void *param);
        Mutex   XASendData::postMutex;
        
        CURL *XASendData::easy_handle = NULL;
        XASendData::XASendData()
        {
            
        }
        XASendData::~XASendData()
        {
            
        }
        
        bool    XASendData::getMethodSend(const char *buffer)
        {
            std::string request("http://analytic.xingcloud.com/index.php?json=");
            
            CURL* easy_handle=curl_easy_init();
            char *encodedURL = curl_easy_escape(easy_handle,buffer, strlen(buffer));
            request.append(encodedURL);
            //struct curl_slist *header=NULL;
            //header= curl_slist_append(header,request.c_str());
            //curl_easy_setopt(easy_handle, CURLOPT_VERBOSE,1L);/*open comment when debug mode.*/
            
            curl_easy_setopt(easy_handle,CURLOPT_URL,request.c_str());
            //curl_easy_setopt(easy_handle,CURLOPT_HTTPHEADER,header);
            
            CURLcode code=curl_easy_perform(easy_handle);//
            
            //curl_slist_free_all(header);
            
            curl_easy_cleanup(easy_handle);
            
            return code==CURLE_OK;
        }
        
        bool    XASendData::postMethodSend(const char *buffer,int dataNumber)
        {
            if(buffer==0)
                return false;
            
            //XAPRINT(buffer);
            
            postData  *p = new postData[sizeof(postData)];
            p->data =const_cast<char*>(buffer);
            p->sendEventNumber =dataNumber;
            XingCloud::XAThreadPool::ExecuteTask::addTask(new XingCloud::XAThreadPool::XATask(postPerform,p,&taskGroup));
            
            return true;
        }
        unsigned int  postPerform(void *param)
        {
            
            Lock lock(XASendData::postMutex);
            postData  *p =(postData*)param;
            
            bool receiveOK=false;
            bool *dataSendSuccess=&receiveOK;
          
            CURL* easy_handle = curl_easy_init();
            XASendData::easy_handle = easy_handle;
            char *encodedURL = curl_easy_escape(easy_handle,p->data, strlen(p->data));
            curl_easy_setopt(easy_handle,CURLOPT_URL,"http://analytic.xingcloud.com/index.php?");
            curl_easy_setopt(easy_handle, CURLOPT_HEADERFUNCTION, Getcontentlengthfunc);
            curl_easy_setopt(easy_handle, CURLOPT_HEADERDATA,&receiveOK);
            char *temp= new char[strlen(encodedURL)+6];
            sprintf(temp,"json=%s",encodedURL);
            curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDS,temp);
            curl_easy_setopt(easy_handle,CURLOPT_POSTFIELDSIZE,strlen(temp));
            curl_easy_setopt(easy_handle,CURLOPT_WRITEFUNCTION,postWriteData);//receive callback function
            curl_easy_setopt(easy_handle,CURLOPT_WRITEDATA,dataSendSuccess);
           
            curl_easy_setopt(easy_handle,CURLOPT_POST,1);
            //curl_easy_setopt(easy_handle,CURLOPT_VERBOSE,1); /* open comment when debug mode.*/
            
            CURLcode code=curl_easy_perform(easy_handle);
            
            XASendData::easy_handle = NULL;
            if((code==CURLE_OK && receiveOK && dataSendSuccess))
            {//发送成功则删除缓存
                XAPRINT("send event success!");
                XADataProxy::getInstance()->handleSendSucess(p->sendEventNumber);
            }
            else
            {//
                XAPRINT("send event failed!");
               XADataProxy::getInstance()->handleSendFailed(p->sendEventNumber);
            }
            curl_free(encodedURL);
            curl_easy_cleanup(easy_handle);
            
            free(p->data);
            delete p;
            delete temp;
            return code==CURLE_OK;
        }
        
        size_t postWriteData(void *recvBuffer,size_t size,size_t nmemb,void *userParam)
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