/* 
 * File:   main.c
 * Author: vijay
 * Ubmail : manohara@buffalo.edu
 * Created on March 28, 2014, 8:31 PM
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
#define A  0//assigning values to AorB
#define B  1
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
//function definition and declaration required
void tolayer3(int, struct pkt);
void starttimer(int, float);
void tolayer5(int, char*);
void stoptimer(int);

int count = 0;
int count_send = 0;

//the following variable are used to count the messages.
int no_packet_sent_5to4A = 0;
int no_packet_sent_4to3A = 0;
int no_packet_recd_4to5B = 0;
int no_packet_recd_3to4B = 0;


//function to calculate checksum ..

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
    //return 1 or 0 if the checksum is matched or not.
    if (packet.checksum == checksum)
        return 1;
    else
        return 0;
}

int checksum(struct pkt packet) {


    int checksum = 0; //to store the checksum value
    int i;
    checksum = packet.acknum; //adding the ack value 
    checksum += packet.seqnum; //adding the seq# value
  //  printf("checksum size of %d \n", sizeof (packet.payload));
    for (i = 0; i < (sizeof (packet.payload)); i++) {
        /**treat each character as if it
         **were an 8 bit integer and just add them together
         */
        checksum += packet.payload[i]; //adding the character-by-character sum of the payload field

    }

    //calculate checsum and return here.
  //  printf("the checksum is %d", checksum);
    return checksum;
}

/**the variable to details of the previously sent packet.*/
struct pkt packet_prev_seq, packet_prev_ack;

//to indicate that it is the first packet sent.
int flag_1st_send = 1;
int flag_1st_recv = 1;
int prev_ack = 0;


int flag_send = 1;

/* called from layer 5, passed the data to be sent to other side */
void A_output(message)
struct msg message;
{ //first step before building the next packet, we must make sure that the previous packet is ackd.
    no_packet_sent_5to4A++;
    char payload[20];
    //if there is wait for ack of the send or not. if ack is been waiting discard the upper message
    if (!flag_send)
        return;
    struct pkt packet;
    //condition to check if it is the first message.
    if (flag_1st_send) {
        //first packet
        packet.seqnum = 0;
    } else if (packet_prev_seq.seqnum == 0) {
        /**previous packet seqence is 0 then send the next packet with
         **sequence number 1 or else send with 0.
         */
        packet.seqnum = 1;

    } else {
        packet.seqnum = 0;
    }
    //making the packet here.
    strncpy(packet.payload, message.data, 20);
    packet.acknum = 0;
    packet.checksum = checksum(packet);
    //starting the timer here 
    //taking the rtt as 10. 12 >10 coz of propagation delay and processing delay that may occur
    starttimer(A, 20);

    //sending to layer3
    tolayer3(A, packet);
    no_packet_sent_4to3A++;
    //storing the previous packet incase if we have to resend the packet
    packet_prev_seq = packet;
    flag_1st_send = 0;

    flag_send = 0; //wait for ack of the previous packet.
    strncpy(payload,packet_prev_seq.payload,20);
    printf("sending the following packet\n seqnum:%d\t,acknum:%d,checksum:%d,message:%s\n", packet_prev_seq.seqnum, packet_prev_seq.acknum, packet_prev_seq.checksum, payload);
    //  printf("length of payload sent is %d", strlen(packet_prev_seq.payload));

}

void B_output(message) /* need be completed only for extra credit */
struct msg message;
{
    //nothing here.
}

/* called from layer 3, when a packet arrives for layer 4 */
void A_input(packet)
struct pkt packet;
{
    struct msg message; //payload part will be extracted and sent to layer 5 using this variable.
    //only on a correct ack# we will accept the packet other wise the packet will be discarded.
    int checksum_flag = 0; // gets the check sum flag for match the flag is 1, other wise 0.

    //check for corrupt packet. if corrupt discarding the packet.
    checksum_flag = checksum_check(packet);
    if (checksum_flag == 0) {
        printf("Corrupt ACK\n");

        return;
    }
    //if correct ack received then mark ready to send next packet.
    if ((packet_prev_seq.seqnum == packet.acknum)) {
        stoptimer(A);
        printf("correct ack, now we can send new packet\n");
        flag_send = 1;

    } else {
        printf("invalid ack\n");
    }

}

/* called when A's timer goes off */
void A_timerinterrupt() {
    char payload[20];
      strncpy(payload,packet_prev_seq.payload,20);
    //handling the timeout calls.
    printf(" time at interupt \n");
    starttimer(A, 20);
    no_packet_sent_4to3A++;
    //resending the previous packet to layer3 due to timeout event
    tolayer3(A, packet_prev_seq);

    //marking not ready to send new msg
    flag_send = 0;
    printf("resending the following packet due to time out\n seqnum:%d\t,acknum:%d,checksum:%d,message:%s\n", packet_prev_seq.seqnum, packet_prev_seq.acknum, packet_prev_seq.checksum, payload);
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
    struct pkt ack_packet;
    char payload[20];
    struct msg message;
    int checksum_flag = 0;
    no_packet_recd_3to4B++;

    //check for corrupt packet. if corrupt discarding the packet.
    checksum_flag = checksum_check(packet);
    if (checksum_flag == 0) {
        printf("Corrupt ACK\n");

        return;
    }

    
    strcpy(message.data, " "); //payload part will be extracted and sent to layer 5 using this variable.
    //only on a correct ack# we will accept the packet other wise the packet will be discarded.
    if (flag_1st_recv) {
        prev_ack = 1;
        flag_1st_recv = 0;
    }
    strncpy(payload,packet.payload,20);
    printf("Received the following packet\n seqnum:%d\t,acknum:%d,checksum:%d,message:%s\n", packet.seqnum, packet.acknum, packet.checksum, payload);
    //    printf("previous ackd # is %d\n", prev_ack);

    //check for the correct sequence of the packets received.
    if ((prev_ack != packet.seqnum)) {
        printf("correct seq, now we can send ack packet\n");
        //the message to above and send ack below.

        //step1 send payload to layer5
        strncpy(message.data, packet.payload, 20);
        tolayer5(B, message.data);
        no_packet_recd_4to5B++;
        prev_ack = packet.seqnum;
    } else {
        printf("already acked, message discard\n");
    }

    //step2 send ack to layer3
    //build packet now....
    //acknum
    packet_prev_ack.acknum = packet.seqnum;
    packet_prev_ack.seqnum = 0;
    strcpy(packet_prev_ack.payload, " ");
    packet_prev_ack.checksum = checksum(packet_prev_ack);
//send ack now. 
    tolayer3(B, packet_prev_ack);

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
float lossprob; /* probability that a packet is dropped  */
float corruptprob; /* probability that one bit is packet is flipped */
float lambda; /* arrival rate of messages from layer 5 */
int ntolayer3; /* number sent into layer 3 */
int nlost; /* number lost in media */
int ncorrupt; /* number corrupted by media*/

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
    printf("Protocol: Alternating Bit\n");
    printf("%d of packets sent from the Application Layer of Sender A\n", no_packet_sent_5to4A);
    printf("%d of packets sent from the Transport Layer of Sender A\n", no_packet_sent_4to3A);
    printf("%d of packets received at the Transport layer of Receiver B\n", no_packet_recd_3to4B);
    printf("%d of packets received at the Application layer of Receiver B\n", no_packet_recd_4to5B);
    printf("Total time: %f time units\n", time);
    printf("Throughput = %f packets/units\n", (no_packet_recd_4to5B / time));
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

