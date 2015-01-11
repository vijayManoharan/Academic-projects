/* 
 * File:   main.c
 * Author: vijay
 * c program for demonstrating Go-Back_n protocol
 * email: manohara@buffalo.edu
 * Created on March 29, 2014, 8:11 PM
 */

//hearder files
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
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
#define A  0
#define B  1//assigning values to AorB
#define BIDIRECTIONAL 0    /* change to 1 if you're doing extra credit */
/* and write a routine called B_output */



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
    //b=b+2;

    //  printf("the checksum is %d", checksum);
    return checksum;
}
//function to return 0 or 1 based the value of checksum matched or not
//1 means matched
//0 not matched.

int checksum_check(struct pkt packet) {
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
    //return 0 or 1 based on whether the checksum is matched or not matched.
    if (packet.checksum == checksum)
        return 1;
    else
        return 0;
}
/**the variable to details of the previously sent unackd packet.*/
struct pkt packet_slidingwindow[1000];

//when the number or unackd but sent msg is equal to the sliding window size, buffer the messages. the variable is used for it

int numberofsentunackd = 0; //keep track of number of sent and unack messages
int sizeofslidingwindowfilled = 0; //window size filled
int sizeofbufferfilled = 0; //buffer size filled
int seqnumber = 0; //current sequence number.
int flag_timeroff = 1; //check 
int WindowSize = 500; //size of the window
int expectedsequenceB = 1; //expected sequence number at B
struct msg sending_buffer[1000]; //buffer at sending side


float timeout = 20.0; //timer 

//the following variable are used to count the messages.
int no_packet_sent_5to4A = 0;
int no_packet_sent_4to3A = 0;
int no_packet_recd_4to5B = 0;
int no_packet_recd_3to4B = 0;

/* called from layer 5, passed the data to be sent to other side */
void A_output(message)
struct msg message;
{
    int i;
    char payload[20];
    printf("\nAt A during incoming message from layer5\n");
    no_packet_sent_5to4A++;
    if (sizeofslidingwindowfilled < WindowSize) {
        //send the message, as there is vacancy in the window.

        seqnumber++;
        packet_slidingwindow[sizeofslidingwindowfilled].seqnum = seqnumber;
        packet_slidingwindow[sizeofslidingwindowfilled].acknum = 0;
        strncpy(packet_slidingwindow[sizeofslidingwindowfilled].payload, message.data, 20);
        packet_slidingwindow[sizeofslidingwindowfilled].checksum = checksum(packet_slidingwindow[sizeofslidingwindowfilled]);
        sizeofslidingwindowfilled++;

        //incrementing the pointer of the last packet that next seq.
    } else {
        //buffer the message.

        strncpy(sending_buffer[sizeofbufferfilled].data, message.data, 20);
        sizeofbufferfilled++;
    }

    //check the timer is running or not. if not running then start it.
    if (flag_timeroff) {
        starttimer(A, timeout);
        flag_timeroff = 0;
    }

    //start sending all new messages that is in the buffer but not sent.
    for (i = numberofsentunackd; i < sizeofslidingwindowfilled; i++) {

        tolayer3(A, packet_slidingwindow[i]);
        no_packet_sent_4to3A++;
        numberofsentunackd++;
        strcpy(payload, "");
        strncpy(payload, packet_slidingwindow[i].payload, 20);
        printf("sending the following packet\n seqnum:%d\t,acknum:%d,checksum:%d,message:%s\n", packet_slidingwindow[i].seqnum, packet_slidingwindow[i].acknum, packet_slidingwindow[i].checksum, payload);
        //keeping a track of unackd sent packets in the sliding window.
    }
    //  printf("the values of unack pointer %d\t Swindow pointer%d\t buffer size%d\n", numberofsentunackd, sizeofslidingwindowfilled, sizeofbufferfilled);

}

void B_output(message) /* need be completed only for extra credit */
struct msg message;
{

}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
struct pkt packet;
{
    printf("\nAt A during incoming ack\n");
    int i = 0, j, numberackd = -1, flag_ack = 0, resizebuff = -1;
    int checksum_flag;

    //corrupt message, discard here
    checksum_flag = checksum_check(packet);
    if (checksum_flag == 0) {
        printf("Corrupt ACK\n");

        return;
    }

    //check for ack here.
    for (i = 0; i < sizeofslidingwindowfilled; i++) {

        if (packet.acknum == packet_slidingwindow[i].seqnum) {
            numberackd = i;
            printf("ack received till %d\n", packet_slidingwindow[i].seqnum);
            flag_ack = 1;
        }
    }

    //shift the packets and remove the packets ackd
    for (j = 0; j < sizeofslidingwindowfilled - numberackd - 1; j++) {
        packet_slidingwindow[j] = packet_slidingwindow[j + numberackd + 1];
    }

    //update the sliding window pointer and size on ack.
    sizeofslidingwindowfilled = sizeofslidingwindowfilled - (numberackd + 1);
    numberofsentunackd = numberofsentunackd - (numberackd + 1);
    if (!flag_ack) {
        printf("invalid ack\n");

    }

    stoptimer(A);
    flag_timeroff = 1;

    //if unackd packets are there in the window then send start timer again.
    if (numberofsentunackd > 0) {
        starttimer(A, timeout);
        flag_timeroff = 0;
    }
    //on ack the sliding window space will become vacant, so move if any buffered messages to the window.

    if (sizeofbufferfilled > 0) {
        //there are some packets in the buffer

        printf("there msgs in buffer\nbuffer size %d\nnew window size is %d\n", sizeofbufferfilled, sizeofslidingwindowfilled);
        if (sizeofbufferfilled > (WindowSize - sizeofslidingwindowfilled)) {
            //buffer is more than the vacant space in the window
            for (i = sizeofslidingwindowfilled; i < WindowSize; i++) {
                //fill the window completely 


                seqnumber++;
                packet_slidingwindow[i].seqnum = seqnumber;
                packet_slidingwindow[i].acknum = 0;
                strncpy(packet_slidingwindow[i].payload, sending_buffer[resizebuff].data, 20);
                packet_slidingwindow[i].checksum = checksum(packet_slidingwindow[i]);
                sizeofslidingwindowfilled++;
                resizebuff++;
            }
            for (j = 0; j < sizeofbufferfilled - (resizebuff + 1); j++) {
                strncpy(sending_buffer[j].data, sending_buffer[j + resizebuff + 1].data, 20);
            }
            sizeofbufferfilled = sizeofbufferfilled - (resizebuff + 1);
            printf("buffer reduced to %d\nnew window size is %d\n", sizeofbufferfilled, sizeofslidingwindowfilled);
        } else {

            //buffer space is less then the vacant space in the window.
            j = 0;
            for (i = sizeofslidingwindowfilled; i < (sizeofslidingwindowfilled + sizeofbufferfilled); i++) {
                //fill window the remainder of the buffer.


                packet_slidingwindow[i].seqnum = seqnumber;
                packet_slidingwindow[i].acknum = 0;
                strncpy(packet_slidingwindow[i].payload, sending_buffer[j].data, 20);
                packet_slidingwindow[i].checksum = checksum(packet_slidingwindow[i]);
                j++;

            }

            //update the buffer and window size
            sizeofslidingwindowfilled = sizeofslidingwindowfilled + sizeofbufferfilled;
            sizeofbufferfilled = 0;
            printf("buffer emptied\nnew window size is %d\n", sizeofslidingwindowfilled);
        }

    }


}

/* called when A's timer goes off */
void A_timerinterrupt() {
    int i;
    char payload[20];
    //timeout has occurs here 
    starttimer(A, timeout);
    printf("timer interrupt occured\n");
    //send all the packets in the window.
    for (i = 0; i < sizeofslidingwindowfilled; i++) {
        tolayer3(A, packet_slidingwindow[i]);
        no_packet_sent_4to3A++;
        strcpy(payload, "");
        strncpy(payload, packet_slidingwindow[i].payload, 20);
        printf("sending seq=%d  -->%s\n", packet_slidingwindow[i].seqnum, payload);
    }
    numberofsentunackd = sizeofslidingwindowfilled;

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
    struct pkt ack;
    char payload[20];
    no_packet_recd_3to4B++;

    strcpy(payload, "");
    strncpy(payload, packet.payload, 20);
    printf("received the following packet\n seqnum:%d\t,acknum:%d,checksum:%d,message:%s\n", packet.seqnum, packet.acknum, packet.checksum,payload);

    int checksum_flag;

    //corrupt packet, discard here.
    checksum_flag = checksum_check(packet);
    if (checksum_flag == 0) {
        printf("Corrupt ACK\n");

        return;
    }

    //check the packet is in order
    if ((expectedsequenceB == packet.seqnum)) {
        //send ack back and expected sequence to next.
        printf("correct sequence number received = %d\n", expectedsequenceB);
        expectedsequenceB++;
        ack.acknum = packet.seqnum;
        ack.seqnum = 0;
        strcpy(ack.payload, " ");
        ack.checksum = checksum(ack);
        //send message to upper layer.
        tolayer5(B, packet.payload);
        no_packet_recd_4to5B++;
        tolayer3(B, ack);

    } else if (expectedsequenceB > packet.seqnum) {
        //packet is already ackd then send a ack and discard the message.

        ack.acknum = packet.seqnum;
        ack.seqnum = 0;
        strcpy(ack.payload, " ");
        ack.checksum = checksum(ack);
        tolayer3(B, ack);
        printf("Duplicate packet, discard the packet\n");

    } else {
        printf("invalid sequence received, discard the packet\n");
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



int TRACE = 1; /* for my debugging */
int nsim = 0; /* number of messages from 5 to 4 so far */
int nsimmax = 0; /* number of msgs to generate, then stop */
float time = 0.000;

float lossprob = 0.0; /* probability that a packet is dropped */
float corruptprob = 0.0; /* probability that one bit is packet is flipped */
float lambda = 0.0; /* arrival rate of messages from layer 5 */
int ntolayer3 = 0; /* number sent into layer 3 */
int nlost = 0; /* number lost in media */
int ncorrupt = 0; /* number corrupted by media*/

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
    printf("Protocol: Go-Back-N\n");
    printf("%d of packets sent from the Application Layer of Sender A\n", no_packet_sent_5to4A);
    printf("%d of packets sent from the Transport Layer of Sender A\n", no_packet_sent_4to3A);
    printf("%d of packets received at the Transport layer of Receiver B\n", no_packet_recd_3to4B);
    printf("%d of packets received at the Application layer of Receiver B\n", expectedsequenceB-1);
    printf("Total time: %f time units\n", time);
    printf("Throughput = %f packets/units\n", (expectedsequenceB-1 )/ time);
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
    float lastime, x, jimsrand();
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
    if (jimsrand() < corruptprob) {
        ncorrupt++;
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
