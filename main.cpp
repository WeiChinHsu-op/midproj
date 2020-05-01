#include "mbed.h"
#include "DA7212.h"
#include "uLCD_4DGL.h"
#include <cmath>

DA7212 audio; 
uLCD_4DGL uLCD(D1, D0, D2);
InterruptIn sw3(SW3);
DigitalOut led(LED1);


int16_t waveform[kAudioTxBufferSize];
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;
int play = 0;

int songmute[3] ={50000,50000,50000};

int song0[42] = {
  261, 261, 392, 392, 440, 440, 392,
  349, 349, 330, 330, 294, 294, 261,
  392, 392, 349, 349, 330, 330, 294,
  392, 392, 349, 349, 330, 330, 294,
  261, 261, 392, 392, 440, 440, 392,
  349, 349, 330, 330, 294, 294, 261};

int noteLength0[42] = {
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2,
  1, 1, 1, 1, 1, 1, 2};

int song1[32] = {
  392,330,330,330,349,294,294,294,
  261,294,330,349,392,392,392,392,
  392,330,330,330,349,294,294,294,
  261,330,392,392,261,261,261,261};

int noteLength1[32] = {
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1};

int song2[32] = {
  261,294,330,261,261,294,330,261,
  330,349,392,392,330,349,392,392,
  440,349,330,261,440,349,330,261,
  261,294,261,261,261,294,261,261};

int noteLength2[32] = {
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1};

void ISR1(){
    led = !led;
    queue.cancel(play);
    wait(5.0);
}

void playNote(int freq)
{
   for (int i = 0; i < kAudioTxBufferSize; i++)
   {
     waveform[i] = (int16_t) (sin((double)i * 2. * M_PI/(double) (kAudioSampleFrequency / freq)) * ((1<<16) - 1));
   }
      // the loop below will play the note for the duration of 1s
   for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
   {
     audio.spk.play(waveform, kAudioTxBufferSize);
   }
}

int main(){
    
    
    
    uLCD.background_color(WHITE);
    uLCD.textbackground_color(WHITE);
    uLCD.color(BLUE);
    char songs[] = "1.star 2.bee 3.tiger ";
    uLCD.text_width(1); //4X size text
    uLCD.text_height(1);
    uLCD.printf("%s\n",songs);
    
    
    
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    int select=0;
    sw3.fall(&ISR1);
    while(true){
      if(sw3==0){
        queue.call(playNote, songmute[1]);
      }
      if(sw3==1){
        while(true){
          if(sw3==0)
          break;
        if(select==0){
            uLCD.color(RED);
            uLCD.locate(1,2);
            uLCD.printf("Your song : \n 1.star    \n");
            for(int i = 0; i < 42; i++)
            {
              
              int length = noteLength0[i];
              while(length--)
              {
                play = queue.call(playNote, song0[i]);
                if(length <= 1) wait(1.0);
                if(sw3==0)
                break;
                
              }
              if(sw3==0)
                break;
            }
            
            select++;
        }
        else if (select==1){
            uLCD.color(RED);
            uLCD.locate(1,2);
            uLCD.printf("Your song : \n 2.bee    \n");
            for(int i = 0; i < 32; i++)
            {
              
              int length = noteLength1[i];
              while(length--)
              {
                play = queue.call(playNote, song1[i]);
                if(length <= 1) wait(1.0);
                if(sw3==0)
                break;
              }
              if(sw3==0)
                break;
            }
            select++;
            
        }
        else{
            uLCD.color(RED);
            uLCD.locate(1,2);
            uLCD.printf("Your song : \n 3.tiger   \n");
            for(int i = 0; i < 32; i++)
            {
              
              int length = noteLength2[i];
              while(length--)
              {
                play = queue.call(playNote, song2[i]);
                if(length <= 1) wait(1.0);
                if(sw3==0)
                break;
              }
              if(sw3==0)
                break;
            }
            
            select=0;
        }
    
    }
  }
  } 
}