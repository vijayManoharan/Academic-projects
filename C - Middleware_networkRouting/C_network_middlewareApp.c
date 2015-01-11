/* 
 * File:   main.c
 * Author: vijay manoharan
 * Manohara@buffalo.edu
 *
 * Created on April 24, 2014, 12:45 PM
 */


//header files
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <net/if.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <ifaddrs.h>
#define max 9999

/*
 * structure used in the program
 */
struct SERVERINFO {
    unsigned short int serverid;
    unsigned char ip[50];
    unsigned short int port;

};//server details

struct distance_vector {
    int connected;
    unsigned short int cost;
    int next_hopid;
    int timeout_count;
};//routing table

struct field {
    unsigned short int oxoo;
    unsigned short int id;
    unsigned short int cost;
    unsigned short int port;
    unsigned char ipadd[5];
};//message format

struct MSG {
    short int No_update_fields;
    unsigned short int server_port;
    unsigned char server_ip[5];
    struct field server[5];
};//message format

struct DVec {
    unsigned short int nexthop[5];
    unsigned short int cost[5];
};

struct DVec Dvector;
struct SERVERINFO servers[5], REC_SERVER;
struct distance_vector dv[5], localDV;


//function declaration
void MYIP();
void UDP_SOCKET();
void send_dv(struct SERVERINFO, char *);
void create_message();
void construct_costmatrix();
void hopcheck(int);

//server details
int myid;
int myport;
char myip[20];


int nexthop[5];
int interval;

//filename of the file
char filename[200];
int no_node = 0;
int no_neighbouring_nodes = 0;


//data structure for the server information
int cost_matrix[5][5];
int dvcost_init[5];
int timerEvent;






//main function.

int main(int argc, char** argv) {

    //part 1 read the content of the file first
    //part 2 make a rounting table based on the content of the file
    //create a socket to send out the routing information of the file to every other node that is connected.
    //recieve the routing information from other nodes.
    //execute the commands.
    int check_interval, flag_parsed = 0, i, j, id, count = 0;
    MYIP();
    if (argc != 5) {
        printf("invalid command\n See discription:\nEnter the command as ./server -t <topology-file-name> -i <routing-update-interval> \n");
    } else {
        //to parse the command input 
        if (strcasecmp(argv[1], "-t") == 0) {
            //file is parsed here
            flag_parsed = fileparser(argv[2]);
            if (flag_parsed == 1) {
                printf("File parsed succesfully\n");

            } else if (flag_parsed == 2) {
                printf("invalid file name\n");
                return;
            } else if (flag_parsed == 3) {
                printf("invalid value for the number of nodes\n");
                return;
            } else if (flag_parsed == 4) {
                printf("invalid value for the number of neighboring nodes\n");
                return;
            } else if (flag_parsed == 5) {
                printf("Mismatch in the number of entries in the file\n");
                return;
            } else if (flag_parsed == 6) {
                printf("invalid server details\n");
                return;
            } else {
                printf("Invalid file content\n");
                return;

            }

        }
        
        if (strcasecmp(argv[3], "-i") == 0) {

            check_interval = is_number(argv[4]);
            if (check_interval == 1) {
                interval = atoi(argv[4]);
                printf("interval: %s\n", argv[4]);

                for (i = 0; i < no_node; i++) {
                    if (dv[i].connected == 1) {
                        dv[i].timeout_count = 3 * interval;
                    } else {
                        dv[i].timeout_count = 1000;
                    }

                }

                dv[getindex(myid)].timeout_count = interval;


            } else {
                printf("invalid value for interval\n");
                return;
            }




        } else {
            printf("invalid command\n See description:\nEnter the command as C or S followed by the port number \n C <port no>- Client\nS <port no> -Server\nExample : S 1234\n");
            return;
        }
    }

    for (i = 0; i < no_node; i++) {

    }

    UDP_SOCKET();


    return 0;

    //so far file read and contents are good.


    return (EXIT_SUCCESS);
}

/*Backbone of the program
 * sends the message 
 * receives message
 * event handles
 * takes action on command from user. 
 */
void UDP_SOCKET() {
    printf("at updsocket\n");
    int sockfd, fdmax = 0, temp_sock = 0, dvupdate_count = 0;
    //FD used 
    fd_set readfds;
    fd_set master;

    //socket address
    struct sockaddr_in serveraddr;
    //client addr
    struct sockaddr_in clientaddr;
    struct hostent *hostp;
    char *hostaddrp;
    socklen_t clientaddlen = sizeof (clientaddr);


    //buffer for receive messages
    char buf[1024];
    struct MSG rev_msg;


    //to store return value of select 
    int selc_return;

    //miscellaneous
    int i, optval, n, d, j;
    char recip[20];
    char cmd[100];
    int id, id2, idsr, iddt;

    //timer 
    struct timeval tv;
    int mintime = 1000;
    int minindx;
    int timer = interval;

    //variable for string tok, breaking the string operations
    int k;
    char * temp;
    char temp12[100];
    char str[10][100];


    // variable for disable command
    int disable_id;
    int flag_dis = 0;

    //variables for update command
    unsigned short int upd_cost = 0;
    int flag_uc = 0;

    //declaration over

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
        perror("ERROR opening socket");

    optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void *) &optval, sizeof (int));


    bzero((char *) &serveraddr, sizeof (serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("myport is%d\n", myport);
    serveraddr.sin_port = htons(myport);

    if (bind(sockfd, (struct sockaddr *) &serveraddr, sizeof (serveraddr)) < 0)
        perror("ERROR on binding");

    clientaddlen = sizeof (clientaddr);
    FD_ZERO(&master);
    FD_ZERO(&readfds);
    FD_SET(fileno(stdin), &master);
    FD_SET(sockfd, &master);
    fdmax = sockfd;
    tv.tv_sec = interval;
    tv.tv_usec = 0;

    if (temp_sock != 0) {
        if (temp_sock > fdmax) {
            FD_SET(temp_sock, &master);
            fdmax = temp_sock;
            fflush(stdout);
            temp_sock = 0;
        }
    }/*
     ***************************************************************************************************************************
      events are handled here
      *?
      */
    timerEvent = interval;
    for (;;) {
        mintime = 1000;
        printf("\nServer%d>>\n\t", myid);
        fflush(stdout);
        readfds = master;
        selc_return = select(fdmax + 1, &readfds, NULL, NULL, &tv);
        if (selc_return == -1) {
            perror("select");
            continue;
        } else if (selc_return == 0) {
            //{ interval, 0 };
            /*send the packets periodically
             * make the packet.
             * send it only to the neighbors nodes
             */

            /*call message maker function
             * once the message is created
             * call the send message with the server details and the message
             * also find the next time and set to select timer interrupt
             */
            id = getindex(myid);

            printf("SERVER%d>>\tInterrupt time event \n", myid);

            for (i = 0; i < no_node; i++) {
                if ((dv[i].connected == 1)) {
                    dv[i].timeout_count = dv[i].timeout_count - timerEvent;
                }
                if (i == id) {

                    dv[i].timeout_count = dv[i].timeout_count - timerEvent;
                }
            }

            //did not hear from %d =i server for 3 time interval.
            for (i = 0; i < no_node; i++) {
                if ((dv[i].timeout_count == 0)&&(i != id)) {
                   printf("\nSERVER%d>>time out for %d \n", myid, servers[i].serverid);

                    dv[i].timeout_count = 1000;
                    dv[i].cost = max;
                    dv[i].connected = 0;
                    cost_matrix[id ][i] = max;
                    DV_function();
                }

            }

            //reset parent server's timer. for sending messages
            if ((dv[id].timeout_count >= 0)&&(dv[id].timeout_count <= 1)) {
                DV_function();
                float intv = interval;
                dv[id].timeout_count = intv;
                create_message();

            }
            //finding the next interrupt to be called at 
            for (i = 0; i < no_node; i++) {
           //     printf("SERVER%d>>timer for %d is %d\n", myid, servers[i].serverid, dv[i].timeout_count);
                if ((mintime > dv[i].timeout_count)) {


                    mintime = dv[i].timeout_count;
                    minindx = i;

                }
            }
            //setting the timer to next event's time.
           //  printf("next timer at %d for sevr%d   %d", mintime, servers[minindx].serverid, interval);
            timerEvent = mintime;
            tv.tv_sec = timerEvent;
            tv.tv_usec = 0;


        }/*
     ***************************************************************************************************************************
/*time out part over.
         * listen for incoming messages.
         *stdin 
         */
        else {
            /* part1 receive incoming messages
             * part2 commands from the stdin.
             * 
             */

            /*Part 1 Receive routing message
             */

            if (FD_ISSET(sockfd, &readfds)) {
                memset(buf, 0x0, 1024);
                printf("Recieved message\n");
                bzero(buf, 1024);
                n = recvfrom(sockfd, buf, 1024, 0, (struct sockaddr *) &clientaddr, &clientaddlen);
                if (n < 0)
                    perror("ERROR:\n\tThe function recvfrom did not receive properly\n");


                strcpy(REC_SERVER.ip, inet_ntoa(clientaddr.sin_addr));

                //here is the part to do with the dv received 
                memcpy(&rev_msg.No_update_fields, buf, 2);
                memcpy(&rev_msg.server_port, buf + 2, 2);
                memcpy(&rev_msg.server_ip[1], buf + 4, 1);
                memcpy(&rev_msg.server_ip[2], buf + 5, 1);
                memcpy(&rev_msg.server_ip[3], buf + 6, 1);
                memcpy(&rev_msg.server_ip[4], buf + 7, 1);
                j = 8;
                for (i = 0; i < no_node; i++) {

                    memcpy(&rev_msg.server[i].ipadd[1], buf + j, 1);
                    j++;
                    memcpy(&rev_msg.server[i].ipadd[2], buf + j, 1);
                    j++;
                    memcpy(&rev_msg.server[i].ipadd[3], buf + j, 1);
                    j++;
                    memcpy(&rev_msg.server[i].ipadd[4], buf + j, 1);
                    j++;
                    memcpy(&rev_msg.server[i].port, buf + j, 2);
                    j = j + 2;
                    memcpy(&rev_msg.server[i].oxoo, buf + j, 2);
                    j = j + 2;
                    memcpy(&rev_msg.server[i].id, buf + j, 2);
                    j = j + 2;
                    memcpy(&rev_msg.server[i].cost, buf + j, 2);
                    j = j + 2;

                }

                sprintf(temp12, "%d", rev_msg.server_ip[1]);
                strcpy(recip, temp12);
                for (i = 2; i < 5; i++) {
                    strcat(recip, ".");
                    sprintf(temp12, "%d", rev_msg.server_ip[i]);
                    strcat(recip, temp12);

                }
                
                //Change the timeout for all the nodes and reset for the sender server.
                timerEvent = timerEvent - tv.tv_sec;

                mintime = 1000;

                REC_SERVER.port = rev_msg.server_port;
                for (i = 0; i < no_node; i++) {
                    if ((strcmp(servers[i].ip, REC_SERVER.ip) == 0)&&(servers[i].port == REC_SERVER.port)) {
                        REC_SERVER.serverid = servers[i].serverid;
                        dv[i].timeout_count = 3 * interval;

                        printf("\n\tServer %d timeout count reset\n", servers[i].serverid);
                    }
                }
                for (i = 0; i < no_node; i++) {
                    if ((i != getindex(REC_SERVER.serverid))&&(dv[i].connected == 1)) {
                        dv[i].timeout_count = dv[i].timeout_count - timerEvent;
                    }

                    if (i == getindex(myid)) {

                        dv[i].timeout_count = dv[i].timeout_count - timerEvent;
                    }
                }
                //find next timeout event

                for (i = 0; i < no_node; i++) {
                  //    printf("the time count %d is %d\n",servers[i].serverid,dv[i].timeout_count);
                    if ((mintime > dv[i].timeout_count)) {


                        mintime = dv[i].timeout_count;
                        minindx = i;

                    }



                }
//set that to the select timeout.
                // printf("next timer at %d for sevr%d   %d", mintime, servers[minindx].serverid, interval);
                timerEvent = mintime;
                tv.tv_sec = timerEvent;
                tv.tv_usec = 0;


                if (dv[getindex(REC_SERVER.serverid)].connected != 2) {
                    printf("\n\tRECEIVED A MESSAGE FROM SERVER <server-ID> server- %d\n", REC_SERVER.serverid);
                    dvupdate_count++;
                    for (i = 0; i < no_node; i++){
                    if(dv[getindex(REC_SERVER.serverid)].connected==0)
                        cost_matrix[i][getindex(REC_SERVER.serverid)] = rev_msg.server[i].cost;
                    }
                    dv[getindex(REC_SERVER.serverid)].connected = 1;
                    
                } else {
                    printf("\t\t Discard the message from server-%d  \n", REC_SERVER.serverid);
                }
                for (i = 0; i < no_node; i++) {
                    //cost matrix from neighbor nodes.

                    cost_matrix[getindex(REC_SERVER.serverid)][i] = rev_msg.server[i].cost;


                }

                DV_function();

            }

            /*/*
             ***************************************************************************************************************************

             * command from STDIN is handled from here. 
             */

            if (FD_ISSET(fileno(stdin), &readfds)) {

                fgets(cmd, 1000, stdin);
                if (cmd[strlen(cmd) - 1] == '\n')
                    cmd[strlen(cmd) - 1] = 0;
                switch (cmd[0]) {

                        /*
                         * crash
                         */
                    case 'C':
                    case 'c':
                        if ((word_count(cmd) == 1)&&(space_count(cmd) == 0)) {
                            if (strcasecmp(cmd, "crash") == 0) {
                                printf("System crash Success\n");
                                for (;;) {
                                    //printf("System is down\n");
                                }
                            } else {
                                printf("Error:\n\tInvalid command, Please refer the HELP command\n");
                                break;
                            }
                        } else {
                            printf("Error:\n\tInvalid command, Please refer the HELP command\n");
                        }

                        break;
                        /*
                         * display
                         */
                    case 'D':
                    case 'd':
                        flag_dis = 0;
                        //command for display.
                        if ((word_count(cmd) == 1)&&(space_count(cmd) == 0)) {
                            if (strcasecmp(cmd, "display") == 0) {
                               
                                i = getindex(myid);
                                printf(" for router %d\n", servers[i].serverid);
                                printf("\n\tdestination-server-ID  next-Hop  Cost-of-the-path  \n\n");
                                for (j = 0; j < no_node; j++) {
                                    if(Dvector.cost[j]!=max)
                                    printf("\t\t%d \t\t %d \t\t %d \n", servers[j].serverid, Dvector.nexthop[j], Dvector.cost[j]);
                                    else
                                         printf("\t\t%d \t\t %d \t\t inf \n", servers[j].serverid, Dvector.nexthop[j]); 
                                }


                                printf("\nDisplay SUCCESS\n");
                            } else {
                                printf("Error:\n\tInvalid command, Please refer the HELP command\n");
                                break;
                            }

                        }/*
                             * disable
                             */
                        else if ((word_count(cmd) == 2)&&(space_count(cmd) == 1)) {
                            k = 0;
                            temp = strtok(cmd, " ");


                            strcpy(str[k], temp);
                            while ((temp != NULL)&&(k < 2)) {
                                k++;
                                strcpy(str[k], temp);
                                temp = strtok(NULL, " ");
                            }

                            if (strcasecmp(str[0], "disable") != 0) {
                                printf("Error:\n\tInvalid command, HERE please have a look the read me by typing HELP as the command\n ");
                                break;
                            }
                            disable_id = atoi(str[2]);
                            for (i = 0; i < no_node; i++) {

                                if (disable_id == servers[i].serverid) {
                                    if (dv[i].connected == 1) {

                                        dv[i].cost = max;


                                        dv[i].connected = 2;


                                        for (j = 0; j < no_node; j++) {
                                            cost_matrix[i ][j] = max;
                                        }
                                        id = getindex(myid);
//      
                                        cost_matrix[id ][i] = max;

                                        DV_function();
                                        flag_dis = 1;
                                        printf("Disabled the link to server %d \n", servers[i].serverid);
                                    }
                                }
                            }
                            if (!flag_dis) {
                                printf("Disable Error:\n\tInvalid server-id to disable\n");
                            } else
                                printf("\tDisable SUCCESS\n");
                        } else {
                            printf("Error:\n\tInvalid command, Please refer the HELP command\n");
                            break;
                        }

                        break;
                        /*
                         * help
                         */
                    case 'H':
                    case 'h':
                        if ((word_count(cmd) == 1)&&(space_count(cmd) == 0)) {
                            if (strcasecmp(cmd, "help") == 0) {
                                printf("Please follow the syntax of the commands as\n");
                                printf("1.Crash:\n\tcrash or CRASH\n");
                                printf("\tOutput : \t<<Serverid>> crashed\n");
                                printf("\tCrash SUCCESS\n");

                                printf("2.Display:\n\tdisplay or DISPLAY\n");
                                printf("\tDisplay SUCCESS\n");

                                printf("3.Disable:\n\t(Disable or DISABLE) <<server id>>\n");
                                printf("Example: \n\tDisable 2\n");
                                printf("\tOutput : \tThe link from the <<source-id>> to 2 is disabled\n");
                                printf("\tDisable SUCCESS\n");


                                printf("4.Packets:\n\tpackets or PACKETS\n");
                                printf("\tOutput : \tThe number of routing information received so far is 100\n");
                                printf("\tPackets SUCCESS\n");

                                printf("5.Step:\n\tstep or STEP\n");
                                printf("\tOutput : \tSending rounting info to <<server-id>>\n");
                                printf("\tSTEP SUCCESS\n");

                                printf("3.Update:\n\t(update or UPDATE) <<server source>><<server destination>><<cost>>\n");
                                printf("Example: \n\tupdate 1 2 10\n\tupdate 1 2 inf\n");
                                printf("\tOutput : \tupdated the cost of link 1->2 to 10\n");
                                printf("\tUpdate SUCCESS\n");



                            } else {
                                printf("Error:\n\tInvalid command, Please refer the HELP command\n");
                                break;
                            }
                        } else {
                            printf("Error:\n\tInvalid command, Please refer the HELP command\n");
                        }

                        break;
                        /*
                         * packets
                         */
                    case 'P':
                    case 'p':
                        if ((word_count(cmd) == 1)&&(space_count(cmd) == 0)) {
                            if (strcasecmp(cmd, "packets") == 0) {
                                //print the number of packets
                                printf("\t\tThe number of packets are %d\n", dvupdate_count);
                                printf("Packets SUCCESS\n");
                            } else {
                                printf("Error:\n\tInvalid command, Please refer the HELP command\n");
                                break;
                            }
                            dvupdate_count = 0;
                        } else {
                            printf("Invalid command, Please refer the HELP command\n");
                        }

                        break;
                        /*
                         * step
                         */
                    case 'S':
                    case 's':
                        if ((word_count(cmd) == 1)&&(space_count(cmd) == 0)) {
                            if (strcasecmp(cmd, "step") == 0) {
                                //send the packets
                                create_message();
                                printf("Step SUCCESS\n");
                            } else {
                                printf("Error:\n\tInvalid command, Please refer the HELP command\n");
                                break;
                            }
                        } else {
                            printf("Error:\n\tInvalid command, Please refer the HELP command\n");
                        }

                        break;
                        /*
                         * update
                         */
                    case 'U':
                    case 'u':
                        if ((word_count(cmd) == 4)&&(space_count(cmd) == 3)) {
                            flag_uc = 0;
                            k = 0;
                            temp = strtok(cmd, " ");


                            strcpy(str[k], temp);
                            while ((temp != NULL)&&(k < 4)) {
                                k++;
                                strcpy(str[k], temp);
                                temp = strtok(NULL, " ");
                            }

                            if (strcasecmp(str[0], "update") != 0) {
                                printf("Error:\n\tInvalid command, HERE please have a look the read me by typing HELP as the command\n ");
                                break;
                            }
                            /*
                             * update to inf
                             */
                            if (strcasecmp(str[4], "inf") == 0) {
                                upd_cost = max;
                            }/*
                                 * update to cost
                                 */
                            else if (is_number(str[4])) {
                                upd_cost = atoi(str[4]);
                                if (upd_cost < 1) {
                                    printf("Update Error:\n\tCost value cannot be ZERO!\n");
                                    break;
                                }

                            } else {
                                printf("Update Error:\n\tCost value is invalid\n");
                                break;
                            }
                            for (i = 0; i < no_node; i++) {
                                if (servers[i].serverid == atoi(str[2])) {
                                    flag_uc++;
                                }
                                if (servers[i].serverid == atoi(str[3])) {
                                    flag_uc++;
                                }
                            }
                            if (flag_uc == 2) {
                                if (atoi(str[2]) == myid) {
                                    id = getindex(myid);
                                    idsr = getindex(atoi(str[2]));
                                    iddt = getindex(atoi(str[3]));
                                    if (dv[iddt].connected == 1) {
                                        //resetCM(myid - 1, atoi(str[3]) - 1, upd_cost);
                                        for (j = 0; j < no_node; j++) {
                                            cost_matrix[myid - 1 ][j] = dvcost_init[j];
                                        }
                                        cost_matrix[ idsr][ iddt] = upd_cost;

                                        
                                        if (upd_cost == max)dv[iddt].connected = 0;
                                        else dv[iddt].connected = 1;
                                     
                                    } else {
                                        printf("Update Error:\n\tThere is no direct link between server %d and server %d\n", idsr + 1, iddt + 1);
                                        break;
                                    }
                                   
                                } else {
                                    printf("Update Error:\n\tThe entered server id are invalid\n");
                                    break;
                                }

                                DV_function();

                            } else {
                                printf("Update Error:\n\t Server-id of source or destination is invalid sr %s dst %s\n", str[2], str[3]);
                                break;
                            }
                            printf("\n\t The link cost between %s and %s is updated to %d \n", str[2], str[3], upd_cost);
                            printf("Update SUCCESS\n");

                        } else {
                            printf("Error:\n\tInvalid command, Please refer the HELP command\n");
                        }
                        break;

                    default:
                        printf("Invalid command, DEFAULT please have a look the read me by typing HELP as the command \n");

                        break;
                }
            }
        }
    }
}

//this keeps a structure of the message.
//the function is referred from beej guide.

void send_dv(struct SERVERINFO server, char *message) {
    //  printf("at senddv\n");
    int sockfd, rc;
    struct sockaddr_in serveraddr;
    struct hostent *hp;
    // char message[1024];
    int serveraddrlen = sizeof (serveraddr);
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Error:\n\tUDP Client - socket() error");
        exit(-1);
    }

    memset(&serveraddr, 0, sizeof (struct sockaddr_in));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr(server.ip);
    serveraddr.sin_port = htons(server.port);

    hp = gethostbyname(server.ip);
    if (!hp) {
        fprintf(stderr, "could not obtain address of %s\n", server.ip);
        return;
    }


    memcpy(&serveraddr.sin_addr, hp->h_addr, sizeof (serveraddr.sin_addr));
    //this is where i convert the message and send.


    if (sendto(sockfd, message, 1024, 0, (struct sockaddr *) &serveraddr, serveraddrlen) < 0) {

        perror("Error:\n\tSending failed");
        close(sockfd);
        exit(-1);
    }
    close(sockfd);

}

//makes the message here, and call send_dv to send the message.

void create_message() {
    //have to make message content here.
    int i, k, j, h;
    char str[20][200];
    char *temp1;
    struct MSG message;
    char ip[20];
    unsigned char temp[68];
    k = 0;
    temp1 = strtok(myip, ".");
    strcpy(str[k], temp1);
    // printf("at creating message\n");
    while ((temp1 != NULL)&&(k <= 4)) {
        k++;
        strcpy(str[k], temp1);
        temp1 = strtok(NULL, ".");

    }
    for (i = 1; i <= 4; i++) {
        message.server_ip[i] = atoi(str[i]);

    }

    message.server_port = myport;
    message.No_update_fields = 3;
    for (i = 0; i < no_node; i++) {

        message.server[i].port = servers[i].port;
        message.server[i].id = servers[i].serverid;
        message.server[i].cost = dv[i].cost;
        message.server[i].oxoo = 00;
        message.No_update_fields = message.No_update_fields + 5;

    }

    for (i = 0; i < no_node; i++) {

        k = 0;


        strcpy(ip, servers[i].ip);
        temp1 = strtok(ip, ".");
        strcpy(str[k], temp1);
        while ((temp1 != NULL)&&(k <= 4)) {
            k++;
            strcpy(str[k], temp1);
            temp1 = strtok(NULL, ".");

        }
        for (j = 1; j <= 4; j++) {
            char b = atoi(str[j]);
            strcpy(&message.server[i].ipadd[j], &b);

        }

    }

    /*
     message formating is done here.
     * message is created and sent to all the neighbors
     */

    memcpy(temp, &message.No_update_fields, 2);
    memcpy(temp + 2, &message.server_port, 2);
    memcpy(temp + 4, &message.server_ip[1], 1);
    memcpy(temp + 5, &message.server_ip[2], 1);
    memcpy(temp + 6, &message.server_ip[3], 1);
    memcpy(temp + 7, &message.server_ip[4], 1);
    j = 8;
    
    //so far 8bytes are in temp
    for (i = 0; i < no_node; i++) {
        //  printf("j======%d",j);


        temp[j] = message.server[i].ipadd[1];
        j++;
        temp[j] = message.server[i].ipadd[2];
        j++;
        temp[j] = message.server[i].ipadd[3];
        j++;
        temp[j] = message.server[i].ipadd[4];
        j++;
        memcpy(temp + j, &message.server[i].port, 2);
        j = j + 2;
        memcpy(temp + j, &message.server[i].oxoo, 2);
        j = j + 2;
        memcpy(temp + j, &message.server[i].id, 2);
        j = j + 2;
        memcpy(temp + j, &message.server[i].cost, 2);
        j = j + 2;
        
        //plus 12 bytes for every node
    }




    for (j = 0; j < no_node; j++) {
        /*check for the neighboring nodes
         * send the routing info only to it
         */
        if (dv[j].connected != 2) {

            printf("sending rounting info to %d \n", servers[j].serverid);
            //to send temp to server[j].serverid
            send_dv(servers[j], temp);
        }








    }


}
//checks whether the given char is number or not

int is_number(const char *s) {
    while (*s) {

        if (isdigit(*s++) == 0) return 0;
    }

    return 1;
}

//function for file parsing

int fileparser(char Filecontent[200]) {
    char source[5000];
    char s[100][200], s1[100][200];
    char str[20][200];
    char *temp, *temp1;
    int ipcheck, flag_myid, flag_nn;
    size_t len = 0;
    ssize_t read;
    int id;
    int i = 0, lines = 0, k = 0, c = 0, d = 0;
    printf("Topology filename is: %s\n", Filecontent);

    //file check is done here. 

    FILE *fp = fopen(Filecontent, "r");
    if (fp != NULL) {
        /* Go to the end of the file. */
        if (fseek(fp, 0L, SEEK_END) == 0) {
            /* Get the size of the file. */
            long bufsize = ftell(fp);
            if (bufsize == -1) {
                /* Error */
            }

            /* Allocate our buffer to that size. */
            // source = malloc(sizeof (char) * (bufsize + 1));

            /* Go back to the start of the file. */
            if (fseek(fp, 0L, SEEK_SET) == 0) {
                /* Error */

            }

            /* Read the entire file into memory. */
            size_t newLen = fread(source, sizeof (char), bufsize, fp);
            if (newLen == 0) {
                fputs("Error: file parsing \n\tError reading file", stderr);
            } else {
                source[++newLen] = '\0'; /* Just to be safe. */
            }
        }
        fclose(fp);
        printf("the content of the file is \n\n--------------------------------------------\n\t%s\n--------------------------------------------\n%s\n", Filecontent, source);


    } else {
        printf("Error: file parsing \n\tFile not found\n");
    }
    for (i = 0; i < strlen(source); i++) {
        if (source[i] == '\n') {

            lines++;
        }
    }

    //breaking the content of the line into array by lines.
    printf("\tnumber of lines %d\t filesize : %d \n", lines, strlen(source));
    temp = strtok(source, "\n");
    strcpy(s[k], temp);
    while ((temp != NULL)&&(k <= lines)) {
        k++;
        strcpy(s[k], temp);
        temp = strtok(NULL, "\n");
    }

    for (i = 0; i <= lines; i++) {
        k = 0;
        while (s[i][k] == ' ') {
            k++;
        }
        for (d = 0; d < strlen(s[i]); d++) {
            s[i][d] = s[i][d + k];
        }
        k = strlen(s[i]);
        while (s[i][k - 1] == ' ') {
            k--;
        }

        s[i][ k] = '\0';

    }

    //checking the contents are all valid or not.

    if ((word_count(s[0]) == 1) && (is_number(s[0]))) {
        no_node = atoi(s[1]);


    } else {
        printf("Error: file parsing \n\tnot a number no_nodes\n");
        return 3;
    }
    if ((word_count(s[0]) == 1) && (is_number(s[1]))) {
        no_neighbouring_nodes = atoi(s[2]);

    } else {
        printf("Error: file parsing \n\tnot a number no_Nnodes\n");
        return 4;
    }

    if ((no_neighbouring_nodes + no_node != lines - 2)&&((no_neighbouring_nodes + no_node != lines - 3))) return 5;
    printf("File parsing: \n\tthe number of nodes are %d and number connections are %d\n", no_node, no_neighbouring_nodes);

    //from here we extract the file contents
    for (i = 3; i < no_node + 3; i++) {
        if (word_count(s[i]) != 3)
            return 6;

        k = 0;
        //  free(temp);


        temp = strtok(s[i], " ");
        strcpy(str[k], temp);

        while ((temp != NULL)&&(k <= 4)) {
            k++;
            strcpy(str[k], temp);
            temp = strtok(NULL, " ");

        }
        if ((is_number(str[3]) == 0)) {
            printf("Error: file parsing \n\tport number is not valid for %d\n", i - 2);
            return 6;
        } else {
            servers[i - 3].port = atoi(str[3]);
        }
        if (is_number(str[1]) == 0) {
            printf("Error: file parsing \n\tinvalid id for %d\n", i - 2);
            return 6;
        } else {
            servers[i - 3].serverid = atoi(str[1]);
        }

        strcpy(servers[i - 3].ip, str[2]);


    }

    for (i = 0; i < no_node; i++) {
        for (c = 0; c < no_node; c++) {
            if ((strcmp(servers[i].ip, servers[c].ip) == 0)&&(i != c)) {
                if (servers[i].port == servers[c].port) {
                    printf("Error: file parsing \n\tDuplicate server details. %d and %d are same\n", i, c);
                    return 6;
                }
            }
        }

    }
    printf("-------------------------------\nthe server details are \n----------------------------------\nid\t     ip    \tport\n");
    ipcheck = checkipaddress();
    if (ipcheck != 10) {
        printf("Error: file parsing \n\tip address for connection %d is not valid\n", ipcheck + 1);
        return 6;
    }

    //parsing the neighbor nodes details.
    for (i = 3; i < no_node + 3; i++) {
        dv[i - 3].connected = 2;
        dv[i - 3].cost = max;
        dv[i - 3].timeout_count = 0;
        dv[i - 3].next_hopid = 0;
        printf("%d\t%s\t%d\n", servers[i - 3].serverid, servers[i - 3].ip, servers[i - 3].port);
    }
    for (i = 3 + no_node; i < 3 + no_node + no_neighbouring_nodes; i++) {
        flag_nn = 0;
        flag_myid = 0;
        if (word_count(s[i]) != 3)
            return 7;
        k = 0;
        temp = strtok(s[i], " ");
        strcpy(str[k], temp);

        while ((temp != NULL)&&(k <= 4)) {
            k++;
            strcpy(str[k], temp);
            temp = strtok(NULL, " ");

        }

        if (is_number(str[1]) == 0) {
            printf("Error: file parsing \n\tinvalid id for %d\n", i - (2 + no_node));
            return 7;
        } else {
            if (myid == 0) {
                myid = atoi(str[1]);
            } else {
                if (myid != atoi(str[1])) {
                    printf("Error: file parsing \n\tmismatch of server id\n");
                    return 7;
                }
            }

            construct_costmatrix();
            for (c = 0; c < no_node; c++) {
                if (atoi(str[1]) == servers[c].serverid) {
                    dv[c].cost = 0;
                    if (strcmp(servers[c].ip, myip) == 0)
                        flag_myid = 1;
                    myport = servers[c].port;
                    id = getindex(myid);
                    cost_matrix[id][c] = dv[c].cost;


                }
            }

        }
        if (is_number(str[2]) == 0) {
            printf("Error: file parsing \n\tinvalid id for %d\n", i - (2 + no_node));
            return 7;
        } else {
            if ((is_number(str[3]) == 0)) {
                
                printf("Error: file parsing \n\tcost not valid for %d\n", i - (2 + no_node));
                return 7;
            }

            if(atoi(str[3])<=0){
                  printf("Error: file parsing \n\tcost Cannot be ZERO for %d\n", i - (2 + no_node));
                return 7;
            }
            //cost is initialized here
            for (c = 0; c < no_node; c++) {
                if (atoi(str[2]) == servers[c].serverid) {
                    
                    dv[c].cost = atoi(str[3]);
                    dv[c].connected = 1;
                    flag_nn = 1;
                    dvcost_init[c] = dv[c].cost;

                }
            }
        }
        if ((flag_myid != 1) || (flag_nn != 1)) {
            if (flag_myid == 1)
                printf("Error: file parsing \n\tinvalid neighbor id at line %d\n", i);
            else
                printf("Error: file parsing \n\tinvalid server id at line %d\n", i);

            return 7;
        }
    }
    //making the initial cost matrix
    for (i = 0; i < no_node; i++) {
        //Dvector[myid - 1].cost[i] = dv[i].cost;

        cost_matrix[id][i] = dv[i].cost;
        dvcost_init[i] = dv[i].cost;


    }



    DV_function();
    printf(" distant vector for id %d\n", myid);
    printf("id\tconnected\tcost\n");
    for (i = 0; i < no_node; i++) {
        //if (dv[i].cost != 0)

        printf("%d          %d          %d\n", i + 1, dv[i].connected, dv[i].cost);
    }


    return 1;
}
//distance vector algorithm

DV_function() {


    int id, i, j, k, count = 0;

    id = getindex(myid);

    //initialize the DVector to the servers cost matrix
    for (j = 0; j < no_node; j++) {

        Dvector.cost[j] = cost_matrix[id][j]; //initialise the distance equal to cost matrix

        Dvector.nexthop[j] = servers[j].serverid;
    }

    /*bellman for equation starts here.
     ***************************************************************************************************************************
    /*We choose arbitary vertex k and we calculate the direct distance from the node i to k using the cost matrix
     *and add the distance from k to node j
     */
    for (i = 0; i < no_node; i++)
        for (j = 0; j < no_node; j++)
            for (k = 0; k < no_node; k++)
                if (dv[k].connected == 1)
                    if (Dvector.cost[j] > cost_matrix[id][k] + cost_matrix[k][j]) {//We calculate the minimum distance
                        Dvector.cost[j] = cost_matrix[id][k] + cost_matrix[k][j];
                        Dvector.nexthop[j] = servers[k].serverid;

                    }



    for (i = 0; i < no_node; i++) {

        dv[i].cost = Dvector.cost[i];

    }


}
//the function returns the word count for the given string.

int word_count(const char command[ ]) {
    int counted = 0, sp_count = 0; // result


    const char* cpointer = command;
    int inword = 0;

    do switch (*cpointer) {
            case '\0':
            case ' ':

            case '\t': case '\n': case '\r': // TODO others?
                if (inword) {
                    inword = 0;
                    counted++;
                }
                break;
            default: inword = 1;
        } while (*cpointer++);

    //printf("count is %d\n", counted);

    return counted;
}
//the function to the ip address is correct of not.

int checkipaddress() {

    int count = 0, i;
    char str[20][200];
    int a[10];
    char *temp;
    int k, j;

    for (i = 0; i < no_node; i++) {
        count = 0;
        if (strcmp(servers[i].ip, "255.255.255.255") == 0) {
            printf("Error: file parsing \n\tinvalid ip\n");
            return i;
        }
        if (strcmp(servers[i].ip, "0.0.0.0") == 0) {
            printf("Error: file parsing \n\tinvalid ip\n");
            return i;
        }
        char server[20];
        strcpy(server, servers[i].ip);
        const char* cpointer = server;
        do switch (*cpointer) {
                case '.':
                    count++;
                    break;

            } while (*cpointer++);

        if (count != 3) {
            return i;
        }
        k = 0;
        temp = strtok(server, ".");
        strcpy(str[k], temp);

        while ((temp != NULL)&&(k <= 4)) {
            k++;
            strcpy(str[k], temp);
            temp = strtok(NULL, ".");

        }

        for (j = 1; j <= 4; j++) {
            //printf("%d %s\t",j,str[j]);
            if (is_number(str[j]) == 1) {
                a[j] = atoi(str[j]);
                //   printf("%d %d\t", j, a[j]);
                if ((a[j] > 255) || (a[j] < 0)) {
                    printf("Error ip address:Number not in range of Ip\n");
                    return i;

                }
            } else {
                printf("not number\n");

                return i;
            }


        }
    }
    return 10;
}


//code for myip from stack overflow

int ip_addresses(const int domain) {
    int s;
    struct ifconf ifconf;
    struct ifreq ifr[50];
    int ifs;
    int i;

    s = socket(domain, SOCK_STREAM, 0);
    if (s < 0) {
        perror("socket");
        return 0;
    }

    ifconf.ifc_buf = (char *) ifr;
    ifconf.ifc_len = sizeof ifr;

    if (ioctl(s, SIOCGIFCONF, &ifconf) == -1) {
        perror("ioctl");
        return 0;
    }

    ifs = ifconf.ifc_len / sizeof (ifr[0]);

    for (i = 0; (i < ifs); i++) {
        char ip[INET_ADDRSTRLEN];
        struct sockaddr_in *s_in = (struct sockaddr_in *) &ifr[i].ifr_addr;

        if (!inet_ntop(domain, &s_in->sin_addr, ip, sizeof (ip))) {
            perror("inet_ntop");
            return 0;
        }
        if (i == 1) {
            //   printf("%s - %s\n I=%d", ifr[i].ifr_name, ip, i);

            strcpy(myip, ip);
            printf("myip is %s\n", myip);
        }
    }
    close(s);

    return 1;
}
//getting the ip address of the server.

void MYIP() {
    int domains[] = {AF_INET};
    int i;

    for (i = 0; i < sizeof (domains) / sizeof (domains[0]); i++)

        if (!ip_addresses(domains[i]))
            printf("ok");


}
//function to initialise the cost matrix.

void construct_costmatrix() {
    int i, j;
    for (i = 0; i < no_node; i++) {
        for (j = 0; j < no_node; j++)

            cost_matrix[i][j] = max;
    }
    for (i = 0; i < no_node; i++) {
        for (j = 0; j < no_node; j++)

            if (i == j)
                cost_matrix[i][j] = 0;

    }

}
//the following function is used to give the count of space between words in a given string.

int space_count(const char command[ ]) {
    int counted = 0, sp_count = 0; // result

    // state:
    const char* cpointer = command;
    int inword = 0;

    do switch (*cpointer) {
            case '\0':
            case ' ': sp_count++;

            case '\t': case '\n': case '\r': // TODO others?
                if (inword) {
                    inword = 0;
                    counted++;
                }
                break;
            default: inword = 1;
        } while (*cpointer++);
    // printf("\nnumber of space is %d \t strlengt %d\t\n", sp_count - 1, strlen(command));

    return (sp_count - 1);
}

//the funciton to the index of the given server

int getindex(int node) {
    int id, i;
    for (i = 0; i < no_node; i++) {
        if (servers[i].serverid == node) {
            return i;
        }
    }
    return id;
}