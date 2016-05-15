/**
 * @file simplescan.c
 * @author Alex V. Joseph,
 * 		   Frederic Simard (frederic.simard.1@outlook.com) || Atlants Embedded
 * @brief Short example on how to scan bluetooth devices and inquire about a specific device
 * @ref https://people.csail.mit.edu/albert/bluez-intro/c404.html
 * @ref http://code.haskell.org/blues/blues/include/hci_lib.h
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>

#define MAX_RSP 255 // maximum of device that will be inquired
#define MAX_NAME_LENGTH 248
#define INQUIRY_LENGTH 8 //max duration of inquiry (multiply by 1.28 for time in seconds)
#define MAX_NB_TRIALS 3

int scan_and_show_bluetooth_devices(int host_id, int sock, inquiry_info *inq_info_array);
int inquire_and_show_selected_bluetooth_devices(int device_id, int host_id, int sock, inquiry_info *inq_info_array);
int ask_user_to_select_device(int num_rsp);

/**
 * int main(int argc, char **argv)
 * @brief Inquire bluetooth neighboring devices, display info, ask user
 * 		  for a device of interest and display info for that device
 * @param argc, argv, unused
 * @return EXIT_SUCCESS, EXIT_FAILURE
 */
int main(int argc, char **argv)
{	
	//array of inquiry_info (devices information)
    inquiry_info *inq_info_array = NULL;
    //initializing maximum response and number of response received
    int num_rsp,user_selection=-1;
    //declaring host id, host socket used for communication 
    int host_id, sock;
    

	//setup socket
    host_id = hci_get_route(NULL);
    sock = hci_open_dev( host_id );

	/*check if all good or die*/
    if (host_id < 0 || sock < 0) {
        perror("cannot open socket");
        return EXIT_FAILURE;
    }

	//allow memory for devices info
    inq_info_array = (inquiry_info*)malloc(MAX_RSP * sizeof(inquiry_info));

	// inquery and display device info
	num_rsp = scan_and_show_bluetooth_devices(host_id, sock, inq_info_array);
	
    // check if any device found
    if( num_rsp < 0 ){
		perror("no devices found");
        return EXIT_FAILURE;
	}
	
	// get user selection
    user_selection = ask_user_to_select_device(num_rsp);
    
    // inquire device selected by user
    inquire_and_show_selected_bluetooth_devices(user_selection, host_id, sock, inq_info_array);
    
	// cleanup
    free( inq_info_array );
    close( sock );
    
    return EXIT_SUCCESS;
}

/**
 * int scan_and_show_bluetooth_devices(int host_id, int sock, inquiry_info *inq_info_array)
 * @brief Inquire bluetooth neighboring devices and display info
 * @param host_id, identifies bluetooth host
 * @param sock, opened socket
 * @param inq_info_array, allocated array to store device info
 * @return number of devices found
 */
int scan_and_show_bluetooth_devices(int host_id, int sock, inquiry_info *inq_info_array){
	
	int i;
	char nb_trials= 0;
	// set flag for inquiry
	int flags = IREQ_CACHE_FLUSH;
	
    //init device address and name buffers
    char dev_addr[19] = { 0 };
    char dev_name[MAX_NAME_LENGTH] = { 0 };
	
	//run the inquiry
    int num_rsp = hci_inquiry(host_id, INQUIRY_LENGTH, MAX_RSP, NULL, &inq_info_array, flags);
    
	// run over all devices and display name and address
    for (i = 0; i < num_rsp; i++) {
        
        // get the address
        ba2str(&(inq_info_array[i].bdaddr), dev_addr);
        
        // get the name
        while (hci_read_remote_name(sock, &(inq_info_array[i].bdaddr), MAX_NAME_LENGTH, 
            dev_name, 0) < 0 && nb_trials<MAX_NB_TRIALS){
			nb_trials++;
		}
		
		// check if couldn't get the name
		if(nb_trials==MAX_NB_TRIALS){
			strcpy(dev_name, "[unknown]");
		}
		
		//show in console
        printf("%d: %s  %s\n", i, dev_addr, dev_name);
        fflush(stdout);
    }
	
	return num_rsp;
}

/**
 * int inquire_and_show_selected_bluetooth_devices(int device_id, int host_id, int sock, inquiry_info *inq_info_array)
 * @brief show info about the device
 * @param device_id, device to be displayed
 * @param host_id, identifies bluetooth host
 * @param sock, opened socket
 * @param inq_info_array, allocated array to store device info
 * @return EXIT_SUCCESS/EXIT_FAILURE
 */
int inquire_and_show_selected_bluetooth_devices(int device_id, int host_id, int sock, inquiry_info *inq_info_array){
	
	int nb_trials = 0;
    //init device address and name buffers
    char dev_addr[19] = { 0 };
    char dev_name[MAX_NAME_LENGTH] = { 0 };
    
    // check that array is initialized
	if(!inq_info_array){
		printf("Scan devices first");
		return EXIT_FAILURE;
	}
	
	// show information about that device
    ba2str(&(inq_info_array[device_id].bdaddr), dev_addr);
    while(hci_read_remote_name(sock, &(inq_info_array[device_id].bdaddr), MAX_NAME_LENGTH, 
            dev_name, 0) < 0 && nb_trials<MAX_NB_TRIALS){
		nb_trials++;
	}

	// check if couldn't get the name
	if(nb_trials==MAX_NB_TRIALS){
		strcpy(dev_name, "[unknown]");
	}
	
	
    printf("The device %d is chosen, \naddress is: %s\nname is: %s\n", device_id, dev_addr, dev_name);
	
	return EXIT_SUCCESS;
}

/**
 * int ask_user_to_select_device(int num_rsp)
 * @brief ask user for a device id, loop until valid
 * @param num_rsp, number of devices found
 * @return user selection
 */
int ask_user_to_select_device(int num_rsp){
	
	int user_selection = -1;
	char* buf[MAX_NAME_LENGTH];
	
	// ask user to pick a device of interest
    while(user_selection<0 || user_selection>(num_rsp-1)){
		printf("\nChoose your device by number:\n");
		if(!scanf("%d",&user_selection)){
			//flush and start again
			fgets(buf,MAX_NAME_LENGTH,stdin);
		}
	}
	
	return user_selection;
}

