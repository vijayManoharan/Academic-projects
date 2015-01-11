/* 
 * File:   SelectiveRepeat.c
 * Author: vijay
 *
 * Created on March 31, 2014, 9:08 AM
 */

/* 
 * File:   main.c
 * Author: vijay
 *
 * Created on March 29, 2014, 8:11 PM
 */


#include <stdlib.h>
#include <stdio.h>

#include <string.h>


/*
 * 
 */

/* ******************************************************************
 ALTERNATING BIT AND GO-BACK-N NETWORK EMULATOR: VERSION 1.1  J.F.Kurose

   This code should be used for PA2, unidirectional or bidirectional
   data transfer protocols (from A to B. Bidirectional transfer of data
   is for extra credit and is not required).  Network properties:
   - one way network delay averages five time units (longer if there
     are other messages in the channel for GBN), but can be larger
   - packets can be corrupted (either the header or the data portion)
     or lost, according to user-defined probabilities
   - packets will be delivered in the order in which they were sent
     (although some can be lost).
 **********************************************************************/

#define BIDIRECTIONAL 0    /* change to 1 if you're doing extra credit */
/* and write a routine called B_output */

#define A  0
#define B  1//assigning values to AorB
static int WindowSize = 500;//size of the window.
/* a "msg" is the data unit passed from layer 5 (teachers code) to layer  */
/* 4 (students' code).  It contains the data (characters) to be delivered */

/* to layer 5 via the students transport level protocol entities.         */
struct msg {
    char data[20];
};

/* a packet is the data unit passed from layer 4 (students code) to layer */
/* 3 (teachers code).  Note the pre-defined packet structure, which all   */

/* students must follow. */
struct pkt {
    int seqnum;
    int acknum;
    int checksum;
    char payload[20];
};

/********* STUDENTS WRITE THE NEXT SEVEN ROUTINES *********/
//function definition
void tolayer3(int, struct pkt);
void starttimer(int, float);
void tolayer5(int, char*);
void stoptimer(int);

//function to calculate checksum..

int checksum(struct pkt packet) {


    int checksum = 0; //to store the checksum value
    int i;
    checksum = packet.acknum; //adding the ack value 
    checksum += packet.seqnum; //adding the seq# value
    //printf("checksum size of %d \n",sizeof(packet.payload));
    for (i = 0; i < (sizeof (packet.payload)); i++) {
        /**treat each character as if it
         **were an 8 bit integer and just add them together
         */
        checksum += packet.payload[i]; //adding the character-by-character sum of the payload field

    }


    // printf("the checksum is %d", checksum);

    return checksum;
}

/*function to check the checksum of the packet is matched or not
 **matched returns 1
 * not matched returns 0 */

int checksum_check(struct pkt packet) {
    int checksum = 0; //to store the checksum value
    int i;
    checksum = packet.acknum; //adding the ack value 
    checksum += packet.seqnum; //adding the seq# value
    // printf("checksum size of %d \n",sizeof(packet.payload));
    for (i = 0; i < (sizeof (packet.payload)); i++) {
        /**treat each character as if it
         **were an 8 bit integer and just add them together
         */
        checksum += packet.payload[i]; //adding the character-by-character sum of the payload field

    }
    //compare the checksum here.
    if (packet.checksum == checksum)
        return 1;
    else
        return 0;
}
// structure that is used to keep the track of the messages sent at sender

struct tracker {
    struct pkt packets;
    int flag_status; //tracks the flag_status of the packet. -- 0 message in window, not yet sent. --1 message sent but not yet ackd,2-- sent and ackd
    float timeSent; //time stamp when the message is sent
};
/**the variable to details of the previously sent unackd packet.*/

//the queue of sender and receiver
struct tracker packetqueue[2000], Recievedqueue[2000];

int packetqueue_pointer = 0; //pointer for number of messages at the sender.

int sequence = 0;
//to keep track of unackd packet
int no_unackd = 0;

//flag for checking whether the timer is running or not.
int Flag_timer = 0;

//to keep a track on the expected sequence number at B
int expectedsequenceB = 0;
//assigning values to AorB

//flag for ackd or not after first send
int ack_recd = 0;

//value for timeout.
float timeout = 20.0;
float time = 0.0;
//the following variable are used to count the messages.
int no_packet_sent_5to4A = 0;
int no_packet_sent_4to3A = 0;
int no_packet_recd_4to5B = 0;
int no_packet_recd_3to4B = 0;

///the variable to keep track on the window start, window end and the buffer

int Base_pointer = 0; //start of the window
int Window_Pointer = 0; //end of the window
int Sent_pointer = 0; //number of packets in the window sent

/* to get the window size  ..... 
 
 
 */

/* called from layer 5, passed the data to be sent to other side */
void A_output(message)
struct msg message;
{
    no_packet_sent_5to4A++; //increasing the count of packets sent 5 to 4 of A
    int i;
    char payloadprint[20];
    //making packets and adding to the message queue
    printf("\nAt A during incoming message from layer5\n");
    packetqueue[packetqueue_pointer].packets.seqnum = sequence;
    packetqueue[packetqueue_pointer].packets.acknum = 0;
    strncpy(packetqueue[packetqueue_pointer].packets.payload, message.data, 20);
    packetqueue[packetqueue_pointer].packets.checksum = checksum(packetqueue[packetqueue_pointer].packets);
    packetqueue[packetqueue_pointer].flag_status = 0;
    packetqueue[packetqueue_pointer].timeSent = 0.0;
    sequence++; //incrementing the sequence number
    packetqueue_pointer++; //incrementing the size of the queue
    if (Base_pointer + WindowSize > Window_Pointer) {//check if the window is filled or not
        //window not filled, so fill the window here
        printf("inside packet queue\n");


        Window_Pointer++; //increasing the size of the window

    } else {
        //buffer the message. window is full
        printf("inside buffer\n");

    }


    /*send the message here. check if the timer is running or not..
     * if found running, just send the packet and add its tracking details
     * else start the timer and repeat the above step.
     */
    for (i = Sent_pointer; i < Window_Pointer; i++) {

        if ((Flag_timer == 0)&&(packetqueue[i].flag_status != 2)) {
            tolayer3(A, packetqueue[i].packets);
            no_packet_sent_4to3A++; //increasing the count of packets sent 4 to 3 of A
            starttimer(A, timeout);
            packetqueue[i].timeSent = time;
            Flag_timer = 1;
            packetqueue[i].flag_status = 1;
            Sent_pointer++;
            strncpy(payloadprint, packetqueue[i].packets.payload, 20);
            printf("sending the following packet\n seqnum:%d\t,acknum:%d,checksum:%d,message:%s\nsent at time %f\n", packetqueue[i].packets.seqnum, packetqueue[i].packets.acknum, packetqueue[i].packets.checksum, payloadprint, packetqueue[i].timeSent);
        } else if (packetqueue[i].flag_status == 0) {
            tolayer3(A, packetqueue[i].packets);
            no_packet_sent_4to3A++; //increasing the count of packets sent 4 to 3 of A
            packetqueue[i].timeSent = time;
            Flag_timer = 1;
            packetqueue[i].flag_status = 1;
            strncpy(payloadprint, packetqueue[i].packets.payload, 20);
            printf("sending the following packet\n seqnum:%d\t,acknum:%d,checksum:%d,message:%s\nsent at time %f\n", packetqueue[i].packets.seqnum, packetqueue[i].packets.acknum, packetqueue[i].packets.checksum, payloadprint, packetqueue[i].timeSent);
            Sent_pointer++;

        }


    }

    //to check the size of window and unack packet and buffer size.
    no_unackd = 0;
    //calculate the number of unackd packets
    for (i = Base_pointer; i < Window_Pointer; i++) {
        if (packetqueue[i].flag_status == 1) {
            no_unackd++;
        }

    }

    printf("the pointer values are base %d, window %d and buffer %d sent %d\n", Base_pointer, Window_Pointer, packetqueue_pointer, Sent_pointer);


}

void B_output(message) /* need be completed only for extra credit */
struct msg message;
{

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
struct pkt packet;
{
    int checksum_flag, i, LTidentifier = -1;
    int count = 0;
    float leasttime; //to set the timer to the earliest timestamp after the ack
    int resize = 0;
    char payloadprint[20];
    printf("\nAt A. Received ACK\n ");
    //corrupt packet, discard it
    checksum_flag = checksum_check(packet);
    if (checksum_flag == 0) {
        printf("Corrupt ACK\n");

        return;
    }

    //check if the ack is equal to the base.mark the status to 2
    if (Base_pointer == packet.acknum) {


        packetqueue[Base_pointer].flag_status = 2;

        ack_recd = 1;
        //   Base_pointer++;

    } else {
        //check if the ack is for any other packet. mark the status to 2
        for (i = Base_pointer; i < Window_Pointer; i++) {

            if ((packet.acknum == packetqueue[i].packets.seqnum)) {

                printf("Ack for %d\n", packetqueue[i].packets.seqnum);


                packetqueue[i].flag_status = 2;
                ack_recd = 1;


                //start timer now.

            }

        }
    }
    leasttime = time;
    /*change timer now.
     * if packets are there to send then find out the earliest sent of all the packets in window. set the nect timeout to that earliest timestamp.
     * 
     * if there is are no message in the window to be sent. stop the timer. 
     * if we find any packets which is timed out. then sent those packets.
     */
    for (i = Base_pointer; i < Sent_pointer; i++) {
        if ((time < (timeout + packetqueue[i].timeSent))&&(packetqueue[i].flag_status == 1)) {
            leasttime = leasttime > packetqueue[i].timeSent ? packetqueue[i].timeSent : leasttime;
            if (leasttime == packetqueue[i].timeSent)
                LTidentifier = i;
        } else if (packetqueue[i].flag_status == 1) {
            //sending all the packets for which the timer has expired.
            printf("time is %f and the packet sequence is %d n its sent time %f\n", time, packetqueue[i].packets.seqnum, packetqueue[i].timeSent);
            tolayer3(A, packetqueue[i].packets);
            no_packet_sent_4to3A++;
            packetqueue[i].timeSent = time;
            packetqueue[i].flag_status = 1;
            strncpy(payloadprint, packetqueue[i].packets.payload, 20);
            printf("sending the following packet due to timeout\n seqnum:%d\t,acknum:%d,checksum:%d,message:%s time : %f\n", packetqueue[i].packets.seqnum, packetqueue[i].packets.acknum, packetqueue[i].packets.checksum, payloadprint, packetqueue[i].timeSent);


        }


    }

    printf("the pointer values are base %d, window %d and buffer %d sent %d\n", Base_pointer, Window_Pointer, packetqueue_pointer, Sent_pointer);
    //finding the number of unack packets for all the send packets in the window.
    no_unackd = 0;
    for (i = Base_pointer; i < Window_Pointer; i++) {
        if (packetqueue[i].flag_status == 1) {
            no_unackd++;
        }

    }


    if (ack_recd) {
        stoptimer(A); //stop the timer only after ack is received for some packet


        if ((LTidentifier != -1)&&(no_unackd > 0)) {

            starttimer(A, timeout - (time - leasttime)); //setting the time out event for the next earliest timestamp
            Flag_timer = 1; //indicating that timer is started and is running.
        } else if (no_unackd > 0) {
            for (i = Base_pointer; i < Window_Pointer; i++) {
                if ((time < (20 + packetqueue[i].timeSent))&&(packetqueue[i].flag_status == 1)) {
                    leasttime = leasttime > packetqueue[i].timeSent ? packetqueue[i].timeSent : leasttime;
                }
            }
            Flag_timer = 1;
            starttimer(A, timeout - (time - leasttime));
        } else {
            Flag_timer = 0; //timer is stopped no packets are in the window.
        }

    }

    //remove if acknowledged continuous
    //1. find the variable upto which ackd.
    i = Base_pointer;
    while ((packetqueue[i].flag_status == 2)&&(i < Window_Pointer)) {
        i++;

        Base_pointer++;
    }








    if (packetqueue_pointer - Window_Pointer > 0) {
        printf("there are msgs in buffer\nbuffer size %d\nnew window size is %d\n", packetqueue_pointer - Window_Pointer, Window_Pointer - Base_pointer);

        if (packetqueue_pointer - Window_Pointer > (WindowSize - (Window_Pointer - Base_pointer))) {
            printf("buffer larger than the vacant window\n");
            Window_Pointer = Base_pointer + 10;

        } else {

            Window_Pointer = packetqueue_pointer;
        }
    }

    strcpy(payloadprint, "");
    for (i = Base_pointer; i < Window_Pointer; i++) {
        if ((Flag_timer == 0)&&(packetqueue[i].flag_status != 2)) {
            tolayer3(A, packetqueue[i].packets);
            no_packet_sent_4to3A++;

            starttimer(A, timeout);
            packetqueue[i].timeSent = time;
            Flag_timer = 1;
            packetqueue[i].flag_status = 1;
            strncpy(payloadprint, packetqueue[i].packets.payload, 20);
            printf("sending the following packet here 1\n seqnum:%d\t,acknum:%d,checksum:%d,message:%s\nsent at time %f\n", packetqueue[i].packets.seqnum, packetqueue[i].packets.acknum, packetqueue[i].packets.checksum, payloadprint, packetqueue[i].timeSent);
        } else if (packetqueue[i].flag_status == 0) {
            tolayer3(A, packetqueue[i].packets);
            no_packet_sent_4to3A++;
            packetqueue[i].timeSent = time;
            Flag_timer = 1;
            packetqueue[i].flag_status = 1;
            strncpy(payloadprint, packetqueue[i].packets.payload, 20);
            printf("sending the following packet here 2\n seqnum:%d\t,acknum:%d,checksum:%d,message:%s\nsent at time %f\n", packetqueue[i].packets.seqnum, packetqueue[i].packets.acknum, packetqueue[i].packets.checksum, payloadprint, packetqueue[i].timeSent);
        }

    }

    //send the packets now. if it is not acknowledged then ok. 
    //  printf("flagtimer value%d\n", Flag_timer);



    //to check the size of window and unack packet and buffer size.
    no_unackd = 0;
    //calculate the number of unackd packets
    for (i = Base_pointer; i < Window_Pointer; i++) {
        if (packetqueue[i].flag_status == 1) {
            no_unackd++;
        }

    }

    printf("the pointer values end of A_input() are base %d, window %d and buffer %d sent %d\n", Base_pointer, Window_Pointer, packetqueue_pointer, Sent_pointer);


}

/* called when A's timer goes off */
void A_timerinterrupt() {
    int i, LTidentifier = -1;
    char payloadprint[20];
    float leasttime;

    leasttime = time;
    printf("At A interrupt has occured at time:%f\n", time);
    for (i = Base_pointer; i < Window_Pointer; i++) {
        /*find the packet for which the interrupt is called for
         * send that packet
         * set its timestamp to present time.
         */
      
        if ((packetqueue[i].flag_status == 1)&&(time >= (timeout - 0.009 + packetqueue[i].timeSent))) {
            strncpy(payloadprint, packetqueue[i].packets.payload, 20);
            packetqueue[i].timeSent = time;
            packetqueue[i].flag_status = 1;
            printf("sending the following packet in interupt\n seqnum:%d\t,acknum:%d,checksum:%d,message:%s\nsent at time %f\n", packetqueue[i].packets.seqnum, packetqueue[i].packets.acknum, packetqueue[i].packets.checksum, payloadprint, packetqueue[i].timeSent);

            tolayer3(A, packetqueue[i].packets);
            no_packet_sent_4to3A++;

        }


        //identify the earliest time stamp and set that as the timer event now.
        if (packetqueue[i].flag_status == 1) {
            leasttime = leasttime >= packetqueue[i].timeSent ? packetqueue[i].timeSent : leasttime;
            if (leasttime == packetqueue[i].timeSent)
                LTidentifier = i;
        }

        printf("\n");
    }


    //setting the new timer event
    if (LTidentifier != -1) {
        starttimer(A, timeout - (time - packetqueue[LTidentifier].timeSent));
        Flag_timer = 1;
    }




    printf("At A interrupt func ended time:%f\n", time);

}

/* the following routine will be called once (only) before any other */

/* entity A routines are called. You can use it to do any initialization */
void A_init() {
}


/* Note that with simplex transfer from a-to-B, there is no B_output() */

/* called from layer 3, when a packet arrives for layer 4 at B*/
void B_input(packet)
struct pkt packet;
{

    int i, countrec = 0;
    int flag_inc = 1;
    struct pkt ack;
    char payload[20], payloadprint[20];
    int flag_checksum = checksum_check(packet);
    no_packet_recd_3to4B++;
    strncpy(payloadprint, packet.payload, 20);

    printf("\n At B \nReceived the following packet\n seqnum:%d\t,acknum:%d,checksum:%d,message:%s\n", packet.seqnum, packet.acknum, packet.checksum, payloadprint);
    //check for checksum match. discard if it doesn't match.

    if (!flag_checksum) {
        printf("corrupt packet, discard the packet\n");
        return;
    }
    /*save all the packets that arrives within the window.
     * send ack to those packets
     * save them if received out of order.
     * in-order packets are sent to layer 5.
     */
    if ((expectedsequenceB + WindowSize > packet.seqnum)&&(expectedsequenceB <= packet.seqnum)) {
        printf(" expectedseq %d\n", expectedsequenceB);
        printf(" sequence received is within the window\n");

        Recievedqueue[packet.seqnum ].packets = packet;
        Recievedqueue[packet.seqnum ].flag_status = 1;
        Recievedqueue[packet.seqnum ].timeSent = time;
        ack.acknum = packet.seqnum;
        ack.seqnum = 1;
        strcpy(ack.payload, " ");
        ack.checksum = checksum(ack);
        //send ack back and expected sequence to next.

        tolayer3(B, ack);

        if ((expectedsequenceB == packet.seqnum)) {
            strncpy(payload, packet.payload, 20);
            tolayer5(B, payload);
            strcpy(payload, "");
            i = 1;
            printf("old expectedseq %d\n", expectedsequenceB);

            countrec = expectedsequenceB;

            //update the next expected in-order packet.
            for (i = countrec; i < countrec + WindowSize; i++) {
                if (Recievedqueue[i].flag_status == 0)
                    flag_inc = 0;

                if ((Recievedqueue[i].flag_status == 1)&&(flag_inc)) {

                    expectedsequenceB++;

                }
            }




            printf("next expectedseq %d\n", expectedsequenceB);
            //   printf("\n receiver buffer details\n");


        }

    } else if ((expectedsequenceB > packet.seqnum)) {
        /* there may be packet with sequence number less than expected sequence.
         * for those packets 
         * send ACK only and do nothing
         */
        printf(" expectedseq %d\n", expectedsequenceB);
        printf("duplicated packet, discard \n");
        ack.acknum = packet.seqnum;
        ack.seqnum = 0;
        strcpy(ack.payload, " ");
        ack.checksum = checksum(ack);
        tolayer3(B, ack);

    }

}

/* called when B's timer goes off */
void B_timerinterrupt() {
}

/* the following rouytine will be called once (only) before any other */

/* entity B routines are called. You can use it to do any initialization */
void B_init() {


}

/*****************************************************************
 ***************** NETWORK EMULATION CODE STARTS BELOW ***********
The code below emulates the layer 3 and below network environment:
  - emulates the tranmission and delivery (possibly with bit-level corruption
    and packet loss) of packets across the layer 3/4 interface
  - handles the starting/stopping of a timer, and generates timer
    interrupts (resulting in calling students timer handler).
  - generates message to be sent (passed from later 5 to 4)

THERE IS NOT REASON THAT ANY STUDENT SHOULD HAVE TO READ OR UNDERSTAND
THE CODE BELOW.  YOU SHOLD NOT TOUCH, OR REFERENCE (in your code) ANY
OF THE DATA STRUCTURES BELOW.  If you're interested in how I designed
the emulator, you're welcome to look at the code - but again, you should have
to, and you defeinitely should not have to modify
 ******************************************************************/

struct event {
    float evtime; /* event time */
    int evtype; /* event type code */
    int eventity; /* entity where event occurs */
    struct pkt *pktptr; /* ptr to packet (if any) assoc w/ this event */
    struct event *prev;
    struct event *next;
};
struct event *evlist = NULL; /* the event list */

//forward declarations
void init();
void generate_next_arrival();
void insertevent(struct event*);

/* possible events: */
#define  TIMER_INTERRUPT 0  
#define  FROM_LAYER5     1
#define  FROM_LAYER3     2

#define  OFF             0
#define  ON              1
#define   A    0
#define   B    1

float lossprob = 0.0; /* probability that a packet is dropped */
float corruptprob = 0.0; /* probability that one bit is packet is flipped */
float lambda = 0.0; /* arrival rate of messages from layer 5 */
int ntolayer3 = 0; /* number sent into layer 3 */
int nlost = 0; /* number lost in media */
int ncorrupt = 0; /* number corrupted by media*/

int TRACE = 1; /* for my debugging */
int nsim = 0; /* number of messages from 5 to 4 so far */
int nsimmax = 0; /* number of msgs to generate, then stop */

void main() {
    struct event *eventptr;
    struct msg msg2give;
    struct pkt pkt2give;

    int i, j;
    char c;

    init();
    A_init();
    B_init();

    while (1) {
        eventptr = evlist; /* get next event to simulate */
        if (eventptr == NULL)
            goto terminate;
        evlist = evlist->next; /* remove this event from event list */
        if (evlist != NULL)
            evlist->prev = NULL;
        if (TRACE >= 2) {
            printf("\nEVENT time: %f,", eventptr->evtime);
            printf("  type: %d", eventptr->evtype);
            if (eventptr->evtype == 0)
                printf(", timerinterrupt  ");
            else if (eventptr->evtype == 1)
                printf(", fromlayer5 ");
            else
                printf(", fromlayer3 ");
            printf(" entity: %d\n", eventptr->eventity);
        }
        time = eventptr->evtime; /* update time to next event time */
        if (nsim == nsimmax)
            break; /* all done with simulation */
        if (eventptr->evtype == FROM_LAYER5) {
            generate_next_arrival(); /* set up future arrival */
            /* fill in msg to give with string of same letter */
            j = nsim % 26;
            for (i = 0; i < 20; i++)
                msg2give.data[i] = 97 + j;
            if (TRACE > 2) {
                printf("          MAINLOOP: data given to student: ");
                for (i = 0; i < 20; i++)
                    printf("%c", msg2give.data[i]);
                printf("\n");
            }
            nsim++;
            if (eventptr->eventity == A)
                A_output(msg2give);
            else
                B_output(msg2give);
        } else if (eventptr->evtype == FROM_LAYER3) {
            pkt2give.seqnum = eventptr->pktptr->seqnum;
            pkt2give.acknum = eventptr->pktptr->acknum;
            pkt2give.checksum = eventptr->pktptr->checksum;
            for (i = 0; i < 20; i++)
                pkt2give.payload[i] = eventptr->pktptr->payload[i];
            if (eventptr->eventity == A) /* deliver packet by calling */
                A_input(pkt2give); /* appropriate entity */
            else
                B_input(pkt2give);
            free(eventptr->pktptr); /* free the memory for packet */
        } else if (eventptr->evtype == TIMER_INTERRUPT) {
            if (eventptr->eventity == A)
                A_timerinterrupt();
            else
                B_timerinterrupt();
        } else {
            printf("INTERNAL PANIC: unknown event type \n");
        }
        free(eventptr);
    }

terminate:
    printf(" Simulator terminated at time %f\n after sending %d msgs from layer5\n", time, nsim);
    printf("Protocol: Selective Repeat\n");
    printf("%d of packets sent from the Application Layer of Sender A\n", no_packet_sent_5to4A);
    printf("%d of packets sent from the Transport Layer of Sender A\n", no_packet_sent_4to3A);
    printf("%d of packets received at the Transport layer of Receiver B\n", no_packet_recd_3to4B);
    printf("%d of packets received at the Application layer of Receiver B\n", expectedsequenceB);
    printf("Total time: %f time units\n", time);
    printf("Throughput = %f packets/time units\n", ((expectedsequenceB) / time));
}

void init() /* initialize the simulator */ {
    int i;
    float sum, avg;
    float jimsrand();


    printf("-----  Stop and Wait Network Simulator Version 1.1 -------- \n\n");
    printf("Enter the number of messages to simulate: ");
    scanf("%d", &nsimmax);
    printf("Enter  packet loss probability [enter 0.0 for no loss]:");
    scanf("%f", &lossprob);
    printf("Enter packet corruption probability [0.0 for no corruption]:");
    scanf("%f", &corruptprob);
    printf("Enter average time between messages from sender's layer5 [ > 0.0]:");
    scanf("%f", &lambda);
    printf("Enter TRACE:");
    scanf("%d", &TRACE);

    srand(9999); /* init random number generator */
    sum = 0.0; /* test random number generator for students */
    for (i = 0; i < 1000; i++)
        sum = sum + jimsrand(); /* jimsrand() should be uniform in [0,1] */
    avg = sum / 1000.0;
    if (avg < 0.25 || avg > 0.75) {
        printf("It is likely that random number generation on your machine\n");
        printf("is different from what this emulator expects.  Please take\n");
        printf("a look at the routine jimsrand() in the emulator code. Sorry. \n");
        exit(0);
    }

    ntolayer3 = 0;
    nlost = 0;
    ncorrupt = 0;

    time = 0.0; /* initialize time to 0.0 */
    generate_next_arrival(); /* initialize event list */
}

/****************************************************************************/
/* jimsrand(): return a float in range [0,1].  The routine below is used to */
/* isolate all random number generation in one location.  We assume that the*/
/* system-supplied rand() function return an int in therange [0,mmm]        */

/****************************************************************************/
float jimsrand() {
    double mmm = 2147483647; /* largest int  - MACHINE DEPENDENT!!!!!!!!   */
    float x; /* individual students may need to change mmm */
    x = rand() / mmm; /* x should be uniform in [0,1] */
    return (x);
}

/********************* EVENT HANDLINE ROUTINES *******/
/*  The next set of routines handle the event list   */

/*****************************************************/

void generate_next_arrival() {
    double x, log(), ceil();
    struct event *evptr;
    //char *malloc();
    float ttime;
    int tempint;

    if (TRACE > 2)
        printf("          GENERATE NEXT ARRIVAL: creating new arrival\n");

    x = lambda * jimsrand()*2; /* x is uniform on [0,2*lambda] */
    /* having mean of lambda        */
    evptr = (struct event *) malloc(sizeof (struct event));
    evptr->evtime = time + x;
    evptr->evtype = FROM_LAYER5;
    if (BIDIRECTIONAL && (jimsrand() > 0.5))
        evptr->eventity = B;
    else
        evptr->eventity = A;
    insertevent(evptr);
}

void insertevent(p)
struct event *p;
{
    struct event *q, *qold;

    if (TRACE > 2) {
        printf("            INSERTEVENT: time is %lf\n", time);
        printf("            INSERTEVENT: future time will be %lf\n", p->evtime);
    }
    q = evlist; /* q points to header of list in which p struct inserted */
    if (q == NULL) { /* list is empty */
        evlist = p;
        p->next = NULL;
        p->prev = NULL;
    } else {
        for (qold = q; q != NULL && p->evtime > q->evtime; q = q->next)
            qold = q;
        if (q == NULL) { /* end of list */
            qold->next = p;
            p->prev = qold;
            p->next = NULL;
        } else if (q == evlist) { /* front of list */
            p->next = evlist;
            p->prev = NULL;
            p->next->prev = p;
            evlist = p;
        } else { /* middle of list */
            p->next = q;
            p->prev = q->prev;
            q->prev->next = p;
            q->prev = p;
        }
    }
}

void printevlist() {
    struct event *q;
    int i;
    printf("--------------\nEvent List Follows:\n");
    for (q = evlist; q != NULL; q = q->next) {
        printf("Event time: %f, type: %d entity: %d\n", q->evtime, q->evtype, q->eventity);
    }
    printf("--------------\n");
}



/********************** Student-callable ROUTINES ***********************/

/* called by students routine to cancel a previously-started timer */
void stoptimer(AorB)
int AorB; /* A or B is trying to stop timer */
{
    struct event *q, *qold;

    if (TRACE > 2)
        printf("          STOP TIMER: stopping timer at %f\n", time);
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB)) {
            /* remove this event */
            if (q->next == NULL && q->prev == NULL)
                evlist = NULL; /* remove first and only event on list */
            else if (q->next == NULL) /* end of list - there is one in front */
                q->prev->next = NULL;
            else if (q == evlist) { /* front of list - there must be event after */
                q->next->prev = NULL;
                evlist = q->next;
            } else { /* middle of list */
                q->next->prev = q->prev;
                q->prev->next = q->next;
            }
            free(q);
            return;
        }
    printf("Warning: unable to cancel your timer. It wasn't running.\n");
}

void starttimer(AorB, increment)
int AorB; /* A or B is trying to stop timer */
float increment;
{

    struct event *q;
    struct event *evptr;
    //char *malloc();

    if (TRACE > 2)
        printf("          START TIMER: starting timer at %f\n", time);
    /* be nice: check to see if timer is already started, if so, then  warn */
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next)  */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == TIMER_INTERRUPT && q->eventity == AorB)) {
            printf("Warning: attempt to start a timer that is already started\n");
            return;
        }

    /* create future event for when timer goes off */
    evptr = (struct event *) malloc(sizeof (struct event));
    evptr->evtime = time + increment;
    evptr->evtype = TIMER_INTERRUPT;
    evptr->eventity = AorB;
    insertevent(evptr);
}

/************************** TOLAYER3 ***************/
void tolayer3(AorB, packet)
int AorB; /* A or B is trying to stop timer */
struct pkt packet;
{


    struct pkt *mypktptr;
    struct event *evptr, *q;
    //char *malloc();
    float lastime, x, jimsrand(), jsrn;
    int i;


    ntolayer3++;

    /* simulate losses: */
    if (jimsrand() < lossprob) {
        nlost++;
        if (TRACE > 0)
            printf("          TOLAYER3: packet being lost\n");
        return;
    }

    /* make a copy of the packet student just gave me since he/she may decide */
    /* to do something with the packet after we return back to him/her */
    mypktptr = (struct pkt *) malloc(sizeof (struct pkt));
    mypktptr->seqnum = packet.seqnum;
    mypktptr->acknum = packet.acknum;
    mypktptr->checksum = packet.checksum;
    for (i = 0; i < 20; i++)
        mypktptr->payload[i] = packet.payload[i];
    if (TRACE > 2) {
        printf("          TOLAYER3: seq: %d, ack %d, check: %d ", mypktptr->seqnum,
                mypktptr->acknum, mypktptr->checksum);
        for (i = 0; i < 20; i++)
            printf("%c", mypktptr->payload[i]);
        printf("\n");
    }

    /* create future event for arrival of packet at the other side */
    evptr = (struct event *) malloc(sizeof (struct event));
    evptr->evtype = FROM_LAYER3; /* packet will pop out from layer3 */
    evptr->eventity = (AorB + 1) % 2; /* event occurs at other entity */
    evptr->pktptr = mypktptr; /* save ptr to my copy of packet */
    /* finally, compute the arrival time of packet at the other end.
       medium can not reorder, so make sure packet arrives between 1 and 10
       time units after the latest arrival time of packets
       currently in the medium on their way to the destination */
    lastime = time;
    /* for (q=evlist; q!=NULL && q->next!=NULL; q = q->next) */
    for (q = evlist; q != NULL; q = q->next)
        if ((q->evtype == FROM_LAYER3 && q->eventity == evptr->eventity))
            lastime = q->evtime;
    evptr->evtime = lastime + 1 + 9 * jimsrand();



    /* simulate corruption: */
    if (jsrn = jimsrand() < corruptprob) {
        ncorrupt++;
        printf("the values of jimsrand %f and corruption prob %f\n", jsrn, corruptprob);
        if ((x = jimsrand()) < .75)
            mypktptr->payload[0] = 'Z'; /* corrupt payload */
        else if (x < .875)
            mypktptr->seqnum = 999999;
        else
            mypktptr->acknum = 999999;
        if (TRACE > 0)
            printf("          TOLAYER3: packet being corrupted\n");
    }

    if (TRACE > 2)
        printf("          TOLAYER3: scheduling arrival on other side\n");
    insertevent(evptr);
}

void tolayer5(AorB, datasent)
int AorB;
char datasent[20];
{
    int i;
    if (TRACE > 2) {
        printf("          TOLAYER5: data received: ");
        for (i = 0; i < 20; i++)
            printf("%c", datasent[i]);
        printf("\n");
    }

}
