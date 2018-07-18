#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include <sys/socket.h>  
#include <bluetooth/bluetooth.h>  
#include <bluetooth/rfcomm.h>  
  
int main (int argc,char **argv)  
{  
    struct sockaddr_rc loc_addr ={0},rem_addr={0};  
    char buf[1024] ={0};//,*addr;   
    int s,client, bytes_read,result;  
    int opt = sizeof(rem_addr);  
    /*创建socket */  
    printf("Creating socket...\n");  
    s =socket(PF_BLUETOOTH,SOCK_STREAM,BTPROTO_RFCOMM);  
    if(s<0)  
    {  
        perror("create socket error");  
        exit(1);  
    }  
    else  
    {  
        printf("create socket success!\n");  
    }  

    loc_addr.rc_family=AF_BLUETOOTH;  
    loc_addr.rc_bdaddr=*BDADDR_ANY;  
    loc_addr.rc_channel=(uint8_t)1;  

    /*绑定socket*/  
    printf("Binding socket...\n");  
    result=bind(s,(struct sockaddr *)&loc_addr, sizeof(loc_addr));  
    if(result<0)  
    {  
        perror("bind socket error:");  
        exit(1);  
    }  
    else  
    {  
        printf("bind success!\n");                                                                                           
    }  

    /*监听*/  
    printf("Listen... \n");  
    result=listen(s,5);  
    if(result<0)  
    {  
        printf("error:%d\n:",result);  
        perror("listen error:");  
        exit(1);  
    }  
    else  
    {  
        printf("requested!\n");  
    }
	
    /*Accept*/  
    printf("Accepting...\n");  
    client= accept(s,(struct sockaddr *)&rem_addr, (socklen_t *)&opt);  
    if(client<0)  
    {  
        perror("accept error\n");  
        exit(1);  
    }  
    else  
    {  
        printf("OK!\n");  
    }  

    ba2str(&rem_addr.rc_bdaddr,buf);  

    printf("accepted connection from %s \n",buf);  
    memset(buf,0,sizeof(buf));  

    while(1)  
    {  
        bytes_read = read(client,buf,sizeof(buf));  
        if(bytes_read>0){  
            printf("Received:%s\n",buf);  
            if(strcmp(buf,"goodbye")==0)  
            {  
                printf("Client is down!!");  
                exit(1);  
            }  
            memset(buf,0,bytes_read);  
        }  
    }  
    close(client);  
    close(s);  
    return 0;  
}
