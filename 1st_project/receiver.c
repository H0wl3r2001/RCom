/*Non-Canonical Input Processing*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>


#define BAUDRATE B2400
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define FLAG 0x7E
#define FLAG_STUFFED_BYTE_0 0x7D
#define FLAG_STUFFED_BYTE_1 0x5E
#define ESC 0x7D
#define CMD 0x01
#define ANS 0x03
#define UA 0x07 
#define SET 0x03 // Alterar
#define RR 0x05
#define REJ 0x01
#define DISC 0x0B
#define MAX_SIZE 256

#define TRANSMITTER 0
#define RECEIVER 1

unsigned char infoFrame[MAX_SIZE];
unsigned char nreceiver = 1;
unsigned char nsq = 0;
int img_size;
int supertux_fd;
struct termios oldtio,newtio;

enum States
{
  START , FLAG_RCV, A_RCV, C_RCV, BCC_OK, STOP 
};

unsigned char calculateBCC (unsigned char * frame, int length)
{
  unsigned char bcc = 0;
  for(int i = 0; i < length; i++)
  { 
    bcc = bcc ^ frame[i];
  }
  return bcc;
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
    newtio.c_cc[VMIN]     = 1;   /* blocking read until 1 chars received */

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


int checkingFrame(int fd, unsigned char address_field, unsigned char control_field)
{
  enum States state = START;
  unsigned char buf[5];
  int res, i = 0;

  while(state != STOP)
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
  return 0;
}

int llopen(int porta, unsigned char flag)
{
  char path[12] = "/dev/ttyS";
  char port = porta + '0';
  unsigned char ua[5];

  memcpy(path + strlen(path) , &port,1);

  int fd = open(path, O_RDWR | O_NOCTTY );

  if (fd <0) {perror(path); exit(-1);}

  setSettings(fd);

  createFrame(ua, ANS, UA);
  checkingFrame(fd, ANS, SET);  

  int res = write(fd,ua,5);
  
  return fd;
}

int removeFrameTrailer(int pos)
{
  infoFrame[pos-1] = 0x00;
  infoFrame[pos-2] = 0x00;
  return 0;
}

int removeFrameHeader(int nbytesToCpy)
{
  memcpy(infoFrame, infoFrame + 4, nbytesToCpy);
  return 0;
}

int removePacketHeader(int pos, int nbytesToCpy)
{
  memcpy(infoFrame, infoFrame + pos, nbytesToCpy);
  return 0;
}
  
int llwrite(int fd, char * buffer, int length)
{
  write(fd,buffer, length);
  return 1;
}  

int pot(int x , int n)
{
  int p = 1;
  for(int i  =0; i < n;i++)
  {
    p = p * x;
  }
  return p;
}

int arrayCharToInt(unsigned char * value, unsigned char length)
{
  int res = 0;
  for(int i = 0; i< length;i++)
  {
    res += value[i] * pot(256, i);
  }
  return res;
}

int apiread(int fd)
{
  if(infoFrame[0] == 1) 
  { 
    if(infoFrame[1] == nsq)
    {
      nsq += 1;
      int k = 256 * infoFrame[2] + infoFrame[3];
    
      memcpy(infoFrame, infoFrame + 4, k);

      write(supertux_fd, infoFrame, k);
    }
  }
  else if(infoFrame[0] == 2)
  {
    supertux_fd = open("supertux.gif", O_WRONLY |O_TRUNC | O_CREAT, 0777);
    unsigned char result [infoFrame[2]];
    memcpy(result, infoFrame + 3, 2);

    img_size = arrayCharToInt(result, infoFrame[2]);
  } 
  else if (infoFrame[0] == 3)
  {
    llclose(fd);
    return 3;
  }
  else return -1;
  
}

int checkBCC1(int pos)
{
  return (infoFrame[pos-2] ^ infoFrame[pos-1]) == infoFrame[pos];
}

int checkBCC2(int pos)
{
  unsigned char bcc = calculateBCC(infoFrame, pos);
  return bcc == infoFrame[pos];
}

int llread(int fd, unsigned char * buffer)
{
  int acc = 2, i=0, res;
  int stop1 = 0, starting_frame = 0;
  memset(infoFrame, 0, 256);
  memset(buffer, 0, 256);

  while(!stop1)
  {
    res = read(fd, &buffer[i],1);
    if(buffer[i] == FLAG && !starting_frame)
    {
      i = 0;
      buffer[0] = FLAG;
      
      starting_frame = 1;
    }
    else if(buffer[i] == FLAG && starting_frame && buffer[i-1] != FLAG)
    {
      stop1 = 1;
    }
    else if(buffer[i] == FLAG && buffer[i-1] == FLAG)
    {
      i--;
    }
    i++; 
  }

  int numBytesAfterDestuffing = byteDestuffing(buffer, i);
  unsigned char nsSender = infoFrame[2];
  unsigned char response[5];

  if(!checkBCC1(3))
  {
    createFrame(response, ANS, REJ | (nreceiver << 7));
    write(fd, response,5);
    tcflush(fd, TCIOFLUSH);
    return -1;
  } 
  removeFrameHeader(numBytesAfterDestuffing-4);

  if (((nsSender + 1)%2) != nreceiver)
  {
    createFrame(response, ANS, RR | (nreceiver << 7));
    int x = write(fd, response,5);
    return 0;
  }
  
  else if((((nsSender + 1)%2) == nreceiver) && checkBCC2(numBytesAfterDestuffing-6)) 
  {
    createFrame(response, ANS, RR | (nreceiver << 7));
    write(fd, response,5);
    nreceiver = (nreceiver + 1) % 2;
    
  }
  else if((((nsSender + 1)%2) == nreceiver) && !checkBCC2(numBytesAfterDestuffing-6))
  {
    createFrame(response, ANS, REJ | (nreceiver << 7));
    write(fd, response,5);
    tcflush(fd, TCIOFLUSH);
    return -1;
  } else
  {
    createFrame(response, ANS, RR | (nreceiver << 7));
    write(fd, response,5);
  } 
 
  removeFrameTrailer(numBytesAfterDestuffing - 3);

  return numBytesAfterDestuffing - 5;
}

int llclose(int fd)
{
  unsigned char disc[5];

  createFrame(disc, CMD, DISC);
  checkingFrame(fd, ANS ,DISC);
  write(fd, disc, 5);
  checkingFrame(fd, CMD, UA);
  
  return 0;
}

int byteDestuffing(unsigned char * buffer, int length)
{
  int i = 0, j = 0;

  for (int i = 0; i < length; i++)
  {
    if(buffer[i] == ESC)
    {
      if(buffer[i+1] == 0x5E)
      {
        
        infoFrame[j] = FLAG;
      }
      else infoFrame[j] = ESC;
      i++;
    }
    else infoFrame[j] = buffer[i];
    j++;
    
  }
  return j;
}

int main(int argc, char** argv)
{
    int fd,c;
    struct termios oldtio,newtio;
    unsigned char buf[1];

    if ( (argc < 2) ||
      ((strcmp("/dev/ttyS0", argv[1])!=0) &&
       (strcmp("/dev/ttyS1", argv[1])!=0) )) {
      printf("Usage:\tnserial SerialPort\n\tex: nserial /dev/ttyS1\n");
      exit(1);
    }

  /*
    Open serial port device for reading and writing and not as controlling tty
    because we don't want to get killed if linenoise sends CTRL-C.
  */
    int porta = ((int) (argv[1][strlen(argv[1]) - 1]) - '0');
  
    fd = llopen(porta, RECEIVER);
    
    unsigned char buffer [MAX_SIZE];
    int res = 0, i = 0;

    while(res != 3)
    {
      int res1 = llread(fd, buffer);
      if(res1 > 1) res1 = apiread(fd);  
    }

    sleep(1);
    revertSettings(fd);
    
    close(fd);
    close(supertux_fd);
    return 0;
}
