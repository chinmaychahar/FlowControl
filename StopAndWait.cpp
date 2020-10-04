#include<iostream>
#include<stdio.h>
#include<stdlib.h>
using namespace std;

#define time 5
#define max_seq 1
#define tot_pack 3
#define inc(k) if(k<max_seq)k++;else k=0;

typedef struct {
	int data;
}packet;

typedef struct {
	int kind;
	int seq;
	int ack;
	int error;
	packet info;
}frame;

frame DATA;

typedef enum {
	frame_arrival, error, timeout,
}event_type;

void from_network_layer(packet *);
void to_network_layer(packet *);
void from_physical_layer(frame *);
void to_physical_layer(frame *);
void sender();
void receiver();
void wait_for_event_sender(event_type *);
void wait_for_event_receiver(event_type *);

int i=1;
char turn;
int disconnect=0;
int main() {
	while( !disconnect) {
		sender();
		receiver();	
	}
	return 0;
}
	
void sender() { 
	
	static int frame_to_send = 0;
	static frame s;
	packet buffer;
	event_type event;
	static int flag=0;
	
	if(flag==0) {
		from_network_layer(&buffer);
		s.info = buffer;
		s.seq = frame_to_send;
		cout<<"SENDER: "<<s.info.data;
		cout<<" | Sequence No: "<<s.seq;
		
		turn ='r';
		to_physical_layer(&s);
		flag=1;
	}
	
	wait_for_event_sender(&event);
	if(turn=='s')
 	{
     if(event==frame_arrival)
      {
      from_network_layer(&buffer);
      inc(frame_to_send);
      s.info=buffer;
      s.seq=frame_to_send;
      cout<<"\nSENDER: "<<s.info.data;
      cout<<" | Sequence no:"<<s.seq;

      turn='r';
      to_physical_layer(&s);
      }
	}
	 if(event==timeout)
    {
      cout<<"\nSENDER: Resending Frame! "<<endl;
      turn = 'r';
      to_physical_layer(&s);
    }	
}

void receiver()
{
     static int frame_expected=0;
     frame s,r;
     event_type event;
     wait_for_event_receiver(&event);
     
     if(turn=='r') {  
	 	if(event==frame_arrival) {
	 		
          from_physical_layer(&r);
          
          if(r.seq==frame_expected) {
          	
       		to_network_layer(&r.info);
     		inc (frame_expected);
    }
    else
    cout<<"\nRECEIVER: Acknowledgement resent! "<<endl;

    turn='s';
    to_physical_layer(&s);
    }
    
    if(event==error)
    {
      cout<<"RECEIVER: Garbled Frame!"<<endl;
      turn = 's';     
    }                 
  }
}     

void from_network_layer(packet *buffer)
{
     (*buffer).data=i;
     i++;
}  

void to_physical_layer(frame *s)
{
	s->error = rand()%5;
     DATA=*s;
}

void from_physical_layer(frame *buffer)
{
    *buffer=DATA;
}

void to_network_layer(packet *buffer)
{
     cout<<"\nRECEIVER: Packet "<< i-1 <<" received";
     cout<<"\nAcknowledgement sent!"<<endl;
     
     if(i>tot_pack)
      { disconnect=1;
 		cout<<"\nDISCONTINUE";
      }
} 

void wait_for_event_sender(event_type * e)
{
	 static int timer=0;
	 if(turn=='s')
	  {
	   timer++;
	   if(timer==timeout)
	    {
	     *e = timeout;
	     cout<<"\nSENDER: Acknowledgment not recieved! \n-------- TIMEOUT -------";
	     timer = 0;
	     return;
	    }
	   if(DATA.error==0)
	     *e = error;
	   else
	    {
	     timer = 0;
	     *e = frame_arrival;
	    }
	  }
}

void wait_for_event_receiver(event_type *e)
{
     if(turn=='r')
     {
     	if(DATA.error==0)
		  {
   			*e=frame_arrival;	
 	 	 }
     }
}

