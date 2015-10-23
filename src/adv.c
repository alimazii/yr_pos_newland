#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/socket.h>
#include "aliqr.h"
#include "adv.h"


char* http_packet_str = "POST /qrcode/%s? HTTP/1.1\r\n" 
"Host: "ALISER":"SER_PORT"\r\n"
"User-Agent: curl/7.38.0\r\nAccept: */*\r\n"
"Content-Length: %d\r\n"
"Content-Type: application/x-www-form-urlencoded\r\n\r\n%s";

int initLoadImg(char* ImgName)
{
    int sockfd, numbytes;
    struct sockaddr_in backend_addr;
    int fd, size, length = 0, bufsize;
    int b_offset,i;
    char buf[1024], *p;
    char imgFileName[10];
    char post_data[10];
    char post_str[256];

    sprintf(post_data, "ai=%s", ImgName); 
    sprintf(post_str, http_packet_str, "adimg", strlen(post_data), post_data);

    
    if ((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1) 
    {
        //perror("socket");
        //exit(1);
        DebugErrorInfo("socket failed\n");
        return 1;
    }

    backend_addr.sin_family = AF_INET;
    backend_addr.sin_port = htons(PORT); 
    backend_addr.sin_addr.s_addr = inet_addr(ALISER);
    bzero(&(backend_addr.sin_zero), 8);

    if (connect(sockfd, (struct sockaddr *)&backend_addr, sizeof(struct
    sockaddr)) == -1) 
    {
    	  DebugErrorInfo("connect failed\n");
        //perror("connect");
        //exit(1);
        close(sockfd);
        return 1;
    }
    
    if ( send(sockfd, post_str, sizeof(post_str), 0) < 0 )
    {
         //perror("send to backend server failed");
         //exit(1);
         DebugErrorInfo("send to backend server failed\n");
         close(sockfd);
         return 1;
    }

    recv(sockfd,buf,sizeof(buf),0);


    /* check if server return 200 0K */
    DebugErrorInfo("%s\n", buf);
    
    if ( strstr(buf, "HTTP/1.1 200 OK") != NULL )
    {
         /* get the file length */

         p = strstr(buf, "Content-Length: ");
         p += 16;
         
         while ( *p !='\r' )
         {
              length = length*10 + *p - '0';
              p++;
         }
         DebugErrorInfo("file length: %d\n", length);
         sprintf(imgFileName, "%s.bmp", ImgName);
         DebugErrorInfo("file name: %s\n", imgFileName);
         fd = open(imgFileName, O_WRONLY|O_CREAT|O_TRUNC, 0664);
         
         p = strstr(buf, "BM");
         b_offset = 1024 - (p - buf);
         length = length - b_offset;
         write(fd, p, b_offset);
         
         while ( length > 0 )
         {
              if ( length > 1024 ) bufsize = 1024;
              else bufsize = length;

              size = read(sockfd,buf,bufsize);
              if (size <= 0){ 
              	 length = 0;
              	 /*TODO: add failed tag here*/
                 break;
              }   
              DebugErrorInfo("==========%d\n", size);
              write(fd, buf, size);
              length = length - size;
         }

         DebugErrorInfo("file download complete\n");
         close(fd);
       	
    }   
    
    close(sockfd);
    return 0;
	
}

int imgOpPost(int type, char* post_data)	
{
	  int sockfd;
    struct sockaddr_in backend_addr;
    int fd;
    char buf[1024], *p;
        
    char post_str[256];

    switch (type)
    {
        case IMG_CLEAR: 
        	sprintf(post_str, http_packet_str, "clearimg", strlen(post_data), post_data);
        	break;
        	
        case IMG_CONFIRM:
        	sprintf(post_str, http_packet_str, "adimgconfirm", strlen(post_data), post_data);
        	break;
        	        	
        case IMG_DEL_CONFIRM:
        	sprintf(post_str, http_packet_str, "adimgdelconfirm", strlen(post_data), post_data);
        	break; 
        	       	
        default:
        	break;
    }    		   
  
    DebugErrorInfo("%s\n", post_str);

    
    if ((sockfd = socket(AF_INET,SOCK_STREAM,0))==-1) 
    {
        //perror("socket");
        //exit(1);
        DebugErrorInfo("socket failed\n");
        return 1;
    }

    backend_addr.sin_family = AF_INET;
    backend_addr.sin_port = htons(PORT); 
    backend_addr.sin_addr.s_addr = inet_addr(ALISER);
    bzero(&(backend_addr.sin_zero), 8);

    if (connect(sockfd, (struct sockaddr *)&backend_addr, sizeof(struct
    sockaddr)) == -1) 
    {
    	  DebugErrorInfo("connect failed\n");
    	  close(sockfd);
    	  return 1;
        //perror("connect");
        //exit(1);
    }
    
    /* send loading status to backend server */
    if ( send(sockfd, post_str, strlen(post_str), 0) < 0 )
    {
         //perror("send to backend server failed");
         //exit(1);
         DebugErrorInfo("send to backend server failed\n");
         close(sockfd);
         return 1;
    }
    
    recv(sockfd,buf,sizeof(buf),0);
    
    DebugErrorInfo("%s\n", buf);
    
    if ( strstr(buf, "HTTP/1.1 200 OK") != NULL )
    {             	   
    	    DebugErrorInfo("backend server got loading status\n");
    } 
    
    close(sockfd);
    return 0;   
}