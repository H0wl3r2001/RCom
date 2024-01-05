/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <math.h>

#define BAUDRATE B2400
#define MODEMDEVICE "/dev/ttyS1"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1

#define FLAG 0x7E
#define CMD 0x03
#define ANS 0x01
#define SET 0x03
#define DISC 0x0B
#define UA 0x07
#define ESC 0x7D
#define RR 0x05
#define RR8 0x85
#define REJ 0x01
#define REJ8 0x81
#define TRANSMITTER 0
#define RECEIVER 1
#define DATA 1
#define START_ 2
#define END 3
#define NOTHING 0x00
#define MAX_SIZE 256


unsigned char nsequence = 0;
unsigned char nSequencePacket = 0;
int info_frame_size; 

struct termios oldtio,newtio;

enum States
{
  START , FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP 
};

unsigned char infoFrame[MAX_SIZE * 2];

int flag_signal=1, conta=1;

void atende()                   // atende alarme
{
	printf("alarme # %d\n", conta);
	flag_signal=1;
	conta++;
}

int sendFrame (int fd, unsigned char * frame, int length)
{
  int res = write(fd, frame, length);
  alarm(3);
  flag_signal = 0;
  return res;
}

int printMsg(unsigned char* msg, int length)
{
  //printf("--------------------\n");
  for(int i = 0; i < length; i++)
    {
        //printf("%02X\t", msg[i]);
    }
  //printf("--------------------\n");
}

unsigned char checkingOneResponse(int fd, unsigned char * frame_to_retransmit, unsigned address_field, unsigned char control_field)
{
  enum States state;
  unsigned char buf[5];
  conta = 0;
  
  int res, i, no_error = 1;

  while(conta < 4){
    
    if (flag_signal){
      sendFrame(fd,frame_to_retransmit,5);
      no_error = 1;
    }

    if(no_error)
    {
      state = START;
      i=0;
      while(state != STOP && !flag_signal)
      {
        res = read(fd,&buf[i],1) ;

        switch (state)
        {
          case START:
            if(buf[i] == FLAG) state = FLAG_RCV;
            break;
          case FLAG_RCV:
            if(buf[i] == address_field) state = A_RCV;
            else if (buf[i] == FLAG) state = FLAG_RCV;
            else state = START;
            break;
          case A_RCV:
            if(buf[i] == control_field) state = C_RCV;
            else if (buf[i] == FLAG) state = FLAG_RCV;
            else state = START;
            break;
          case C_RCV:
            if(buf[i] == (address_field^control_field)) state = BCC_OK;
            else if (buf[i] == FLAG) state = FLAG_RCV;
            else state = START;
            break;
          case BCC_OK:
            if (buf[i] == FLAG) state = STOP;
            else state = START;
            break;
        }
      }
      if(state == STOP)
      {
        alarm(0);
        return 0;
      } 
    }    
  }
  return 1;
}

unsigned char checkingFourResponse(int fd, unsigned address_field, unsigned char alt1, unsigned char alt2, unsigned char alt3, unsigned char alt4)
{
  enum States state;
  unsigned char buf[5];
  
  int res, i;
  unsigned char found = 0x00;
  conta = 1;

  while(conta < 4){

    if (flag_signal){
      alarm(0);
      sendFrame(fd,infoFrame,info_frame_size);
      
    }

    state = START;
    i=0;
    while(state != STOP && !flag_signal)
    {
      
      res = read(fd,&buf[i],1);
      
      switch (state)
      {
        case START:
          if(buf[i] == FLAG) state = FLAG_RCV;
          break;
        case FLAG_RCV:
          if(buf[i] == address_field) state = A_RCV;
          else if (buf[i] == FLAG) state = FLAG_RCV;
          else state = START;
          break;
        case A_RCV:
          if((buf[i] == alt1) || (buf[i] == alt2) || (buf[i] == alt3) || (buf[i] == alt4)) 
          {
            state = C_RCV;
            found = buf[i];
          }
          else if (buf[i] == FLAG) state = FLAG_RCV;
          else state = START;
          break;
        case C_RCV:
          if(buf[i] == (address_field^found)) state = BCC_OK;
          else if (buf[i] == FLAG) state = FLAG_RCV;
          else state = START;
          break;
        case BCC_OK:
          if (buf[i] == FLAG) state = STOP;
          else state = START;
          break;
      }
    }
    if(state == STOP)
    {
      
      if(found == REJ || found == REJ8)
      {
        conta++;
        alarm(0);
        sendFrame(fd, infoFrame, info_frame_size);
        state = START;  
        continue;
      }
      else 
      {
        return found;
      }
    } 
  }
  return 1;
}          

int byteStuffing(unsigned char * buffer, int length)
{
  int j = 1;
  infoFrame[0] = buffer[0];
  int changes = 0;

  for (int i = 1; i < length - 1; i++)
  {
    if( buffer[i] == FLAG || buffer[i] == ESC)
    {
      infoFrame[j] = ESC;
      j++;
      changes++;
      infoFrame[j] = buffer[i] ^ 0x20;
    }
    else infoFrame[j] = buffer[i];
    j++;
  
  }
  infoFrame[j] = buffer[length - 1];
  j += 1;
  return j;
}

int setSettings(int fd) 
{
  if ( tcgetattr(fd,&oldtio) == -1) { /* save current port settings */
      perror("tcgetattr");
      exit(-1);
    }

    bzero(&newtio, sizeof(newtio));
    newtio.c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = IGNPAR;
    newtio.c_oflag = 0;

    /* set input mode (non-canonical, no echo,...) */
    newtio.c_lflag = 0;

    newtio.c_cc[VTIME]    = 0.1;   /* inter-character timer unused */
    newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */

    /* 
      VTIME e VMIN devem ser alterados de forma a proteger com um temporizador a 
      leitura do(s) pr�ximo(s) caracter(es)
    */

    tcflush(fd, TCIOFLUSH);

    if ( tcsetattr(fd,TCSANOW,&newtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
  
 
}

int createFrame(unsigned char * frame, unsigned char address_field, unsigned char control_field)
{
  frame[0] = FLAG;
  frame[1] = address_field;
  frame[2] = control_field;
  frame[3] = address_field^control_field;
  frame[4] = FLAG;
}

revertSettings(int fd)
{
  if ( tcsetattr(fd,TCSANOW,&oldtio) == -1) {
      perror("tcsetattr");
      exit(-1);
    }
}


int llopen(int porta, unsigned char flag)
{
  char path[12] = "/dev/ttyS";
  char port = porta + '0';
  memcpy(path + strlen(path) , &port,1);

  int fd = open(path, O_RDWR | O_NOCTTY );

  if (fd <0) {perror(path); return -1; }
  setSettings(fd);
  
  unsigned char ua_expected[5];
  unsigned char set[5];

  createFrame(ua_expected, CMD, UA);
  createFrame(set, CMD, SET); 

  sendFrame(fd, set, 5);

  unsigned char res = checkingOneResponse(fd, set, CMD, UA);
  if(res == 1) return -1; 
  
  return fd;
}

int createControlPacket(unsigned char * packet, unsigned char control_field, unsigned char type, unsigned char length,  unsigned char * value)
{
  packet[0] = control_field;
  packet[1] = type;
  packet[2] = length;
  memcpy(packet+3, value, length);
}

int createDataPacket(unsigned char * packet, unsigned char l1, unsigned char l2, unsigned char * data)
{
  packet[0] = DATA;
  packet[1] = nSequencePacket;
  packet[2] = l2;
  packet[3] = l1;
  
  int k = l2*256 + l1;
  nSequencePacket = (nSequencePacket + 1) % 256;
  memcpy(packet+4, data, k);
  
  return k + 4;
}

void addFrameHeader(unsigned char * frame)
{
  frame[0] = FLAG;
  frame[1] = CMD;
  frame[2] = nsequence;
  frame[3] = CMD ^ nsequence;
  nsequence = (nsequence + 1)%2;
} 

unsigned char calculateBCC (unsigned char * frame, int length)
{
  unsigned char bcc = 0;
  for(int i = 0; i < length; i++)
  { 
    bcc = bcc ^ frame[i];
  }
  return bcc;
}

void addFrameTrailer(unsigned char * frame, unsigned char bcc,  int pos)
{
  frame[pos] = bcc;
  frame[pos+1] = FLAG;
}

int llwrite(int fd, unsigned char * buffer, int length)
{
  unsigned char frame [length + 6];

  addFrameHeader(frame);
  memcpy(frame + 4, buffer, length);

  unsigned bcc = calculateBCC(buffer, length);
  addFrameTrailer(frame, bcc,  4 + length);

  int nbytes = byteStuffing(frame, length + 6);

  info_frame_size = nbytes;
  int res = sendFrame(fd, infoFrame, info_frame_size);

  unsigned char response = checkingFourResponse(fd, CMD, RR, RR8, REJ, REJ8);
  if(response == 1) return -1;

  alarm(0);
  
  return res;
}

int llclose(int fd)
{
  unsigned char disc[5];
  unsigned char ua[5];

  createFrame(disc, CMD, DISC);
  createFrame(ua, ANS, UA);

  sendFrame(fd, disc, 5);
  
  checkingOneResponse(fd, disc, ANS, DISC);
  sendFrame(fd, ua, 5);

  return 0;
}

int intToArrayChar(int value, unsigned char * result)
{
  int i = 0;
  while(value > 0)
  {
    result[i] = value % 256;
    value /= 256;
    i++;
  }
  return i;
}

int startAndEndApiConnection(int fd, unsigned char control_field, int img_size)
{
  unsigned char packet[5];
  unsigned char nbytesneeded;
  unsigned char result[5];

  nbytesneeded = intToArrayChar(img_size, result);

  createControlPacket(packet, control_field, 0, nbytesneeded , result);
  int lengthControloPacket = 3 + nbytesneeded;

  return llwrite(fd, packet, lengthControloPacket);
}

int apiWrite(int fd, unsigned char * buffer, int length)
{
  unsigned char packet[MAX_SIZE];
  unsigned char result[2];
  int nbytesneeded;

  nbytesneeded = intToArrayChar(length, result);
  if(nbytesneeded == 1)
  {
    result[1] = result[0];
    result[0] = 0x00;
  }
  
  int data_packet_size = createDataPacket(packet,result[1], result[0], buffer);

  return llwrite(fd, packet, data_packet_size);
}


int main(int argc, char** argv)
{
    int fd,c, res;
    
    (void) signal(SIGALRM, atende);

    int i = 0, sum = 0, speed = 0;
    
    if ( (argc < 2) || 
  	     ((strcmp("/dev/ttyS0", argv[1])!=0) && 
  	      (strcmp("/dev/ttyS1", argv[1])!=0) )) //mudar para testes
  	      {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */

    int porta = ((int) (argv[1][strlen(argv[1]) - 1]) - '0');

    
    fd = llopen(porta, TRANSMITTER);
    if(fd < 0){ perror("error when trying openning.."); return -1;} 

    int fdImg = open("pinguim.gif", O_RDONLY);
    struct stat sb;

    if(fstat(fdImg, &sb) == -1)
    {
      perror("stat");
      exit(1);
    }

    int img_size = (int) sb.st_size;

    if(startAndEndApiConnection(fd, START_, img_size) < 0) {
        perror("tried 3 times\n");
        return -1;
      } 


    unsigned char buffer[10968];
    int end = 0;

    int frame_size = 128;
    int mod = img_size % frame_size;
    int j = 0;

    struct timeval before_sending;
    struct timeval after_sending;

    gettimeofday(&before_sending, NULL);
    while(end < (img_size - mod))
    {
      for(int i = 0; i < frame_size; i++)
      {
        read(fdImg, &buffer[i], 1);    
      }
      
      if(apiWrite(fd, buffer, frame_size) < 0) 
      {
        perror("tried 3 times\n");
        return -1;
      }

      end += frame_size;
      j++;
    }
   
    for(int i = 0; i < mod; i++) read(fdImg, &buffer[i], 1);
  
    apiWrite(fd, buffer, mod);

    gettimeofday(&after_sending, NULL);
    time_t numSec = after_sending.tv_sec - before_sending.tv_sec;
    suseconds_t uSec = after_sending.tv_usec - before_sending.tv_usec;
    printf("tempo decorrido em segundos : %ld \t tempo em usec : %ld", numSec, uSec);
    
    startAndEndApiConnection(fd, END, img_size);

    llclose(fd);
    
    revertSettings(fd);

    close(fd);
    close(fdImg);
  
    return 0;
}
