// OWNER INFO : 
// DATE STARTED : 2
//DATE ENDED: 2-MAR-2014 10'00 pm Buffalo local time.
//
//

/* pr
 * File:   manohara_proj1.c 
 * Author: Vj <VIJAY MANOHARAN>
 * Created on January 7 2014 5'30 PM BUFFALO LOCAL TIME.
 * Completed on March 2 2014 10'00 PM Buffalo, NY local time.
 * references used.
 * beej guide-----> For the select function and read, send messages.
 * www.stackoverflow.com-----> to understand the message passing from client to server and how three way handshake is done.
 * http://www.tutorialspoint.com/cprogramming/c_strings.htm for string functions and operations.
 */
//Header file.
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

int flag, flag_register = 0; //flag to keep information of system type, 0 server and 1 client.
int fdmax; // maximum file descriptor number
int connection_no = 0, connection_no_server = 0;
char host[NI_MAXHOST];
struct sockaddr_in serv_addr;


//the structure holds the information of the connection.

struct connection_info {
    char hostname[100];
    int fd; // Roll Number’s  as Integer
    char ip[20]; // Char means Name as “Character”
    char port[5];
};


//global variables needed
struct connection_info myinfo, info[10], serverinfo;
struct connection_info Client[4];
struct connection_info serverregisterinfo[4];

//function declaratoion
void MYIP(void);
void MYPORT(void);
void LIST(void);

//int clientserver(void);
int SERVER(void);
int CLIENT(void);
int HELP(void);
void OWNER(void);
void RETURN(void);

//main program

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("invalid command\n See discription:\nEnter the command as C or S followed by the port number \n C <port no>- Client\nS <port no> -Server\nExample : S 1234\n");
    } else {
        //to check the program code is a client or server
        if (strcasecmp(argv[1], "S") == 0) {
            printf("$$Server\tPort no : %s\n", argv[2]);
            gethostname(myinfo.hostname, 100);
            myinfo.fd = 0;
            //printf("\nhostname is %s",myinfo.hostname);
            strcpy(myinfo.port, argv[2]);
            flag = 0;
            SERVER();
        } else if (strcasecmp(argv[1], "C") == 0) {
            printf("$$Client\tPort no : %s\n", argv[2]);
            gethostname(myinfo.hostname, 100);
            myinfo.fd = 0;
            //printf("\nhostname is %s", myinfo.hostname);
            strcpy(myinfo.port, argv[2]);
            serverinfo.fd = 1;
            flag = 1;
            info[connection_no] = myinfo;
            connection_no++;
            CLIENT();

        } else printf("invalid command\n See description:\nEnter the command as C or S followed by the port number \n C <port no>- Client\nS <port no> -Server\nExample : S 1234\n");
    }

    return 0;
}

//server code starts here

int SERVER() {

    printf("This is the server system\n");
    char cmd[100], str[3][15];
    char * temp;
    int skip = 0;
    int listenfd = 0, connfd = 0, n = 0, i, nbytes, g, h, k = 0;
    char recvBuff[1024], buf[256];
    fd_set master; // master file descriptor list
    fd_set read_fds; // temp file descriptor list for select()

    int newfd; // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;
    struct sockaddr_in serv_addr;
    char remoteIP[INET6_ADDRSTRLEN];
    char sendBuff[1025];

    FD_ZERO(&master); // clear the master and temp sets
    FD_ZERO(&read_fds);


    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    printf("socket retrieve success\n");

    memset(&serv_addr, '0', sizeof (serv_addr));
    memset(sendBuff, '0', sizeof (sendBuff));
    // printf("server port is1 %s\n", myinfo.port);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(myinfo.port));

    bind(listenfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr));

    if (listen(listenfd, 10) == -1) {
        printf("Failed to listen\n");
        return -1;
    }
    // printf("listen done\n");
    // add the listenfd to the master set for listening from the socket connection
    FD_SET(listenfd, &master);
    // add Standard input to the master set for listening from the standard input
    FD_SET(0, &master);

    // keep track of the biggest file descriptor
    fdmax = listenfd; // so far, it's this one

    // main loop
    for (;;) {

        printf("Server>>%s$$", myinfo.hostname);
        fflush(stdout);

        read_fds = master; // copy it
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }

        if (FD_ISSET(0, &read_fds)) {
            //  printf("\nServer>>");
            fgets(cmd, 1000, stdin);
            if (cmd[strlen(cmd) - 1] == '\n')
                cmd[strlen(cmd) - 1] = 0;
            //  printf("%s\n", cmd);

            switch (cmd[2]) {


                case 'L':
                case 'l':
                    if (strcasecmp(cmd, "HELP") == 0) {
                        if ((word_count(cmd) != 1)&&(space_count(cmd) != 0)) {
                            printf("Invalid command, Please refer the HELP command\n");
                            break;
                        }
                        HELP();
                    } else {
                        printf("Invalid command, please have a look the read me by typing HELP as the command \n");
                    }
                    break;
                case 'I': case'i':
                    if (strcasecmp(cmd, "MYIP") == 0) {
                        if ((word_count(cmd) != 1)&&(space_count(cmd) != 0)) {
                            printf("Invalid command, Please refer the HELP command\n");
                            break;
                        }
                        MYIP();
                        printf("\t ip  Address : <%s>\n", myinfo.ip);
                    } else if (strcasecmp(cmd, "EXIT") == 0) {
                        if ((word_count(cmd) != 1)&&(space_count(cmd) != 0)) {
                            printf("Invalid command, Please refer the HELP command\n");
                            break;
                        } else {
                            for (g = 0; g < connection_no; g++) {
                                FD_CLR(info[g].fd, &master);
                                close(info[g].fd);
                            }
                            printf("\tDisconnecting other connections\n", myinfo.ip);
                            printf("\t Terminating. BYE\n", myinfo.ip);
                            return 0;
                        }
                    } else {
                        printf("Invalid command, please have a look the read me by typing HELP as the command \n");
                    }
                    break;
                case 'P':case 'p':
                    if (strcasecmp(cmd, "MYPORT") == 0) {
                        if ((word_count(cmd) != 1)&&(space_count(cmd) != 0)) {
                            printf("Invalid command, Please refer the HELP command\n");
                            break;
                        }
                        MYPORT();
                    } else {
                        printf("Invalid command, please have a look the read me by typing HELP as the command \n");
                    }
                    break;
                case 'E':case 'e':
                    if (strcasecmp(cmd, "CREATOR") == 0) {
                        if ((word_count(cmd) != 1)&&(space_count(cmd) != 0)) {
                            printf("Invalid command, Please refer the HELP command\n");
                            break;
                        }
                        OWNER();
                    } else {
                        printf("Invalid command, please have a look the read me by typing HELP as the command \n");
                    }
                    break;

                case 'S':case's':
                    if (strcasecmp(cmd, "LIST") == 0) {
                        if ((word_count(cmd) != 1)&&(space_count(cmd) != 0)) {
                            printf("Invalid command, Please refer the HELP command\n");
                            break;
                        }
                        LIST();
                    } else {
                        printf("Invalid command, please have a look the read me by typing HELP as the command \n");
                    }
                    break;
                case 'R': case 'r':
                    k = 0;
                    //terminate cmd.
                    // command check is done here
                    if ((word_count(cmd) != 2)&&(space_count(cmd) != 1)) {
                        printf("Invalid command, Please refer the HELP command\n");
                        break;
                    }
                    temp = strtok(cmd, " ");


                    strcpy(str[k], temp);
                    while ((temp != NULL)&&(k < 2)) {
                        k++;
                        strcpy(str[k], temp);
                        temp = strtok(NULL, " ");
                    }

                    if (strcasecmp(str[0], "TERMINATE") != 0) {
                        printf("Invalid command, HERE please have a look the read me by typing HELP as the command\n ");
                        break;
                    }

                    //checking the connection id is correct or not
                    for (k = 0; k < connection_no; k++) {
                        if (info[k + 1].fd == atoi(str[2])+3) {
                            skip = 1;
                        }

                    }

                    if (skip == 1) {
                        if (atoi(str[2])+3 == fdmax) {
                            fdmax--;
                        }
                        FD_CLR(atoi(str[2])+3, &master);
                        close(atoi(str[2])+3);
                        printf("Connection %d is terminated.\n", atoi(str[2]));

                        for (g = 0; g < connection_no; g++) {
                            if (info[g].fd == atoi(str[2])+3) {

                                for (k = g; k < connection_no; k++) {
                                    info[k] = info[k + 1];

                                }
                            }
                        }
                        connection_no--;

                    } else {

                        printf("Invalid command, connection id does not exist \n");

                    }



                    break;


                default:
                    printf("Invalid command, please have a look the read me by typing HELP as the command\n ");
                    break;
            }
        }

        // printf("\nconnection details\nfd\t\tip\t\tport\n");
        // run through the existing connections looking for data to read
        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listenfd) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listenfd,
                            (struct sockaddr *) &remoteaddr,
                            &addrlen); //gethostbyaddr
                    connection_no++;
                    //   printf("\nno of connection is : %d\n", connection_no);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) { // keep track of the max
                            fdmax = newfd;
                        }
                        //                        printf("\nselectserver: new connection from %s on "
                        //                                "socket %d\n",
                        //                                inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*) &remoteaddr), remoteIP, INET6_ADDRSTRLEN),
                        //                                newfd);
                        info[connection_no - 1].fd = newfd;


                        if ((n = recv(newfd, recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                            recvBuff[n] = '\0';

                            k = 0;
                            //         printf("recieved from client %s\n", recvBuff);

                            temp = strtok(recvBuff, " ");
                            strcpy(str[k], temp);
                            while ((temp != NULL)&&(k < 3)) {
                                k++;
                                strcpy(str[k], temp);
                                temp = strtok(NULL, " ");
                            }
                            strcpy(info[connection_no - 1].ip, str[1]);
                            strcpy(info[connection_no - 1].port, str[2]);
                            strcpy(info[connection_no - 1].hostname, str[3]);
                            //printf("\nconnection %d: %d\t%s\t%s\n %s\t%s\t%s\n", connection_no - 1, info[connection_no - 1].fd, info[connection_no - 1].ip, info[connection_no - 1].port, str[0], str[1], str[2], str[3]);
                            strcpy(recvBuff, "");

                        }
                        printf("New connection from client:%s from the client port %s\n", info[connection_no - 1].hostname, info[connection_no - 1].port);

                        //                        strcpy(sendBuff, "message from server: hello client ");
                        //                        //  strcpy(sendBuff,recvBuff);
                        //                      //  strcat(sendBuff, (char) newfd);
                        //                        send(newfd, sendBuff, strlen(sendBuff), 0);

                        strcpy(sendBuff, "");
                        strcpy(sendBuff, myinfo.hostname);
                        send(info[connection_no - 1].fd, sendBuff, strlen(sendBuff), 0);
                        usleep(100000);
                        //printf("server host send as %s\n", sendBuff);
                        for (h = 0; h < connection_no; h++) {

                            strcpy(sendBuff, "");
                            sprintf(sendBuff, "%d", connection_no);
                            send(info[h].fd, sendBuff, strlen(sendBuff), 0);
                            usleep(100000);
                            //  printf("sending info to fd %d AND no of connections%d\n", info[h].fd, connection_no);
                            for (g = 0; g < connection_no; g++) {
                                strcpy(sendBuff, "");
                                sprintf(sendBuff, "%d", info[g].fd);
                                // strcpy(sendBuff,(char)info[g].fd);
                                strcat(sendBuff, " ");
                                strcat(sendBuff, info[g].ip);
                                strcat(sendBuff, " ");
                                strcat(sendBuff, info[g].port);
                                strcat(sendBuff, " ");
                                strcat(sendBuff, info[g].hostname);
                                //    printf("\nsending the info as : %s\n", sendBuff);

                                send(info[h].fd, sendBuff, strlen(sendBuff), 0);
                                usleep(100000);

                                strcpy(sendBuff, "");
                            }
                        }
                        printf("Server>>$$Client REGISTER successful\n");
                        printf("Server>>$$Updated other Client connections\n");

                    }


                } else {


                    //                    // handle data from a client


                    if ((nbytes = recv(i, buf, sizeof (buf), 0)) == 0) {

                        buf[nbytes] = '\0';
                        // got error or connection closed by client
                        //                        printf("message from client %d: \t %s", i,recvBuff);
                        //                        if (fputs(recvBuff, stdout) == EOF) {
                        //                            printf("\n Error : Fputs error");
                        //                        }
                        if (nbytes == 0) {
                            // connection closed
                            printf(" Client at connection %d Disconnected\n", i);

                            for (g = 0; g < connection_no; g++) {
                                if (info[g].fd == i) {
                                    for (k = g; k < connection_no; k++) {
                                        info[k] = info[k + 1];
                                    }
                                }
                            }
                            connection_no = connection_no - 1;
                            usleep(100000);

                            //UPDATE WHEN A CLIENT IS DISCONNECTED.
                            for (h = 0; h < connection_no; h++) {

                                strcpy(sendBuff, "");
                                sprintf(sendBuff, "%d", connection_no);
                                send(info[h].fd, sendBuff, strlen(sendBuff), 0);
                                usleep(100000);
                                //            printf("sending info to fd %d AND no of connections%d\n", info[h].fd, connection_no);
                                for (g = 0; g < connection_no; g++) {
                                    strcpy(sendBuff, "");
                                    sprintf(sendBuff, "%d", info[g].fd);
                                    // strcpy(sendBuff,(char)info[g].fd);
                                    strcat(sendBuff, " ");
                                    strcat(sendBuff, info[g].ip);
                                    strcat(sendBuff, " ");
                                    strcat(sendBuff, info[g].port);
                                    strcat(sendBuff, " ");
                                    strcat(sendBuff, info[g].hostname);
                                    //              printf("\nsending the info as : %s\n", sendBuff);

                                    send(info[h].fd, sendBuff, strlen(sendBuff), 0);
                                    usleep(100000);

                                    strcpy(sendBuff, "");
                                }
                            }
                            //          printf("\nServer>>no of connection is : %d\n", connection_no);
                            printf("Update from Server sent to all the client\n");
                        } else {
                            perror("recv");
                        }


                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                    }
                } // END handle data from client
            } // END got new incoming connection


        }// END looping through file descriptors
    }







    printf("done\n");
    close(connfd);
    sleep(1);
    return 0;
}

//server code ends here

//client code

int CLIENT() {
    printf("This is the client system\n");
    char cmd[200];
    int listenfd = 0, connfd = 0, n = 0, i, nbytes, j, sockfd = 0, K, k = 0, g;
    char recvBuff[1024], buf[256];
    fd_set master; // master file descriptor list       
    fd_set read_fds; // temp file descriptor list for select()
    int fdmax; // maximum file descriptor number
    int newfd; // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;
    //struct sockaddr_in serv_addr;
    char remoteIP[INET6_ADDRSTRLEN];
    char sendBuff[1025];
    char * temp;
    char *fname;
    char str[10][200];
    int upload_flag = 0;
    int skip = 0;
    struct timeval u_start, u_end, d1_start, d2_start, d3_start, d1_end, d2_end, d3_end;
    float tx_upload, rx_upload, tx_download[3], rx_download[3];
    char reciever[100];

    FD_ZERO(&master); // clear the master and temp sets
    FD_ZERO(&read_fds);

    //variables for uploading a file
    unsigned long int count = 0, count1 = 0, count2 = 0, count3 = 0, count4 = 0, countread0 = 0, countread1 = 0, countread2 = 0;
    char buffer[100];
    char filename[200], fileout[200];
    int LENGTH = 512;
    char sdbuf[LENGTH], revbuf[LENGTH];
    //variables for download
    int wordC = 0, spaceC = 0, download_fd[3] = {0, 0, 0}, flag_f1 = 0, flag_f2 = 0, flag_f3 = 0;
    char download_filename[3][100];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    //    printf("socket retrieve success\n");

    memset(&serv_addr, '0', sizeof (serv_addr));
    memset(sendBuff, '0', sizeof (sendBuff));
    //  printf("MY port is2 %s\n", myinfo.port);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(myinfo.port));

    bind(listenfd, (struct sockaddr*) &serv_addr, sizeof (serv_addr));

    if (listen(listenfd, 10) == -1) {
        printf("Failed to listen\n");
        return -1;
    }
    // printf("listen done\n");
    // add the listenfd to the master set for listening from the socket connection
    FD_SET(listenfd, &master);

    // add Standard input to the master set for listening from the standard input
    FD_SET(0, &master);

    //to listen for registered clients from server.


    // keep track of the biggest file descriptor
    fdmax = listenfd; // maxfd is to max of either listenfd or sockfd.

    // main loop
    for (;;) {
        printf("Client>>%s$$", myinfo.hostname);

        fflush(stdout);
        strcpy(recvBuff, "");
        read_fds = master; // copy it
        if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1) {
            printf("skiped the error\n");

        }

        for (i = 0; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) {
                if (i == 0) {
                    //listen from standard input

                    fgets(cmd, 1000, stdin);
                    if (cmd[strlen(cmd) - 1] == '\n')
                        cmd[strlen(cmd) - 1] = 0;
                    //to check the message and its validation i am using switch.
                    switch (cmd[2]) {
                        case 'E':case 'e'://creator cmd
                            if (strcasecmp(cmd, "CREATOR") == 0) {
                                if ((word_count(cmd) != 1)&&(space_count(cmd) != 0)) {
                                    printf("Invalid command, Please refer the HELP command\n");
                                    break;
                                }
                                OWNER();
                            } else {
                                printf("Invalid command, please have a look the read me by typing HELP as the command \n");
                            }
                            break;

                        case 'L': case'l'://help and upload cmd
                            if (strcasecmp(cmd, "HELP") == 0) {
                                if ((word_count(cmd) != 1)&&(space_count(cmd) != 0)) {
                                    printf("Invalid command, Please refer the HELP command\n");
                                }
                                HELP();
                            } else if ((cmd[0] == 'U') || (cmd[0] == 'u')) {
                                //code for upload a file starts here.
                                k = 0;
                                if ((word_count(cmd) != 3)&&(space_count(cmd) != 2)) {
                                    printf("Invalid command, Please refer the HELP command\n");
                                    break;
                                }

                                temp = strtok(cmd, " ");
                                strcpy(str[k], temp);

                                while ((temp != NULL)&&(k < 3)) {
                                    k++;
                                    strcpy(str[k], temp);
                                    temp = strtok(NULL, " ");
                                }
                                if (strcasecmp(str[0], "UPLOAD") != 0) {
                                    printf("Invalid command, please have a look the read me by typing HELP as the command\n ");
                                    break;
                                }
                                for (k = 0; k < connection_no; k++) {
                                    if (info[k].fd == atoi(str[2])) {
                                        upload_flag = 1;
                                        strcpy(reciever, info[k].hostname);
                                    }
                                }
                                if (!upload_flag) {
                                    break;
                                    printf("In valid connection-id\n");
                                }
                                strcpy(filename, str[3]);
                                FILE *fs = fopen(filename, "r");
                                if (fs == NULL) {
                                    printf(" ERROR: File %s not found.\n", filename);
                                    break;
                                }
                                //getting the filename from the path n sending it across

                                //basic condition check to send file is done, on success of all condition file is read n sent.
                                //check the file size and send the information across

                                strcpy(sendBuff, "UPLOAD");
                                printf("sending %s\n", sendBuff);
                                send(atoi(str[2]), sendBuff, strlen(sendBuff), 0);

                                if ((n = recv(atoi(str[2]), recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                    recvBuff[n] = '\0';
                                    printf("%s", recvBuff);

                                }
                                fseek(fs, 0, SEEK_END);
                                count = ftell(fs);
                                fseek(fs, 0, SEEK_SET);
                                fflush(stdout);
                                sprintf(sendBuff, "%lu", (count));
                                fname = strrchr(filename, '/');
                                strcat(sendBuff, " ");
                                if (fname == NULL) {
                                    strcat(sendBuff, filename);
                                } else {
                                    fname++;
                                    strcat(sendBuff, fname);
                                }
                                send(atoi(str[2]), sendBuff, strlen(sendBuff), 0);
                                printf("sending file name and size %s\n", sendBuff);
                                if ((n = recv(atoi(str[2]), recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                    recvBuff[n] = '\0';

                                }

                                bzero(sdbuf, LENGTH);

                                //read "LENGTH" chucks of bytes everytime and send the file 
                                int fs_block_sz = 0;
                                //read time must start now
                                //timer for tx upload starts here.
                                printf("print it start here\n");
                                gettimeofday(&u_start, 0);

                                while ((fs_block_sz = fread(sdbuf, sizeof (char), LENGTH, fs)) > 0) {

                                    count1++;

                                    if (send(atoi(str[2]), sdbuf, fs_block_sz, 0) < 0) {
                                        printf("ERROR: Failed to send file %s. \n", filename);
                                        break;
                                    }

                                    if (((count1 * 5120) == count) || ((count1 * 5120) == (2 * count)) || ((count1 * 5120) == (3 * (count))) || ((count1 * 5120) == (4 * (count))) || ((count1 * 5120) == (5 * (count))) || ((count1 * 5120) == (6 * (count))) || ((count1 * 5120) == (7 * (count))) || ((count1 * 5120) == (8 * (count))) || ((count1 * 5120) == (9 * (count)))) {
                                        printf(" %lu of file is downloaded\n", ((count1 * 100 * 512) / (count)));
                                    }
                                    bzero(sdbuf, LENGTH);
                                }
                                gettimeofday(&u_end, 0);

                                tx_upload = ((double) ((u_end.tv_sec - u_start.tv_sec)*1000000 + u_end.tv_usec - u_start.tv_usec)) / 1000000;


                                printf("File %s from Connection %s was Sent!\n", filename, str[2]);
                                if ((n = recv(atoi(str[2]), recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                    recvBuff[n] = '\0';
                                    printf("%s", recvBuff);
                                }
                                printf("upload complete\n");
                                printf("Tx (%s): %s -> %s,\n File Size: %lu Bytes, \nTime Taken: %.6f seconds, \nTx Rate: %.2f bits/second.\n", myinfo.hostname, myinfo.hostname, reciever, count, tx_upload, (8 * count / tx_upload));

                                //file upload is done and timer should be stopped n noted

                                //calculation of the rate of upload.


                                //calculation ends here
                                //upload function ends here
                            } else {
                                printf("Invalid command, please have a look the read me by typing HELP as the command \n");
                            }
                            break;
                        case 'I':case 'i'://myip and exit cmd
                            if (strcasecmp(cmd, "MYIP") == 0) {
                                if ((word_count(cmd) != 1)&&(space_count(cmd) != 0)) {
                                    printf("Invalid command, Please refer the HELP command\n");
                                    break;
                                }
                                MYIP();
                                printf("\t ip  Address : <%s>\n", myinfo.ip);
                            } else if (strcasecmp(cmd, "EXIT") == 0) {
                                if ((word_count(cmd) != 1)&&(space_count(cmd) != 0)) {
                                    printf("Invalid command, Please refer the HELP command\n");
                                    break;
                                } else {
                                    for (g = 0; g < connection_no; g++) {
                                        FD_CLR(info[g].fd, &master);
                                        close(info[g].fd);
                                    }
                                    printf("\tDisconnecting other connections\n", myinfo.ip);
                                    printf("\t Terminating. BYE\n", myinfo.ip);
                                    return 0;
                                }
                            } else {
                                printf("Invalid command, please have a look the read me by typing HELP as the command \n");
                            }
                            break;
                        case 'P': case 'p'://myport cmd
                            if (strcasecmp(cmd, "MYPORT") == 0) {
                                if ((word_count(cmd) != 1)&&(space_count(cmd) != 0)) {
                                    printf("Invalid command, Please refer the HELP command\n");
                                    break;
                                }
                                MYPORT();
                            } else {
                                printf("Invalid command, please have a look the read me by typing HELP as the command \n");
                            }
                            break;

                        case 'W': case 'w'://download cmd
                            wordC = word_count(cmd);
                            spaceC = space_count(cmd);
                            flag_f1 = 0;
                            flag_f2 = 0;
                            flag_f3 = 0;
                            switch (wordC) {
                                case 3:
                                    k = 0;
                                    //download a single file.
                                    //  printf("One file download request\n");
                                    // printf("%s Is the cmd\n", cmd);
                                    temp = strtok(cmd, " ");


                                    //                                    printf("\n\n%s\n", temp);
                                    strcpy(str[k], temp);

                                    while ((temp != NULL)&&(k < wordC)) {
                                        k++;
                                        strcpy(str[k], temp);
                                        temp = strtok(NULL, " ");
                                    }
                                    //                                  printf("\n<%s %s %s %s>\n", str[0], str[1], str[2], str[3]);
                                    if (strcasecmp(str[0], "DOWNLOAD") != 0) {
                                        printf("\nClient>>Invalid command, please have a look the read me by typing HELP as the command\n ");
                                        break;
                                    }
                                    for (k = 0; k < connection_no; k++) {

                                        if (info[k].fd == atoi(str[2])) {
                                            download_fd[0] = atoi(str[2]);


                                        }
                                    }
                                    if (download_fd[0] == 0) {


                                        printf("\nClient>>Invalid connection-id, please see the list of active connection\t<<cmd:LIST>>\n ");
                                        break;
                                    }

                                    download_fd[1] = 0;
                                    download_fd[2] = 0;
                                    //send details of download.
                                    //                                printf("send download\n");
                                    strcpy(sendBuff, "DOWNLOAD");
                                    send(download_fd[0], sendBuff, strlen(sendBuff), 0);
                                    //                              printf("%s sent\n", sendBuff);
                                    sleep(1);
                                    strcpy(sendBuff, str[3]);
                                    send(download_fd[0], sendBuff, strlen(sendBuff), 0);
                                    //                            printf("waiting for filename ack\n");
                                    if ((n = recv(download_fd[0], recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                        //                              printf("\nreading from client file name\n");
                                        recvBuff[n] = '\0';


                                    }
                                    if (strcmp(recvBuff, "File not found") != 0) {

                                        strcpy(download_filename[0], recvBuff);
                                        //      printf("sending okay\n");
                                        strcpy(sendBuff, "Send ok");
                                        send(download_fd[0], sendBuff, strlen(sendBuff), 0);
                                    } else {
                                        printf("Invalid filename\n");
                                        break;

                                    }

                                    //                        printf("listening for download file0\n");
                                    if ((n = recv(download_fd[0], recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                        recvBuff[n] = '\0';
                                        printf("msg: %s", recvBuff);
                                    }

                                    if ((n = recv(download_fd[0], recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                        recvBuff[n] = '\0';
                                        count2 = strtoul(recvBuff, NULL, 0);
                                    }
                                    strcpy(sendBuff, "file size received");
                                    //                      printf("file size receieved %lu\n", count2);
                                    send(download_fd[0], sendBuff, strlen(sendBuff), 0);
                                    //                    printf("done here at one file download req\n");
                                    printf("DOWNLOAD FILE : %s REQUESTED from C-id : %d \n", download_filename[0], download_fd[0]);
                                    break;
                                case 5:
                                    //download 2 files
                                    k = 0;
                                    //printf("Two file download request\n");
                                    temp = strtok(cmd, " ");
                                    strcpy(str[k], temp);

                                    while ((temp != NULL)&&(k < wordC)) {
                                        k++;
                                        strcpy(str[k], temp);
                                        temp = strtok(NULL, " ");
                                    }
                                    if (strcasecmp(str[0], "DOWNLOAD") != 0) {
                                        printf("Invalid command, please have a look the read me by typing HELP as the command\n ");
                                        break;
                                    }
                                    for (k = 0; k < connection_no; k++) {

                                        if (info[k].fd == atoi(str[2])) {
                                            download_fd[0] = atoi(str[2]);


                                        }
                                    }

                                    if (download_fd[0] == 0) {


                                        printf("Invalid connection-id, please see the list of active connection\t<<cmd:LIST>>\n ");
                                        break;
                                    }
                                    for (k = 0; k < connection_no; k++) {

                                        if (info[k].fd == atoi(str[4])) {
                                            download_fd[1] = atoi(str[4]);


                                        }
                                    }
                                    if (download_fd[1] == 0) {


                                        printf("Invalid connection-id, please see the list of active connection\t<<cmd:LIST>>\n ");
                                        break;
                                    }


                                    download_fd[2] = 0;
                                    //send details of download.
                                    //1...
                                    strcpy(sendBuff, "DOWNLOAD");
                                    send(download_fd[0], sendBuff, strlen(sendBuff), 0);
                                    usleep(100000);

                                    strcpy(sendBuff, str[3]);
                                    send(download_fd[0], sendBuff, strlen(sendBuff), 0);
                                    if ((n = recv(download_fd[0], recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                        recvBuff[n] = '\0';
                                        if (strcmp(recvBuff, "File not found") != 0) {

                                            strcpy(download_filename[0], recvBuff);
                                            //     printf("sending okay\n");
                                            strcpy(sendBuff, "Send ok");
                                            send(download_fd[0], sendBuff, strlen(sendBuff), 0);
                                        } else {
                                            printf("Invalid filename\n");
                                            flag_f1 = 1;

                                        }

                                    }
                                    if (!flag_f1) {
                                        // printf("listening for download file0\n");
                                        if ((n = recv(download_fd[0], recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                            recvBuff[n] = '\0';
                                            //   printf("msg: %s", recvBuff);
                                        }

                                        if ((n = recv(download_fd[0], recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                            recvBuff[n] = '\0';
                                            count2 = strtoul(recvBuff, NULL, 0);
                                        }
                                    }
                                    //printf("got file size\n");
                                    sleep(2);

                                    //2...

                                    strcpy(sendBuff, "DOWNLOAD");
                                    send(download_fd[1], sendBuff, strlen(sendBuff), 0);
                                    strcpy(sendBuff, str[5]);
                                    usleep(100000);
                                    send(download_fd[1], sendBuff, strlen(sendBuff), 0);
                                    if ((n = recv(download_fd[1], recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {

                                        recvBuff[n] = '\0';
                                        if (strcmp(recvBuff, "File not found") != 0) {

                                            strcpy(download_filename[1], recvBuff);
                                            //    printf("sending okay\n");
                                            strcpy(sendBuff, "Send ok");
                                            send(download_fd[1], sendBuff, strlen(sendBuff), 0);
                                        } else {
                                            printf("Invalid filename\n");
                                            flag_f2 = 1;
                                        }
                                    }



                                    //2...
                                    if (!flag_f2) {
                                        if ((n = recv(download_fd[1], recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                            recvBuff[n] = '\0';
                                        }

                                        if ((n = recv(download_fd[1], recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                            recvBuff[n] = '\0';
                                            count3 = strtoul(recvBuff, NULL, 0);
                                        }
                                    }
                                    if (!flag_f1) {
                                        strcpy(sendBuff, "file size received");
                                        send(download_fd[0], sendBuff, strlen(sendBuff), 0);
                                        sleep(1);
                                        printf("DOWNLOAD FILE1 : %s REQUESTED from C-id : %d \n", download_filename[0], download_fd[0]);
                                    }
                                    if (!flag_f2) {
                                        strcpy(sendBuff, "file size received");
                                        send(download_fd[1], sendBuff, strlen(sendBuff), 0);
                                        sleep(1);
                                        printf("DOWNLOAD FILE2 : %s REQUESTED from C-id : %d \n", download_filename[1], download_fd[1]);
                                    }
                                    break;
                                case 7:
                                    //download 3 files
                                    k = 0;
                                    // printf("Two file download request\n");
                                    temp = strtok(cmd, " ");

                                    strcpy(str[k], temp);

                                    while ((temp != NULL)&&(k < wordC)) {
                                        k++;
                                        strcpy(str[k], temp);
                                        temp = strtok(NULL, " ");
                                    }
                                    if (strcasecmp(str[0], "DOWNLOAD") != 0) {
                                        printf("Invalid command, please have a look the read me by typing HELP as the command\n ");
                                        break;
                                    }
                                    for (k = 0; k < connection_no; k++) {

                                        if (info[k].fd == atoi(str[2])) {
                                            download_fd[0] = atoi(str[2]);


                                        }
                                    }

                                    if (download_fd[0] == 0) {


                                        printf("Invalid connection-id, please see the list of active connection\t<<cmd:LIST>>\n ");
                                        break;
                                    }
                                    for (k = 0; k < connection_no; k++) {

                                        if (info[k].fd == atoi(str[4])) {
                                            download_fd[1] = atoi(str[4]);


                                        }
                                    }
                                    if (download_fd[1] == 0) {


                                        printf("Invalid connection-id, please see the list of active connection\t<<cmd:LIST>>\n ");
                                        break;
                                    }
                                    for (k = 0; k < connection_no; k++) {

                                        if (info[k].fd == atoi(str[6])) {
                                            download_fd[2] = atoi(str[6]);


                                        }
                                    }
                                    if (download_fd[2] == 0) {


                                        printf("Invalid connection-id, please see the list of active connection\t<<cmd:LIST>>\n ");
                                        break;
                                    }


                                    //send details of download.
                                    //1...
                                    strcpy(sendBuff, "DOWNLOAD");
                                    send(download_fd[0], sendBuff, strlen(sendBuff), 0);

                                    usleep(100000);
                                    strcpy(sendBuff, str[3]);
                                    send(download_fd[0], sendBuff, strlen(sendBuff), 0);
                                    if ((n = recv(download_fd[0], recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {

                                        recvBuff[n] = '\0';
                                        if (strcmp(recvBuff, "File not found") != 0) {

                                            strcpy(download_filename[0], recvBuff);
                                            strcpy(sendBuff, "Send ok");
                                            send(download_fd[0], sendBuff, strlen(sendBuff), 0);
                                        } else {
                                            printf("Invalid filename\n");
                                            flag_f1 = 1;
                                        }

                                    }
                                    if (!flag_f1) {
                                        if ((n = recv(download_fd[0], recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                            recvBuff[n] = '\0';
                                        }

                                        if ((n = recv(download_fd[0], recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                            recvBuff[n] = '\0';
                                            count2 = strtoul(recvBuff, NULL, 0);
                                        }
                                    }
                                    sleep(1);

                                    //2...
                                    strcpy(sendBuff, "DOWNLOAD");
                                    send(download_fd[1], sendBuff, strlen(sendBuff), 0);
                                    usleep(100000);
                                    strcpy(sendBuff, str[5]);
                                    send(download_fd[1], sendBuff, strlen(sendBuff), 0);
                                    if ((n = recv(download_fd[1], recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {

                                        recvBuff[n] = '\0';
                                        if (strcmp(recvBuff, "File not found") != 0) {

                                            strcpy(download_filename[1], recvBuff);
                                            strcpy(sendBuff, "Send ok");
                                            send(download_fd[1], sendBuff, strlen(sendBuff), 0);
                                        } else {
                                            printf("Invalid filename\n");
                                            flag_f2 = 1;
                                        }

                                    }
                                    if (!flag_f2) {
                                        if ((n = recv(download_fd[1], recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                            recvBuff[n] = '\0';
                                        }

                                        if ((n = recv(download_fd[1], recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                            recvBuff[n] = '\0';
                                            count3 = strtoul(recvBuff, NULL, 0);
                                        }
                                    }
                                    sleep(2);
                                    //3...
                                    strcpy(sendBuff, "DOWNLOAD");
                                    send(download_fd[2], sendBuff, strlen(sendBuff), 0);
                                    strcpy(sendBuff, str[7]);
                                    usleep(100000);
                                    send(download_fd[2], sendBuff, strlen(sendBuff), 0);
                                    if ((n = recv(download_fd[2], recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                        recvBuff[n] = '\0';
                                        if (strcmp(recvBuff, "File not found") != 0) {

                                            strcpy(download_filename[2], recvBuff);
                                            strcpy(sendBuff, "Send ok");
                                            send(download_fd[2], sendBuff, strlen(sendBuff), 0);
                                        } else {
                                            printf("Invalid filename\n");
                                            flag_f3 = 1;
                                        }

                                    }
                                    if (!flag_f3) {
                                        if ((n = recv(download_fd[2], recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                            recvBuff[n] = '\0';
                                        }

                                        if ((n = recv(download_fd[2], recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                            recvBuff[n] = '\0';
                                            count4 = strtoul(recvBuff, NULL, 0);
                                        }
                                    }
                                    if (!flag_f1) {
                                        strcpy(sendBuff, "file size received");
                                        printf("file size receieved %lu\n", count2);
                                        send(download_fd[0], sendBuff, strlen(sendBuff), 0);
                                        printf("DOWNLOAD FILE1 : %s REQUESTED from C-id : %d \n", download_filename[0], download_fd[0]);
                                    }
                                    //2...
                                    if (!flag_f2) {
                                        sleep(1);
                                        strcpy(sendBuff, "file size received");
                                        printf("DOWNLOAD FILE2 : %s REQUESTED from C-id : %d \n", download_filename[1], download_fd[1]);
                                        send(download_fd[1], sendBuff, strlen(sendBuff), 0);
                                    }
                                    //3..
                                    sleep(1);
                                    if (!flag_f3) {
                                        strcpy(sendBuff, "file size received");
                                        printf("DOWNLOAD FILE3 : %s REQUESTED from C-id : %d \n", download_filename[2], download_fd[2]);
                                        send(download_fd[2], sendBuff, strlen(sendBuff), 0);
                                    }
                                    break;
                                default:
                                    printf("\nClient>>Invalid command, please have a look the read me by typing HELP as the command \n");
                                    break;
                            }

                            break;


                        case 'G': case 'g':
                            //register cmd
                            connfd = REGISTER(cmd);
                            if (connfd != 0) {
                                newfd = connfd;
                                sockfd = connfd;
                                fdmax = newfd;
                                FD_SET(newfd, &master);

                            }
                            break;

                        case 'N': case 'n':
                            //connect cmd
                            if (strcasecmp(cmd, "OWNER") == 0) {
                                if ((word_count(cmd) != 1)&&(space_count(cmd) != 0)) {
                                    printf("Invalid command, Please refer the HELP command\n");
                                    break;
                                }
                                OWNER();
                            } else {


                                connfd = CONNECT(cmd);
                                if (connfd != 0) {
                                    newfd = connfd;
                                    fdmax = newfd;
                                    FD_SET(newfd, &master);

                                }
                            }
                            break;


                        case 'R': case 'r':
                            k = 0;
                            //terminate cmd.
                            // command check is done here
                            if ((word_count(cmd) != 2)&&(space_count(cmd) != 1)) {
                                printf("Invalid command, Please refer the HELP command\n");
                                break;
                            }
                            temp = strtok(cmd, " ");


                            strcpy(str[k], temp);
                            while ((temp != NULL)&&(k < 2)) {
                                k++;
                                strcpy(str[k], temp);
                                temp = strtok(NULL, " ");
                            }

                            if (strcasecmp(str[0], "TERMINATE") != 0) {
                                printf("Invalid command, HERE please have a look the read me by typing HELP as the command\n ");
                                break;
                            }
                            if (atoi(str[2]) == 1) {
                                printf("Terminating the connection with server\n");
                                printf("Terminating all the connections\n");
                                for (g = 0; g < connection_no; g++) {
                                    if (info[g].fd > 3) {
                                        printf("closing %d\n", info[g].fd);
                                        close(info[g].fd);
                                        FD_CLR(info[g].fd, &master);
                                    }
                                }
                                printf("Connection with server is terminated\n");
                                close(sockfd);
                                FD_CLR(sockfd, &master);



                                connection_no = 1;
                                flag_register = 0;
                                fdmax = listenfd;
                                break;
                            }
                            //checking the connection id is correct or not
                            for (k = 0; k < connection_no; k++) {
                                if (info[k + 1].fd == atoi(str[2])) {
                                    skip = 1;
                                }

                            }

                            if (skip == 1) {
                                if (atoi(str[2]) == fdmax) {
                                    fdmax--;
                                }
                                FD_CLR(atoi(str[2]), &master);
                                close(atoi(str[2]));
                                printf("Connection %d is terminated.\n", atoi(str[2]));

                                for (g = 0; g < connection_no; g++) {
                                    if (info[g].fd == atoi(str[2])) {

                                        for (k = g; k < connection_no; k++) {
                                            info[k] = info[k + 1];

                                        }
                                    }
                                }
                                connection_no--;

                            } else {

                                printf("Invalid command, connection id does not exist \n");

                            }



                            break;

                        case 'S':case 's':
                            if (strcasecmp(cmd, "LIST") == 0) {
                                if ((word_count(cmd) != 1)&&(space_count(cmd) != 0)) {
                                    printf("Invalid command, Please refer the HELP command\n");
                                    break;
                                }
                                LIST();
                            } else {
                                printf("Invalid command, please have a look the read me by typing HELP as the command \n");
                            }
                            break;
                        case 'T': case 't':
                            if (strcasecmp(cmd, "RETURN") == 0) {
                                if ((word_count(cmd) != 1)&&(space_count(cmd) != 0)) {
                                    printf("Invalid command, Please refer the HELP command\n");
                                    break;
                                }
                                RETURN();
                            } else {
                                printf("Invalid command, please have a look the read me by typing HELP as the command \n");
                            }
                            break;
                        default:
                            printf("Invalid command, DEFAULT please have a look the read me by typing HELP as the command \n");
                            break;
                    }


                } else if (i == download_fd[0]) {
                    //downloading 1st file.
                    FILE *f0 = fopen(download_filename[0], "w+");
                    if (f0 == NULL)
                        printf("File %s Cannot be opened.\n", download_filename[0]);
                    else {
                        bzero(revbuf, LENGTH);
                        int fr_block_sz0 = 0;
                        countread0 = 0;
                        //RECIEVE THE FILE
                        for (g = 0; g < connection_no; g++) {
                            if (download_fd[0] == info[g].fd) {
                                strcpy(reciever, info[g].hostname);
                            }
                        }

                        //timer start here for rx
                        gettimeofday(&d1_start, 0);
                        while ((fr_block_sz0 = recv(download_fd[0], revbuf, LENGTH, 0)) > 0) {

                            countread0 += fr_block_sz0;

                            int write_sz = fwrite(revbuf, sizeof (char), fr_block_sz0, f0);
                            if (write_sz < fr_block_sz0) {
                                printf("File write failed.\n");
                            }
                            bzero(revbuf, LENGTH);
                            if (fr_block_sz0 == 0 || fr_block_sz0 != 511 || fr_block_sz0 != 512) {
                                if (countread0 == count2)break;
                            }



                            //     printf("count after download %lu\n", countread0);
                        }

                        //timer ends here
                        //CHECK THE FILE SIZE OF DOWNLOAD
                        fseek(f0, 0, SEEK_END);
                        countread0 = ftell(f0);
                        fseek(f0, 0, SEEK_SET);
                        //if file not matched then request to transfer the remaining contents.
                        if (countread0 == count2) {
                            printf("The file size matched \n");
                            gettimeofday(&d1_end, 0);
                            tx_download[0] = ((double) ((d1_end.tv_sec - d1_start.tv_sec)*1000000 + d1_end.tv_usec - d1_start.tv_usec)) / 1000000;
                            // printf("Ok File %s from Client was Sent!\n", filename);
                            printf("\nRx (%s): %s -> %s,\n File Size: %lu Bytes, \nTime Taken: %.7f seconds, \nRx Rate: %.2f bits/second.\n", myinfo.hostname, reciever, myinfo.hostname, count2, tx_download[0], (8 * count2 / tx_download[0]));

                            strcpy(sendBuff, "File transfered successfully\n\tDownload Completed!\n");
                            send(download_fd[0], sendBuff, strlen(sendBuff), 0);
                        }


                        if (fr_block_sz0 < 0) {

                            if (errno == EAGAIN) {
                                printf("recv() timed out.\n");
                            } else {
                                printf("recv() failed due to errno \n");
                            }
                        }

                        fclose(f0);




                    }




                } else if (i == download_fd[1]) {
                    //downloading second file

                    FILE *f1 = fopen(download_filename[1], "w+");
                    if (f1 == NULL)
                        printf("File %s Cannot be opened.\n", download_filename[1]);
                    else {
                        bzero(revbuf, LENGTH);
                        int fr_block_sz1 = 0;
                        countread1 = 0;
                        //RECIEVE THE FILE

                        for (g = 0; g < connection_no; g++) {
                            if (download_fd[1] == info[g].fd) {
                                strcmp(reciever, info[g].hostname);
                            }
                        }

                        //timer start here for rx
                        gettimeofday(&d2_start, 0);
                        while ((fr_block_sz1 = recv(download_fd[1], revbuf, LENGTH, 0)) > 0) {

                            countread1 += fr_block_sz1;

                            int write_sz = fwrite(revbuf, sizeof (char), fr_block_sz1, f1);
                            if (write_sz < fr_block_sz1) {
                                printf("File write failed.\n");
                            }
                            bzero(revbuf, LENGTH);
                            if (fr_block_sz1 == 0 || fr_block_sz1 != 511 || fr_block_sz1 != 512) {
                                if (countread1 == count3)break;
                            }

                            //              printf("count after download %lu\n", countread1);
                        }

                        //CHECK THE FILE SIZE OF DOWNLOAD
                        fseek(f1, 0, SEEK_END);
                        countread1 = ftell(f1);
                        fseek(f1, 0, SEEK_SET);
                        //if file not matched then request to transfer the remaining contents.
                        if (countread1 == count3) {
                            gettimeofday(&d2_end, 0);
                            tx_download[1] = ((double) ((d2_end.tv_sec - d2_start.tv_sec)*1000000 + d2_end.tv_usec - d2_start.tv_usec)) / 1000000;
                            // printf("Ok File %s from Client was Sent!\n", filename);
                            printf("\nRx (%s): %s -> %s,\n File Size: %lu Bytes, \nTime Taken: %.7f seconds, \nRx Rate: %.2f bits/second.\n", myinfo.hostname, reciever, myinfo.hostname, count3, tx_download[1], (8 * count3 / tx_download[1]));

                            strcpy(sendBuff, "File transfered successfully\n\tDownload Completed!\n");
                            send(download_fd[1], sendBuff, strlen(sendBuff), 0);
                        }


                        if (fr_block_sz1 < 0) {

                            if (errno == EAGAIN) {
                                printf("recv() timed out.\n");
                            } else {
                                printf("recv() failed due to errno \n");
                            }
                        }

                        fclose(f1);




                    }

                } else if (i == download_fd[2]) {

                    //downloading third file
                    FILE *f2 = fopen(download_filename[2], "w+");
                    if (f2 == NULL)
                        printf("File %s Cannot be opened.\n", download_filename[2]);
                    else {
                        bzero(revbuf, LENGTH);
                        int fr_block_sz2 = 0;
                        countread2 = 0;
                        //RECIEVE THE FILE
                        for (g = 0; g < connection_no; g++) {
                            if (download_fd[2] == info[g].fd) {
                                strcpy(reciever, info[g].hostname);
                            }
                        }

                        //timer start here for rx
                        gettimeofday(&d3_start, 0);
                        while ((fr_block_sz2 = recv(download_fd[2], revbuf, LENGTH, 0)) > 0) {

                            countread2 += fr_block_sz2;

                            int write_sz = fwrite(revbuf, sizeof (char), fr_block_sz2, f2);
                            if (write_sz < fr_block_sz2) {
                                printf("File write failed.\n");
                            }
                            bzero(revbuf, LENGTH);
                            if (fr_block_sz2 == 0 || fr_block_sz2 != 511 || fr_block_sz2 != 512) {
                                if (countread2 == count4)break;
                            }


                            //                  printf("count after download %lu\n", countread2);
                        }
                        gettimeofday(&d3_end, 0);
                        //CHECK THE FILE SIZE OF DOWNLOAD
                        fseek(f2, 0, SEEK_END);
                        countread2 = ftell(f2);
                        fseek(f2, 0, SEEK_SET);
                        //if file not matched then request to transfer the remaining contents.
                        if (countread2 == count4) {

                            tx_download[2] = ((double) ((d3_end.tv_sec - d3_start.tv_sec)*1000000 + d3_end.tv_usec - d3_start.tv_usec)) / 1000000;
                            // printf("Ok File %s from Client was Sent!\n", filename);
                            printf("\nRx (%s): %s -> %s,\n File Size: %lu Bytes, \nTime Taken: %.7f seconds, \nRx Rate: %.2f bits/second.\n", myinfo.hostname, reciever, myinfo.hostname, count4, tx_download[2], (8 * count4 / tx_download[2]));

                            strcpy(sendBuff, "File transfered successfully\n\tDownload Completed!\n");
                            send(download_fd[2], sendBuff, strlen(sendBuff), 0);
                        }


                        if (fr_block_sz2 < 0) {

                            if (errno == EAGAIN) {
                                printf("recv() timed out.\n");
                            } else {
                                printf("recv() failed due to errno \n");
                            }
                        }

                        printf("Ok received from server!\n");
                        fclose(f2);
                    }

                } else if (i == listenfd) {
                    //check for new connection

                    // handle new connections
                    printf("New connection\n", listenfd);
                    addrlen = sizeof remoteaddr;
                    newfd = accept(listenfd,
                            (struct sockaddr *) &remoteaddr,
                            &addrlen);

                    if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) { // keep track of the max
                            fdmax = newfd;
                        }
                        //                        printf("\nselectserver: new connection from %s on "
                        //                                "socket %d\n",
                        //                                inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*) &remoteaddr), remoteIP, INET6_ADDRSTRLEN),
                        //                                newfd);

                        info[connection_no].fd = newfd;


                        if ((n = recv(newfd, recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                            recvBuff[n] = '\0';
                            k = 0;
                            //         printf("recieved from client %s\n", recvBuff);

                            temp = strtok(recvBuff, " ");
                            strcpy(str[k], temp);
                            while ((temp != NULL)&&(k < 3)) {
                                k++;
                                strcpy(str[k], temp);
                                temp = strtok(NULL, " ");
                            }
                            strcpy(info[connection_no].ip, str[1]);
                            strcpy(info[connection_no].port, str[2]);
                            strcpy(info[connection_no].hostname, str[3]);
                            //                printf("\nconnection %d: %d\t%s\t%s\t %s\n", connection_no, info[connection_no].fd, info[connection_no].ip, info[connection_no].port, info[connection_no].hostname);
                            strcpy(recvBuff, "");
                            printf("Successfully connected to: %s  : %s : %s\n", info[connection_no ].hostname, info[connection_no ].ip, info[connection_no ].port);
                        }
                        connection_no++;

                        //                        strcpy(sendBuff, "message from server: hello client ");
                        //                        //  strcpy(sendBuff,recvBuff);
                        //                      //  strcat(sendBuff, (char) newfd);
                        //                        send(newfd, sendBuff, strlen(sendBuff), 0);

                        strcpy(sendBuff, "Successfully connected to:");
                        strcat(sendBuff, myinfo.hostname);
                        strcat(sendBuff, " port: ");
                        strcat(sendBuff, myinfo.port);
                        send(newfd, sendBuff, strlen(sendBuff), 0);
                        usleep(10000);

                    }



                } else if (i == sockfd) {
                    //anything from the server.
                    //  printf("here check for reand in register\n");
                    //                    // handle data from a client


                    if ((n = recv(sockfd, recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {

                        recvBuff[n] = '\0';
                        connection_no_server = atoi(recvBuff);
                        //     printf("number of connection at server: %d\n", connection_no_server);
                        for (j = 0; j < connection_no_server; j++) {
                            K = 0;
                            strcpy(recvBuff, "");
                            if ((n = recv(sockfd, recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                //       printf("\ndata received from server is %s for %d \n", recvBuff, j);
                                recvBuff[n] = '\0';

                                temp = strtok(recvBuff, " ");
                                strcpy(str[K], temp);
                                while ((temp != NULL)&&(K < 4)) {
                                    K++;
                                    strcpy(str[K], temp);
                                    temp = strtok(NULL, " ");
                                }
                                serverregisterinfo[j].fd = atoi(str[1]);
                                strcpy(serverregisterinfo[j].ip, str[2]);
                                strcpy(serverregisterinfo[j].port, str[3]);
                                strcpy(serverregisterinfo[j].hostname, str[4]);
                            }
                        }
                        printf("Got update from server.\n");
                        RETURN();

                    } else {
                        // server termination.
                        printf("Terminating connection with server\n");
                        printf("Removing all connections\n");
                        flag_register = 0;
                        for (g = fdmax; g > listenfd; g--) {

                            if (FD_ISSET(g, &master)) {

                                close(g);
                                FD_CLR(g, &master);

                            }

                        }

                        connection_no = 0;
                        connection_no_server = 0;
                        fdmax = 3;
                    }


                } else {
                    // printf("last else condition-->%d\n", i);
                    if ((n = recv(i, recvBuff, sizeof (recvBuff) - 1, 0)) <= 0) {
                        recvBuff[n] = '\0';
                        //     printf("inside i<=0\n");

                        if (n == 0) {
                            //remove from master and close fd
                            if (i == fdmax) {
                                fdmax--;
                            }
                            FD_CLR(i, &master);
                            close(i);
                            //remove the connection information
                            for (g = 0; g < connection_no; g++) {
                                if (info[g].fd == i) {
                                    for (k = g; k < connection_no; k++) {
                                        info[k] = info[k + 1];
                                    }
                                }
                            }
                            connection_no--;
                            printf("Connection with %d is DISCONNECTED\n", i);

                        } else {
                            perror("recv");
                        }
                    }//end of read<=0
                    else {//any other input
                        recvBuff[n] = '\0';
                        printf("%s", recvBuff);
                        //if there is a date for a client. 
                        if (strcmp(recvBuff, "DOWNLOAD") == 0) {
                            //downloading a file cmd will be addressed here.
                            if ((n = recv(i, recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                //  printf("\nreading from client file name :%s\n", recvBuff);
                                recvBuff[n] = '\0';
                                strcpy(filename, recvBuff);

                            }
                            printf("got filename\n");
                            FILE *fs = fopen(filename, "r");
                            if (fs == NULL) {
                                printf("\nClient>>ERROR: File %s not found.\n", filename);
                                strcpy(sendBuff, "File not found");
                                send(i, sendBuff, strlen(sendBuff), 0);
                            } else {

                                fname = strrchr(filename, '/');
                                printf("sending filename %s", fname);

                                if (fname == NULL) {
                                    strcpy(sendBuff, filename);
                                } else {
                                    fname++;
                                    strcpy(sendBuff, fname);
                                }
                                send(i, sendBuff, strlen(sendBuff), 0);

                                if ((n = recv(i, recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                    //    printf("\nreading from client file name\n");
                                    recvBuff[n] = '\0';
                                    //   printf("%s\n", recvBuff);

                                }

                                strcpy(sendBuff, "START");
                                send(i, sendBuff, strlen(sendBuff), 0);
                                fseek(fs, 0, SEEK_END);
                                count = ftell(fs);
                                //  printf("the size of the file is %lu\n", count);
                                fseek(fs, 0, SEEK_SET);
                                strcpy(sendBuff, "");
                                //send file size
                                sprintf(sendBuff, "%lu", (count));
                                send(i, sendBuff, strlen(sendBuff), 0);

                                //rec ack
                                if ((n = recv(i, recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                    //  printf("\nreading from client file name\n");
                                    recvBuff[n] = '\0';
                                    //    printf("%s\n", recvBuff);

                                }

                                //file transfers starts here
                                bzero(sdbuf, LENGTH);
                                printf("UPLOADING....%s\n", fname);
                                //read 512 chucks of bytes everytime and send the file 
                                int fs_block_sz;
                                //read time must start now
                                gettimeofday(&d1_start, 0);
                                while ((fs_block_sz = fread(sdbuf, sizeof (char), LENGTH, fs)) > 0) {

                                    count1++;

                                    if (send(i, sdbuf, fs_block_sz, 0) < 0) {
                                        printf("ERROR: Failed to send file %s. \n", filename);
                                        break;
                                    }
                                    if (fs_block_sz == 0 || fs_block_sz != 511 || fs_block_sz != 512) {
                                        if (count == count1)break;
                                    }

                                    bzero(sdbuf, LENGTH);
                                }
                                //upload completes here. stop time.
                                gettimeofday(&d1_end, 0);

                                tx_download[0] = ((double) ((d1_end.tv_sec - d1_start.tv_sec)*1000000 + d1_end.tv_usec - d1_start.tv_usec)) / 1000000;
                                printf("UPLOAD complete\n");
                                printf("\nTx (%s): %s -> %s, \nFile Size: %lu Bytes, \nTime Taken: %.7f seconds, \nTx Rate: %.2f bits/second.\n", myinfo.hostname, myinfo.hostname, reciever, count, tx_download[0], (8 * count / tx_download[0]));

                                // printf("Ok File %s from Client was Sent!\n", filename);
                                if ((n = recv(i, recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                    recvBuff[n] = '\0';
                                    printf("%s", recvBuff);
                                }


                            }


                        } else if (strcmp(recvBuff, "UPLOAD") == 0) {
                            //file upload must be downloaded here.
                            //this is download part.
                            //  printf("inside the upload part \n");
                            //SEND MSG
                            strcpy(sendBuff, "START UPLOAD OF THE FILE");
                            send(i, sendBuff, strlen(sendBuff), 0);
                            //RECEIVE FILE SIZE

                            if ((n = recv(i, recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
                                k = 0;
                                recvBuff[n] = '\0';
                                //      printf("received file size%s\n", recvBuff);
                                temp = strtok(recvBuff, " ");
                                strcpy(str[k], temp);
                                while ((temp != NULL)&&(k < 2)) {
                                    k++;
                                    strcpy(str[k], temp);
                                    temp = strtok(NULL, " ");
                                }






                                count = strtoul(str[1], NULL, 0);
                                strcpy(fileout, str[2]);
                                // if (fputs(recvBuff, stdout) == EOF) {
                                //  printf("\n Error : Fputs error");
                                //}
                                //       printf("the count value is %lu\n", count);
                            }
                            //SEND ACK FOR FILE SIZE    
                            strcpy(sendBuff, "file size received");
                            send(i, sendBuff, strlen(sendBuff), 0);

                            FILE *f = fopen(fileout, "a+b");
                            if (f == NULL)
                                printf("File %s Cannot be opened.\n", fileout);
                            else {
                                bzero(revbuf, LENGTH);
                                int fr_block_sz = 0;
                                count1 = 0;
                                for (g = 0; g < connection_no; g++) {
                                    if (info[g].fd == i)strcpy(reciever, info[g].hostname);
                                }
                                //RECIEVE THE FILE
                                //rx for download starts here.
                                gettimeofday(&u_start, 0);
                                //      printf("while loopstarts\n");
                                sleep(1);

                                while ((fr_block_sz = recv(i, revbuf, LENGTH, 0)) > 0) {

                                    count1 += fr_block_sz;

                                    int write_sz = fwrite(revbuf, sizeof (char), fr_block_sz, f);
                                    if (write_sz < fr_block_sz) {
                                        printf("File write failed.\n");
                                    }
                                    bzero(revbuf, LENGTH);
                                    if (fr_block_sz == 0 || fr_block_sz != 511 || fr_block_sz != 512) {
                                        if (count == count1)break;
                                    }




                                }

                                gettimeofday(&u_end, 0);

                                rx_upload = ((double) ((u_end.tv_sec - u_start.tv_sec)*1000000 + u_end.tv_usec - u_start.tv_usec)) / 1000000;


                                //     printf("loop end\n");
                                //CHECK THE FILE SIZE OF DOWNLOAD
                                fseek(f, 0, SEEK_END);
                                count1 = ftell(f);
                                fseek(f, 0, SEEK_SET);

                                //if file not matched then request to transfer the remaining contents.
                                if (count1 == count) {
                                    //   printf("\t\tThe file size matched \n");
                                    strcpy(sendBuff, "\t\tFile transfered successfully\n");
                                    send(i, sendBuff, strlen(sendBuff), 0);

                                    printf("\t\tUpload completed: FILE %s DOWNLOADED!\n", fileout);
                                    printf("\nRx (%s): %s -> %s,\n File Size: %lu Bytes, \nTime Taken: %.7f seconds, \nRx Rate: %.2f bits/second.\n", myinfo.hostname, reciever, myinfo.hostname, count, rx_upload, (8 * count / rx_upload));
                                }
                                if (fr_block_sz < 0) {

                                    if (errno == EAGAIN) {
                                        printf("recv() timed out.\n");
                                    } else {
                                        printf("recv() failed due to errno \n");
                                    }
                                }
                            }

                            fclose(f);
                        } else {

                            printf("nothing matched\n%s", recvBuff);
                            sleep(1);
                        }

                    }

                }//end else statement




            }

        }



    }
    printf("done\n");
    close(connfd);
    sleep(1);

    return 0;
}
//register cmd

int REGISTER(char cmd[1000]) {
    struct hostent *ptrh1;

    int sockfd = 0;
    //multiple registeration check. 
    //checking if the client is registered of not.
    if (flag_register) {
        printf("Client>>$ The client is already registered with server, Can not register\n");
        return 0;

    }


    //breaking the string in parts for checking the command.
    int n = 0, i = 0, j;
    char recvBuff[1025], sendBuff[1025], messagein[200];

    char * temp;
    char str[10][200], portc[10];

    if ((word_count(cmd) != 3)&&(space_count(cmd) != 2)) {
        printf("\nClient>>$ Invalid command, Please refer the HELP command\n");
        return 0;
    }
    temp = strtok(cmd, " ");

    strcpy(str[i], temp);
    while ((temp != NULL)&&(i < 3)) {
        i++;
        strcpy(str[i], temp);
        temp = strtok(NULL, " ");
    }
    serverinfo.fd = connection_no;
    strcpy(serverinfo.ip, str[2]);
    strcpy(serverinfo.port, str[3]);
    if (strcasecmp(str[0], "REGISTER") != 0) {
        printf("Client>>$ Invalid command, please have a look the read me by typing HELP as the command\n ");
        return 0;
    }
    memset(recvBuff, '0', sizeof (recvBuff));
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\nClient>>$ Error : Could not create socket \n");
        return 1;
    }
    // printf("server port is3 %s\n", serverinfo.port);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(serverinfo.port));
    ptrh1 = gethostbyname(serverinfo.ip);
    memcpy(&serv_addr.sin_addr, ptrh1->h_addr, ptrh1->h_length);
    //serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
        printf("\nClient>>$ Error : Connect Failed \n");
        close(sockfd);
        return 0;
    }



    MYIP();

    strcpy(sendBuff, myinfo.ip);
    strcat(sendBuff, " ");
    strcat(sendBuff, myinfo.port);
    strcat(sendBuff, " ");
    strcat(sendBuff, myinfo.hostname);
    //printf("sending from client as %s \n", sendBuff);
    send(sockfd, sendBuff, strlen(sendBuff), 0);
    strcpy(recvBuff, "");
    if ((n = recv(sockfd, recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
        recvBuff[n] = '\0';
        strcpy(serverinfo.hostname, recvBuff);
        //   printf("\n server hostname: %s\n", serverinfo.hostname);
    }
    strcpy(recvBuff, "");
    if ((n = recv(sockfd, recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
        recvBuff[n] = '\0';
        connection_no_server = atoi(recvBuff);
        //    printf("\nnumber of connection at server: %d\n", connection_no_server);
    }
    for (j = 0; j < connection_no_server; j++) {
        i = 0;
        strcpy(recvBuff, "");
        if ((n = recv(sockfd, recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
            //       printf("\ndata received from server is %s for %d \n", recvBuff, j);
            recvBuff[n] = '\0';

            temp = strtok(recvBuff, " ");
            strcpy(str[i], temp);
            while ((temp != NULL)&&(i < 4)) {
                i++;
                strcpy(str[i], temp);
                temp = strtok(NULL, " ");
            }
            serverregisterinfo[j].fd = atoi(str[1]);
            strcpy(serverregisterinfo[j].ip, str[2]);
            strcpy(serverregisterinfo[j].port, str[3]);
            strcpy(serverregisterinfo[j].hostname, str[4]);
            //   printf("\nconnection %d: %d\t%s\t%s\t%s\n", j, serverregisterinfo[j].fd, serverregisterinfo[j].ip, serverregisterinfo[j].port, serverregisterinfo[j].hostname);
            info[connection_no] = serverinfo;
        }
    }
    printf("Client>>$ Successfully registered.\n");
    flag_register = 1;
    connection_no++;
    if (n < 0) {

        printf("\n Read Error \n");
    }

    return sockfd;
}
//function for connecting a client with a client

int CONNECT(char cmd[1000]) {

    int sockfd = 0, wrong = 0;
    struct hostent *ptrh1;
    if (connection_no < 5) {


        //breaking the string in parts for checking the command.
        int n = 0, i = 0, j;
        char recvBuff[1025], sendBuff[1025], messagein[200];

        char * temp;
        char str[10][200], portc[10];

        if ((word_count(cmd) != 3)&&(space_count(cmd) != 2)) {
            printf("\nClient>>$ Invalid command, Please refer the HELP command\n");
            return 0;
        }

        temp = strtok(cmd, " ");
        strcpy(str[i], temp);
        while ((temp != NULL)&&(i < 3)) {
            i++;
            strcpy(str[i], temp);
            temp = strtok(NULL, " ");
        }
        if (strcasecmp(str[0], "CONNECT") != 0) {
            printf("Client>>$ Invalid command, please have a look the read me by typing HELP as the command\n ");
            return 0;
        }
        //printf("\n<%s %s %s %s>\n", str[0], str[1], str[2], str[3]);
        //duplicate connection
        for (i = 0; i < connection_no; i++) {

            if (((strcmp(str[3], info[i].port)) == 0)) {
                if (((strcmp(str[2], info[i].ip)) == 0) || ((strcmp(str[2], info[i].hostname)) == 0)) {
                    printf("Client>>$ Duplicate connection, cannot connect.\n");
                    return 0;
                }
            } else if (((strcmp(str[3], myinfo.port)) == 0)) {
                if (((strcmp(str[2], myinfo.ip)) == 0) || ((strcmp(str[2], myinfo.hostname)) == 0)) {
                    printf("Client>>$ Self connection, cannot connect.\n");
                    return 0;
                }
            }

        }

        for (i = 0; i < connection_no_server; i++) {

            if (((strcmp(str[3], serverregisterinfo[i].port)) == 0)) {
                if (((strcmp(str[2], serverregisterinfo[i].ip)) == 0) || ((strcmp(str[2], serverregisterinfo[i].hostname)) == 0)) {
                    info[connection_no].fd = connection_no;
                    strcpy(info[connection_no].hostname, serverregisterinfo[i].hostname);
                    strcpy(info[connection_no].port, serverregisterinfo[i].port);
                    strcpy(info[connection_no].ip, serverregisterinfo[i].ip);
                    //printf("Client>>$ Establishing connection to %d %s %s %s\n ", info[connection_no].fd, info[connection_no].hostname, info[connection_no].ip, info[connection_no].port);
                    wrong = 1;
                }
            }
        }
        if (wrong != 1) {
            printf("$ Invalid CONNECT details\n ");

            return 0;
        }
        printf(" Establishing connection to %s %s %s\n ", info[connection_no].hostname, info[connection_no].ip, info[connection_no].port);



        memset(recvBuff, '0', sizeof (recvBuff));
        if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            printf("$ Error Cannot connect : connection failed\n");
            return 1;
        }
        info[connection_no].fd = sockfd;
        // printf("SOCK ID %d\n", sockfd);
        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(atoi(info[connection_no].port));
        ptrh1 = gethostbyname(info[connection_no].ip);
        memcpy(&serv_addr.sin_addr, ptrh1->h_addr, ptrh1->h_length);

        if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof (serv_addr)) < 0) {
            printf("\nClient>>$ Error : Connect Failed \n");
            close(sockfd);
            return 1;

        }

        MYIP();

        strcpy(sendBuff, myinfo.ip);
        strcat(sendBuff, " ");
        strcat(sendBuff, myinfo.port);
        strcat(sendBuff, " ");
        strcat(sendBuff, myinfo.hostname);
        // printf("sending from client as %s \n", sendBuff);
        send(sockfd, sendBuff, strlen(sendBuff), 0);
        strcpy(recvBuff, "");
        usleep(50000);
        if ((n = recv(sockfd, recvBuff, sizeof (recvBuff) - 1, 0)) > 0) {
            recvBuff[n] = '\0';
            //  printf("\nClient>>Received from %s port: %s\n\tMessage:%s", info[connection_no].hostname, info[connection_no].port, recvBuff);

            printf("\t\tConnected to %s via %s at %s \nClient>>$ connection successful\n", info[connection_no].hostname, info[connection_no].ip, info[connection_no].port);
            connection_no++;
        }
        //  printf("Client>>$ connection successful\n");


    } else {
        printf("\nClient>>$ Cannot have more connection-max connections reached\n");

        return 0;
    }
    return sockfd;
}

//function for help cmd

int HELP() {
    int choice;
    printf("***************************READ ME***************************\n");
    printf("Enter the choice for the help menu as:\n");
    if (flag) {//for client
        printf("0.Exit READ ME\n1.DISPLAY MYIP\n2.DISPLAY MYPORT\n3.REGISTER A CLIENT\n4.MAKE TCP CONNECTION BETWEEN TO SYSTEM\n5.LIST THE NUMBER OF CONNECTIONS\n6.TERMINATE AN EXISTING CONNECTION\n7.EXIT A PROCESS\n8.UPLOADING A FILE \n9.DOWNLOADING A FILE \n10.OWNER DETAILS\n11.TO SEE THE SERVER-IP-LIST\n");

        for (;;) {
            printf("\nEnter your choice:");
            scanf("%d", &choice);

            switch (choice) {
                case 0:
                    return 0;
                case 1:
                    printf("The command to display ip address is\n>>$MYIP\nReturned output:\n\t ip  Address : <xxx.xxx.xxx.xxx>\nExample:\tip  Address : <255.255.255.255>\n");
                    break;
                case 2:
                    printf("The command to display port number is\n>>$MYIP\nReturned output:\n\t port number : <xxxx>\nExample:\t port number : <0000>\n");
                    break;
                case 3:
                    printf("The command to register a client to a server is \n>>$REGISTER <serverip> <server port>\nReturn output: successfully registered \n");
                    break;
                case 4:
                    printf("The command to connect a client to a client is \n>>$ CONNECT <CONNECT-CLIENT-(<ip>OR<HOSTNAME>) <CLIENT port>\nReturn output: Establishing connection to <client details>\nSuccessfully connected. \n");

                    break;
                case 5:
                    printf("The command to display the list of active connection a system is connected to\n>>$LIST\nReturned output:\nCid:\t<hostname>\t<ipaddress>:\t<port>\n");
                    break;
                case 6:
                    printf("The command to terminate an active connection of a system that is connected \n>>$TERMINATE <connection-id>\nReturned output:\n>>$Connection at <cid> is terminated\n");
                    break;
                case 7:
                    printf("The command to exit\n>>$EXIT\nReturned output:\nTermination all connection\n>>$BYE\n");
                    break;
                case 8:
                    printf("The command to upload a file from a client to a connected client is \n>>$UPLOAD<REGERTED-CLIENTID><FILENAME>\nReturn output: File uploaded. \n");
                    break;
                case 9:
                    printf("The command to Download a file from a client to a connected client is \n>>$DOWNLOAD<REGERTED-CLIENTID><FILENAME><CONNECTION2><FILE2>...<NTHCONNECTOIN-ID><NTH FILENAME>\nReturn output: File uploaded. \n");
                    break;
                case 10:
                    printf("The command to display the owner information is\n>>$CREATOR\nReturned output:\nNAME:\t<name>\nUBID:\t<ubidname>\nEmail:\t<ubid>@buffalo.edu\n");
                    break;
                case 11:
                    printf("The command to display the list of active connection st the server\n>>$RETURN\nReturned output:\nCid:\t<hostname>\t<ipaddress>:\t<port>\n");
                    break;
                default:
                    printf("Invalid choice\n");

                    break;



            }
        }
    } else {//for server
        printf("0.Exit READ ME\n1.DISPLAY MYIP\n2.DISPLAY MYPORT\n3.LIST THE NUMBER OF CONNECTIONS\n4.EXIT\n5.OWNER DETAILS");

        for (;;) {
            printf("\nEnter your choice:");
            scanf("%d", &choice);

            switch (choice) {
                case 0:
                    return 0;
                case 1:
                    printf("The command to display ip address is\n>>$MYIP\nReturned output:\n\t ip  Address : <xxx.xxx.xxx.xxx>\nExample:\tip  Address : <255.255.255.255>\n");
                    break;
                case 2:
                    printf("The command to display port number is\n>>$MYIP\nReturned output:\n\t port number : <xxxx>\nExample:\t port number : <0000>\n");
                    break;
                case 3:
                    printf("The command to display the list of active connection a system is connected to\n>>$LIST\nReturned output:\nCid:\t<hostname>\t<ipaddress>:\t<port>\n");
                    break;
                case 4:
                    printf("The command to exit\n>>$EXIT\nReturned output:\nTermination all connection\n>>$BYE\n");
                    break;
                case 5:
                    printf("The command to display the owner information is\n>>$CREATOR\nReturned output:\nNAME:\t<name>\nUBID:\t<ubidname>\nEmail:\t<ubid>@buffalo.edu\n");
                    break;

                default:
                    printf("Invalid choice\n");

                    break;



            }
        }
    }
    return 0;
}


//part of the code referred from STACKOVERFLOW.

int print_addresses(const int domain) {
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
            strcpy(myinfo.ip, ip);

        }
    }
    close(s);

    return 1;
}

void MYIP() {
    int domains[] = {AF_INET};
    int i;

    for (i = 0; i < sizeof (domains) / sizeof (domains[0]); i++)
        if (!print_addresses(domains[i]))
            printf("ok");


}

void MYPORT() {

    printf(" port number : <%s>\n", myinfo.port);
}

void OWNER() {

    printf("NAME:\tVijay Manoharan\nUBID:\tmanohara\nEmail:\tmanohara@buffalo.edu\n");
}

void LIST() {
    // printf("\nno of connection is : %d\n", connection_no);
    if ((connection_no == 0)&&(flag == 0)) {
        printf("\nNo active connections\n");
    } else if ((connection_no < 2)&&(flag == 1)) {
        printf("\nNo active connections\n");
    } else {
        int i;
        //   printf("\nmax fd value is:", fdmax);

        if (flag) {

            if (connection_no == 1) {
                //            printf("\nNo active connections\n");
            } else {
                printf("\nconnection details\nConnection-id\tHostname\t\t\tip\tport\n");
                //printf("\n%d\t%s\t%s\t%s\n", serverinfo.fd, serverinfo.hostname, serverinfo.ip, serverinfo.port);
                for (i = 1; i < connection_no; i++) {
                    printf("\n%d\t%s    \t%s\t%s\n", info[i].fd, info[i].hostname, info[i].ip, info[i].port);

                }
            }
        } else {
            printf("\nconnection details\nConnection-id\tHostname\t\tip\tport\n");
            for (i = 0; i < connection_no; i++) {

                printf("\n%d\t%s    \t%s\t%s\n", info[i].fd - 3, info[i].hostname, info[i].ip, info[i].port);

            }
        }
    }
}

void RETURN() {
    if (connection_no_server == 0) {
        printf("\nNo active connections\n");
    } else {
        int i;
        //  printf("\nmax fd value is:", fdmax);
        printf("\nconnection details\nConnectionid\tHostname\t\tip\tport\n");
        if (flag) {
            // printf("\n%d\t%s\t%s\t%s\n", serverinfo.fd, serverinfo.hostname, serverinfo.ip, serverinfo.port);


            for (i = 0; i < connection_no_server; i++) {
                printf("\n%d\t%s    \t%s\t%s\n", serverregisterinfo[i].fd - 3, serverregisterinfo[i].hostname, serverregisterinfo[i].ip, serverregisterinfo[i].port);
            }
        } else {
            for (i = 0; i < connection_no_server; i++) {

                printf("\n%d\t%s    \t%s\t%s\n", serverregisterinfo[i].fd - 3, serverregisterinfo[i].hostname, serverregisterinfo[i].ip, serverregisterinfo[i].port);


            }
        }
    }
}

int RETURN1(int i, int a[10]) {
    if (i == 0) {
        printf("\nNo active connections\n");
    } else {
        int k;
        //  printf("\nmax fd value is:", fdmax);

        for (k = 0; k < i; k++) {

            printf("\n%d", a[k]);
        }
    }
    return 0;
}
//the idea for counting the word count was referred from stackover flow.

int word_count(const char command[ ]) {
    int counted = 0, sp_count = 0; // result

    // state:
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
    //printf("\nnumber of space is %d \t strlengt %d\t",sp_count-1,strlen(sentence));

    return counted;
}

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
    // printf("\nnumber of space is %d \t strlengt %d\t",sp_count-1,strlen(sentence));
    return (sp_count - 1);
}