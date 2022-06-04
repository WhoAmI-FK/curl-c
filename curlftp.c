#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define BUFFER_SIZE 2048

static size_t read_callback(void* ptr, size_t size, size_t nmemb, void* user_data);


struct data_chunk{
        char* buffer;
        size_t size;
};


int main()
{
        char ftp_upload[] = "ftp://128.0.0.0/image1.jpg";
        char username_password[] = "username:password";
        char upload_filename[] = "image1.jpg";
        char input_buffer[BUFFER_SIZE];
        CURL* curl;
        CURLcode res;
        struct data_chunk ftp_buffer;
        FILE* input;
        int c;
        input = fopen( upload_filename, "r");
        if( input==NULL)
        {
                fprintf(stderr,"Unable to read from %s\n",upload_filename);
                exit(1);
        }
        ftp_buffer.buffer = malloc(1);
        ftp_buffer.size = 0;
        if( ftp_buffer.buffer==NULL )
        {
                fprintf(stderr,"Unable to initialize buffer\n");
                fclose(input);
                exit(1);
        }

        while(1)
        {
                c = fread( input_buffer, sizeof(char), BUFFER_SIZE, input);

                ftp_buffer.buffer = realloc( ftp_buffer.buffer, ftp_buffer.size+c+1);
                if( ftp_buffer.buffer==NULL )
                {
                        fprintf(stderr,"Unable to resize buffer\n");
                        exit(1);
                }

                memcpy( &ftp_buffer.buffer[ftp_buffer.size], input_buffer, c );

                ftp_buffer.size += c;

                ftp_buffer.buffer[ftp_buffer.size] = 0;

                if( c < BUFFER_SIZE)
                        break;

        }
        fclose(input);


        curl = curl_easy_init();

        printf("Preparing FTP...");

        curl_easy_setopt(curl, CURLOPT_URL, ftp_upload);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_USERPWD, username_password);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, read_callback);
        curl_easy_setopt(curl, CURLOPT_READDATA, &ftp_buffer);
        curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)ftp_buffer.size);


        res = curl_easy_perform(curl);
        if( res!=CURLE_OK )
        {
                fprintf(stderr,"curl upload failed: %s\n",curl_easy_strerror(res));
                exit(1);
        }


        curl_easy_cleanup(curl);

        puts("FTP upload complete");
        return(0);
}

static size_t read_callback(void* ptr, size_t size, size_t nmemb, void* user_data)
{
        struct data_chunk* upload;
        size_t max;
        upload = (struct data_chunk*) user_data;
        max=size*nmemb;
        if(max<1)
                return(0);
        if(upload->size)
        {
                if(max>upload->size)
                        max = upload->size;
                memcpy(ptr,upload->buffer,max);
                upload->buffer+=max;
                upload->size-=max;
                return(max);
        }
        return(0);
}