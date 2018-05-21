#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

void die(char *s)
{
    perror(s);
    exit(1);
}

int main(void)
{
    int server = 0;
    
    struct sockaddr_in serv_addr, si_other;
    int recv_len,slen;

    server = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    printf("Socket retrieve success\n");

    memset(&serv_addr, 0, sizeof(serv_addr));
    
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(7003);

    //bind socket to port
    if( bind(server, (struct sockaddr*) &serv_addr, sizeof(serv_addr) ) == -1)
    {
        die("bind");
    }


    while(1)
    {
        unsigned char offset_buffer[10] = {'\0'}; 
        unsigned char command_buffer[2] = {'\0'}; 
        int offset;
        int command;
        
        
        
        printf("Waiting for client to send the command (Full File (0) Partial File (1)\n"); 

        while((recv_len = recvfrom(server, command_buffer, 2, 0, (struct sockaddr*)&si_other, &slen)) == -1);
            sscanf(command_buffer, "%d", &command); 
        
        
                
        if(command == 0)
                offset = 0;        
        else
        {
                printf("Waiting for client to send the offset\n");  
                while((recv_len = recvfrom(server, offset_buffer, 10, 0, (struct sockaddr*)&si_other, &slen)) == -1);
                    sscanf(offset_buffer, "%d", &offset);         
        }
        
            
        /* Open the file that we wish to transfer */
        FILE *fp = fopen("source_file.txt","rb");
        if(fp==NULL)
        {
            printf("File opern error");
            return 1;   
        }   

        /* Read data from file and send it */
                 fseek(fp, offset, SEEK_SET);
        while(1)
        {
            /* First read file in chunks of 256 bytes */
            unsigned char buff[10]={0};
            int nread = fread(buff,1,10,fp);
            printf("Bytes read %d \n", nread);        

            /* If read was success, send data. */
            if(nread > 0)
            {
                printf("Sending \n");

                if (sendto(server, buff, nread, 0, (struct sockaddr*)&si_other, sizeof(si_other)) == -1)
                {
                    die("sendto()");
                }               
            }

            /*
             * There is something tricky going on with read .. 
             * Either there was error, or we reached end of file.
             */
            if (nread < 10)
            {
                if (feof(fp))
                    printf("End of file\n");
                if (ferror(fp))
                    printf("Error reading\n");
                break;
            }



        }

        sleep(1);
    }


    return 0;
}


