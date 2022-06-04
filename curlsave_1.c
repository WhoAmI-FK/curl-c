#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>

int main()
{
        CURL* curl;
        CURLcode res;
        FILE* output;
        char address[] = "https://c-for-dummies.com/curl_test.txt";
        char error_buffer[CURL_ERROR_SIZE];
        char filename[]="curlout.txt";
        curl=curl_easy_init();
        output=fopen(filename,"w");
        if(output==NULL)
        {
                fprintf(stderr,"Unable to create output file %s\n",filename);
                exit(1);
        }
        if(curl==NULL)
        {
                fprintf(stderr,"Unable to initialize curl\n");
                exit(1);
        }
        curl_easy_setopt(curl,CURLOPT_URL,address);
        curl_easy_setopt(curl,CURLOPT_FOLLOWLOCATION,1L);
        curl_easy_setopt(curl,CURLOPT_ERRORBUFFER,error_buffer);
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,NULL);
        curl_easy_setopt(curl,CURLOPT_WRITEDATA,output);
        res = curl_easy_perform(curl);
        if(res!=CURLE_OK)
        {
                fprintf(stderr,"Unable to curl %s\n",address);
                fprintf(stderr,"Curl error: %s\n",error_buffer);
                exit(1);
        }
        curl_easy_cleanup(curl);
        fclose(output);
        printf("Data received. File '%s' is written\n",filename);
        return(0);
}