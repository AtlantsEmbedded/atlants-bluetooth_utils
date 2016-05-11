#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

int main(int argc, char **argv)
{	
	//inquiry_info initialised with a null pointer 
    inquiry_info *ii = NULL;
    //initializing maximum response and number of response
    int max_rsp, num_rsp,j;
    //declaring host id, host socket used for communication 
    int dev_id, sock, len, flags;
    int i;
    
    
    char addr[19] = { 0 };
    char name[248] = { 0 };

    dev_id = hci_get_route(NULL);
    sock = hci_open_dev( dev_id );

    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        exit(1);
    }

    len  = 8;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));
    
    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if( num_rsp < 0 ) perror("hci_inquiry");

    for (i = 0; i < num_rsp; i++) {
        ba2str(&(ii+i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), 
            name, 0) < 0)
            {
			strcpy(name, "[unknown]");
			}
        printf("%d: %s  %s\n", i+1, addr, name);
    }
    
    printf("\n Choose your device by number\n");
    scanf("%d",&j);
    ba2str(&(ii+j-1)->bdaddr, addr);
    if (hci_read_remote_name(sock, &(ii+j-1)->bdaddr, sizeof(name), 
            name, 0) < 0)
            {
			strcpy(name, "[unknown]");
			}
    printf("The device %d is chosen, \nadress is: %s\nname is: %s\n", j, addr, name);
    free( ii );
    close( sock );
    return 0;
}
