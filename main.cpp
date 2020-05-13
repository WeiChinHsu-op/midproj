#include "DA7212.h"
#include "accelerometer_handler.h"
#include "config.h"
#include "magic_wand_model_data.h"
#include "mbed.h"
#include "uLCD_4DGL.h"
#include <cmath>

#include "tensorflow/lite/c/common.h"
#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

#define bufferLength (32)
#define signalLength (96)

DA7212 audio; 
Serial pc( USBTX, USBRX );
Timer debounce;
InterruptIn sw3(SW3);
InterruptIn sw2(SW2);
DigitalOut green_led(LED2);

uLCD_4DGL uLCD(D1, D0, D2);
int16_t waveform[kAudioTxBufferSize];

EventQueue queue2(32 * EVENTS_EVENT_SIZE);
Thread thread2;
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;
//int countload = 0;
bool should_clear_buffer = false;
bool got_data = false;
constexpr int kTensorArenaSize = 60 * 1024;
uint8_t tensor_arena[kTensorArenaSize];

bool should_clear_buffer1 = false;
bool got_data1 = false;
constexpr int kTensorArenaSize1 = 60 * 1024;
uint8_t tensor_arena1[kTensorArenaSize1];

int nowsong=2 ;
int gesture_index_taiko = 0;
int gesture_index = 0;
int mode = 0;

int signal1[signalLength];
char serialInBuffer[bufferLength];
int serialCount = 0;

int count_taiko = 0;
int countprevious;
int score = 5;
int abstaiko[48] = {0};
int answer[48] = {0,0,0,0,0,0,0,0,0,0,0,0,\
                    0,0,0,0,1,2,1,0,\
                    1,2,1,0,1,2,2,0,1,2,2,0,\
                    2,1,2,0,2,1,2,0,\
                    1,1,2,1,1,1,2,1};

int taiko = 0;
int countscore = 0;

int songmute[3] ={50000,50000,50000};

int song1[48] = {
  261,294,330,349,392,440,485,529,
  261,294,330,349,392,440,485,529,
  330,294,261,294,330,330,330,330,\
  294,294,294,294,330,392,392,392,\
  330,294,261,294,330,330,330,330,\
  294,294,330,294,261,261,261,261};

int noteLength2[48] = {
  1, 1, 1, 1, 1, 1,\
  1, 1, 1, 1, 1, 1,\
  1, 1, 1, 1, 1, 1,\
  1, 1, 1, 1, 1, 1,\
  1, 1, 1, 1, 1, 1,\
  1, 1, 1, 1, 1, 1,\
  1, 1, 1, 1, 1, 1,\
  1, 1, 1, 1, 1, 1};

int noteLength1[96] = {
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1,
  1,1,1,1,1,1,1,1};

void playNote(int freq)
{
   for (int i = 0; i < kAudioTxBufferSize; i++)
   {
     waveform[i] = (int16_t) (sin((double)i * 2. * M_PI/(double) (kAudioSampleFrequency / freq)) * ((1<<16) - 1));
   }
      // the loop below will play the note for the duration of 1s
   //for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
   //{
     audio.spk.play(waveform, kAudioTxBufferSize);
   //}
}

void loadSignal(void)
{
  
  //pc.printf("\n")
  int i = 0;
  //pc.printf("%d",i);
  green_led = 0;
  
  serialCount = 0;
  //audio.spk.pause();
  while(i < signalLength)
  {
    if(pc.readable())
    {
      serialInBuffer[serialCount] = pc.getc();
      serialCount++;
      if(serialCount == 3)
      {
        serialInBuffer[serialCount] = '\0';
        //uLCD.printf("%s ",serialInBuffer);
        signal1[i] = (int) atoi(serialInBuffer);
        serialCount = 0;
        i++;
      }
    }
    if(sw3==0)
    break;
  }
  //debounce.reset();
  green_led = 1;
  if(nowsong==0){
    
    for(int i = 0; i < 32; i++)
    {
      int length = noteLength1[i];
      //uLCD.printf("playing");
      while(length--)
      { 
       //playNote(signal1[i]);
        playNote(signal1[i]);
        if(length <= 1) wait(1);
        if(sw3==0){
        //playNote(songmute[1]);
        break;
        }
      }
    
      if(sw3==0){
        //playNote(songmute[1]);
        break;
      }
    }
  }
  if(nowsong==1){
    
    for(int i = 32; i < 64; i++)
    {
      int length = noteLength1[i];
      //uLCD.printf("playing");
      while(length--)
      { 
       //playNote(signal1[i]);
        playNote(signal1[i]);
        if(length <= 1) wait(1);
        if(sw3==0){
        //playNote(songmute[1]);
        break;
        }
      }
    
      if(sw3==0){
        //playNote(songmute[1]);
        break;
      }
    }
  }
  if(nowsong==2){
    
    for(int i = 64; i < 96; i++)
    {
      int length = noteLength1[i];
      //uLCD.printf("playing");
      while(length--)
      { 
       //playNote(signal1[i]);
        playNote(signal1[i]);
        if(length <= 1) wait(1);
        if(sw3==0){
        //playNote(songmute[1]);
        break;
        }
      }
    
      if(sw3==0 ){
        //playNote(songmute[1]);
        break;
      }
    }
  }
  //pc.printf("5");
  //
}

void playing(){
  for(int i = 0; i < 32; i++)
  {
    int length = noteLength1[i];
    uLCD.printf("playing");
    while(length--)
    {
      //playNote(signal1[i]);
      playNote(signal1[i]);
      if(length <= 1) wait(1);
      if(sw3==0 && debounce.read_ms()>2000){
      playNote(songmute[1]);
      break;
      }
    }
    
    if(sw3==0 && debounce.read_ms()>2000){
      playNote(songmute[1]);
      break;
    
    }
    
  }
}


int PredictGesture(float* output) {
    // How many times the most recent gesture has been matched in a row
    static int continuous_count = 0;
    // The result of the last prediction
    static int last_predict = -1;
  
    // Find whichever output has a probability > 0.8 (they sum to 1)
    int this_predict = -1;
    for (int i = 0; i < label_num; i++) {
      if (output[i] > 0.8) this_predict = i;
    }
  
    // No gesture was detected above the threshold
    if (this_predict == -1) {
      continuous_count = 0;
      last_predict = label_num;
      return label_num;
    }
  
    if (last_predict == this_predict) {
      continuous_count += 1;
    } else {
      continuous_count = 0;
    }
    last_predict = this_predict;
  
    // If we haven't yet had enough consecutive matches for this gesture,
    // report a negative result
    if (continuous_count < config.consecutiveInferenceThresholds[this_predict]) {
      return label_num;
    }
    // Otherwise, we've seen a positive result, so clear all our variables
    // and report it
    continuous_count = 0;
    last_predict = -1;
  
    return this_predict;
}

void taiko1(){
    uLCD.cls();
    uLCD.reset();
    uLCD.background_color(WHITE);
    uLCD.textbackground_color(WHITE);
    uLCD.color(BLUE);
    char songs[] = "12101210\n\n12201220\n\n21202120\n\n11211121";
    uLCD.text_width(1); //4X size text
    uLCD.text_height(1);
    uLCD.printf("%s\n",songs);

    //uLCD.printf("playing");
  for(int i = 0; i < 48 ; i++)
  {
    int length = noteLength2[i];
    
    while(length--)
    {
      //playNote(signal1[i]);
      playNote(song1[i]);
      if(length <= 1) wait(1);
    }
  }
}

int ML_acc_song(){
    int count = 0;

    // Set up logging.
    static tflite::MicroErrorReporter micro_error_reporter;
    tflite::ErrorReporter* error_reporter = &micro_error_reporter;
  
    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    const tflite::Model* model = tflite::GetModel(g_magic_wand_model_data);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
      error_reporter->Report(
          "Model provided is schema version %d not equal "
          "to supported version %d.",
          model->version(), TFLITE_SCHEMA_VERSION);
      //return -1;
    }
    // Pull in only the operation implementations we need.
    // This relies on a complete list of all the ops needed by this graph.
    // An easier approach is to just use the AllOpsResolver, but this will
    // incur some penalty in code space for op implementations that are not
    // needed by this graph.
    static tflite::MicroOpResolver<6> micro_op_resolver;
    micro_op_resolver.AddBuiltin(
        tflite::BuiltinOperator_DEPTHWISE_CONV_2D,
        tflite::ops::micro::Register_DEPTHWISE_CONV_2D());
    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_MAX_POOL_2D,
                                 tflite::ops::micro::Register_MAX_POOL_2D());
    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_CONV_2D,
                                 tflite::ops::micro::Register_CONV_2D());
    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_FULLY_CONNECTED,
                                 tflite::ops::micro::Register_FULLY_CONNECTED());
    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_SOFTMAX,
                                 tflite::ops::micro::Register_SOFTMAX());
    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_RESHAPE,
                                 tflite::ops::micro::Register_RESHAPE(), 1);
   // Build an interpreter to run the model with
   static tflite::MicroInterpreter static_interpreter(
       model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
   tflite::MicroInterpreter* interpreter = &static_interpreter;

   // Allocate memory from the tensor_arena for the model's tensors
   interpreter->AllocateTensors();
 
   // Obtain pointer to the model's input tensor
   TfLiteTensor* model_input = interpreter->input(0);
   if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
       (model_input->dims->data[1] != config.seq_length) ||
       (model_input->dims->data[2] != kChannelNumber) ||
       (model_input->type != kTfLiteFloat32)) {
     error_reporter->Report("Bad input tensor parameters in model");
     //return -1;
   }
 
   int input_length = model_input->bytes / sizeof(float);
 
   TfLiteStatus setup_status = SetupAccelerometer(error_reporter);
   if (setup_status != kTfLiteOk) {
     error_reporter->Report("Set up failed\n");
     //return -1;
   }
 
   //error_reporter->Report("Set up successful...\n");
 
   while (true) {
      
     // Attempt to read new data from the accelerometer
     got_data = ReadAccelerometer(error_reporter, model_input->data.f,
                                  input_length, should_clear_buffer);
 
     // If there was no new data,
     // don't try to clear the buffer again and wait until next time
     if (!got_data) {
       should_clear_buffer = false;
       continue;
     }
 
     // Run inference, and report any error
     TfLiteStatus invoke_status = interpreter->Invoke();
     if (invoke_status != kTfLiteOk) {
       error_reporter->Report("Invoke failed on index: %d\n", begin_index);
       continue;
     }
 
     // Analyze the results to obtain a prediction
     gesture_index = PredictGesture(interpreter->output(0)->data.f);
 
     // Clear the buffer next time we read data
     should_clear_buffer = gesture_index < label_num;
 
     // Produce an output
     count++;
     if (gesture_index < label_num && count%15==0) {
       //pc.printf("%d",gesture_index);
        if(gesture_index==1){
          if(nowsong==0)
            nowsong = 2;
          else
            nowsong--;
          //pc.printf("nowsong :%d",nowsong);
          //pc.printf("%d",nowsong);
          uLCD.locate(1,11);
          uLCD.printf("nowsong: %d\r\n",nowsong+1);
        }
        if(gesture_index==2){
          if(nowsong==2)
            nowsong = 0;
          else
            nowsong++;
          //pc.printf("nowsong :%d",nowsong); 
          //pc.printf("%d",nowsong);
          uLCD.locate(1,11);
          uLCD.printf("nowsong: %d\r\n",nowsong+1);
        }
      //pc.printf("I'm here"); 
      //error_reporter->Report(config.output_message[gesture_index]);
     }       
     if(sw2==0 && debounce.read_ms()>5000)
     break;
  }  
}

int ML_acc(){
    int count = 0;

    // Set up logging.
    static tflite::MicroErrorReporter micro_error_reporter;
    tflite::ErrorReporter* error_reporter = &micro_error_reporter;
  
    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    const tflite::Model* model = tflite::GetModel(g_magic_wand_model_data);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
      error_reporter->Report(
          "Model provided is schema version %d not equal "
          "to supported version %d.",
          model->version(), TFLITE_SCHEMA_VERSION);
      //return -1;
    }
    // Pull in only the operation implementations we need.
    // This relies on a complete list of all the ops needed by this graph.
    // An easier approach is to just use the AllOpsResolver, but this will
    // incur some penalty in code space for op implementations that are not
    // needed by this graph.
    static tflite::MicroOpResolver<6> micro_op_resolver;
    micro_op_resolver.AddBuiltin(
        tflite::BuiltinOperator_DEPTHWISE_CONV_2D,
        tflite::ops::micro::Register_DEPTHWISE_CONV_2D());
    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_MAX_POOL_2D,
                                 tflite::ops::micro::Register_MAX_POOL_2D());
    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_CONV_2D,
                                 tflite::ops::micro::Register_CONV_2D());
    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_FULLY_CONNECTED,
                                 tflite::ops::micro::Register_FULLY_CONNECTED());
    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_SOFTMAX,
                                 tflite::ops::micro::Register_SOFTMAX());
    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_RESHAPE,
                                 tflite::ops::micro::Register_RESHAPE(), 1);
   // Build an interpreter to run the model with
   static tflite::MicroInterpreter static_interpreter(
       model, micro_op_resolver, tensor_arena, kTensorArenaSize, error_reporter);
   tflite::MicroInterpreter* interpreter = &static_interpreter;

   // Allocate memory from the tensor_arena for the model's tensors
   interpreter->AllocateTensors();
 
   // Obtain pointer to the model's input tensor
   TfLiteTensor* model_input = interpreter->input(0);
   if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
       (model_input->dims->data[1] != config.seq_length) ||
       (model_input->dims->data[2] != kChannelNumber) ||
       (model_input->type != kTfLiteFloat32)) {
     error_reporter->Report("Bad input tensor parameters in model");
     //return -1;
   }
 
   int input_length = model_input->bytes / sizeof(float);
 
   TfLiteStatus setup_status = SetupAccelerometer(error_reporter);
   if (setup_status != kTfLiteOk) {
     error_reporter->Report("Set up failed\n");
     //return -1;
   }
 
   //error_reporter->Report("Set up successful...\n");
     uLCD.cls();
     uLCD.reset();
     uLCD.locate(1,0);
     uLCD.printf("your score : %d\n",score/15);
     uLCD.locate(1,1);
     char songs[] = "1.sheep\n2.bee\n3.tiger";
     uLCD.printf("%s\n",songs);
     uLCD.locate(1,5);
     uLCD.printf("0.back\n1.forward\n2.change\n3.taiko\n");

   while (true) {
      
     // Attempt to read new data from the accelerometer
     got_data = ReadAccelerometer(error_reporter, model_input->data.f,
                                  input_length, should_clear_buffer);
 
     // If there was no new data,
     // don't try to clear the buffer again and wait until next time
     if (!got_data) {
       should_clear_buffer = false;
       continue;
     }
 
     // Run inference, and report any error
     TfLiteStatus invoke_status = interpreter->Invoke();
     if (invoke_status != kTfLiteOk) {
       error_reporter->Report("Invoke failed on index: %d\n", begin_index);
       continue;
     }
 
     // Analyze the results to obtain a prediction
     gesture_index = PredictGesture(interpreter->output(0)->data.f);
 
     // Clear the buffer next time we read data
     should_clear_buffer = gesture_index < label_num;
 
     // Produce an output
     count++;
     if (gesture_index < label_num && count%15==0) {
       //pc.printf("%d",gesture_index);
        if(gesture_index==1){
          if(mode==0)
            mode = 3;
          else
            mode--;
          //pc.printf("%d",mode);
          uLCD.locate(1,10);
          uLCD.printf("mode: %d",mode);
        }
        if(gesture_index==2){
          if(mode==3)
            mode = 0;
          else
            mode++;
          //pc.printf("%d\r\n",mode);
          uLCD.locate(1,10);
          uLCD.printf("mode: %d",mode);
        }   
      //pc.printf("I'm here"); 
      //error_reporter->Report(config.output_message[gesture_index]);
    }   
    if(sw2==0)
    break;
  }  
}

void ML_acc_taiko(){
  int count = 0;
    static tflite::MicroErrorReporter micro_error_reporter;
    tflite::ErrorReporter* error_reporter = &micro_error_reporter;
  
    // Map the model into a usable data structure. This doesn't involve any
    // copying or parsing, it's a very lightweight operation.
    const tflite::Model* model = tflite::GetModel(g_magic_wand_model_data);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
      error_reporter->Report(
          "Model provided is schema version %d not equal "
          "to supported version %d.",
          model->version(), TFLITE_SCHEMA_VERSION);
      //return -1;
    }
  
    // Pull in only the operation implementations we need.
    // This relies on a complete list of all the ops needed by this graph.
    // An easier approach is to just use the AllOpsResolver, but this will
    // incur some penalty in code space for op implementations that are not
    // needed by this graph.
    static tflite::MicroOpResolver<6> micro_op_resolver;
    micro_op_resolver.AddBuiltin(
        tflite::BuiltinOperator_DEPTHWISE_CONV_2D,
        tflite::ops::micro::Register_DEPTHWISE_CONV_2D());
    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_MAX_POOL_2D,
                                 tflite::ops::micro::Register_MAX_POOL_2D());
    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_CONV_2D,
                                 tflite::ops::micro::Register_CONV_2D());
    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_FULLY_CONNECTED,
                                 tflite::ops::micro::Register_FULLY_CONNECTED());
    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_SOFTMAX,
                                 tflite::ops::micro::Register_SOFTMAX());
    micro_op_resolver.AddBuiltin(tflite::BuiltinOperator_RESHAPE,
                                 tflite::ops::micro::Register_RESHAPE(), 1);
   // Build an interpreter to run the model with
   static tflite::MicroInterpreter static_interpreter(
       model, micro_op_resolver, tensor_arena1, kTensorArenaSize1, error_reporter);
   tflite::MicroInterpreter* interpreter = &static_interpreter;

   // Allocate memory from the tensor_arena for the model's tensors
   interpreter->AllocateTensors();
 
   // Obtain pointer to the model's input tensor
   TfLiteTensor* model_input = interpreter->input(0);
   if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
       (model_input->dims->data[1] != config.seq_length) ||
       (model_input->dims->data[2] != kChannelNumber) ||
       (model_input->type != kTfLiteFloat32)) {
     error_reporter->Report("Bad input tensor parameters in model");
     //return -1;
   }
 
   int input_length = model_input->bytes / sizeof(float);
 
   TfLiteStatus setup_status = SetupAccelerometer(error_reporter);
   if (setup_status != kTfLiteOk) {
     error_reporter->Report("Set up failed\n");
     //return -1;
   }
 
   error_reporter->Report("Set up successful...\n");
 
  while (true) {
      
     // Attempt to read new data from the accelerometer
     got_data1 = ReadAccelerometer(error_reporter, model_input->data.f,
                                  input_length, should_clear_buffer1);
 
     // If there was no new data,
     // don't try to clear the buffer again and wait until next time
     if (!got_data1) {
       should_clear_buffer1 = false;
       continue;
     }
 
     // Run inference, and report any error
     TfLiteStatus invoke_status = interpreter->Invoke();
     if (invoke_status != kTfLiteOk) {
       error_reporter->Report("Invoke failed on index: %d\n", begin_index);
       continue;
     }
 
     // Analyze the results to obtain a prediction
     gesture_index_taiko = PredictGesture(interpreter->output(0)->data.f);
 
     // Clear the buffer next time we read data
     should_clear_buffer1 = gesture_index_taiko < label_num;
 
     
    count++;
    if (gesture_index_taiko < label_num && count>countprevious+13 && sw3==0) {
      pc.printf("%d  ",taiko+1);
      pc.printf("%d\n\r",gesture_index_taiko);
      abstaiko[taiko]=gesture_index_taiko;
      taiko++;
      countprevious = count;
    }

    if(sw2==0 && count>countprevious+8){
      pc.printf("%d  ",taiko+1);
      pc.printf("0\n\r");
      countprevious = count;
      abstaiko[taiko]=0;
      taiko++;
    }
     
    if(taiko>47)
    break;
  }
   
  for(int i=0;i<48;i++){
    if(abstaiko[i]==answer[i]){
      score = score+1;
    }
  }
  countscore++;
  //if(countscore==1){
    //uLCD.locate(1,9);
    //uLCD.printf("your score : %d\n",score);
  //}
  //wait(5.0);
   //pc.printf("your score : %d",score);
}

void loadSignalHandler(void) {
  //pc.printf("%d",nowsong);
  //pc.printf("%d",nowsong);
  
  queue.call(loadSignal);
}

void stopPlayNote(void) {
    
    playNote(songmute[1]);
    ML_acc();
    if(mode==0){
      if(nowsong==0)
        nowsong = 2;
      else
        nowsong--; 
      //pc.printf("%d",nowsong); 
    }
    else if(mode==1){
      if(nowsong==2)
        nowsong = 0;
      else
        nowsong++;    
      //pc.printf("%d",nowsong); 
    }
    else if(mode==3){
      taiko1();
    }
    else{
      debounce.reset();
      ML_acc_song();
      //pc.printf("%d",nowsong);
    }
    uLCD.cls();
    uLCD.reset();
    uLCD.locate(1,1);
    uLCD.text_width(2); 
    uLCD.text_height(2);
    if(nowsong==0)
    uLCD.printf("nowsong:\n%d.sheep",nowsong+1);
    if(nowsong==1)
    uLCD.printf("nowsong:\n%d.bee",nowsong+1);
    if(nowsong==2)
    uLCD.printf("nowsong:\n%d.tiger",nowsong+1);
    //int j = nowsong;
    //pc.printf("%d",j); 
}

void ML_trig(){
  count_taiko++;
  if(count_taiko>15){
    
    ML_acc_taiko();
  }
    
  //green_led = !green_led;
  //ML_acc_taiko();
}

int main() {
  debounce.start();
  green_led = 1;
  uLCD.cls();
  uLCD.reset();
  uLCD.background_color(WHITE);
  uLCD.textbackground_color(WHITE);
  uLCD.color(BLUE);
  uLCD.text_width(1); 
  uLCD.text_height(1);
  
  t.start(callback(&queue, &EventQueue::dispatch_forever));
  thread2.start(callback(&queue2, &EventQueue::dispatch_forever));

  sw3.fall(queue2.event(ML_trig));
  sw2.rise(queue.event(loadSignalHandler));
  //sw3.rise(queue.event(playing));
  sw3.rise(queue.event(stopPlayNote));
}

