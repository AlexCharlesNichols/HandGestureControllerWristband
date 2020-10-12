/* 
 * Adapted from Magic Wand TFLite demo
 */
#include <Arduino.h>
#include <Arduino_LSM9DS1.h>
#include <ArduinoBLE.h>

#include "accelerometer_handler.h"
#include "gesture_predictor.h"
#include "magic_wand_model_data.h"
#include "output_handler.h"

#include "tensorflow/lite/micro/kernels/micro_ops.h"
#include "tensorflow/lite/micro/micro_error_reporter.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/version.h"

// Globals, used for compatibility with Arduino-style sketches.
namespace {
tflite::ErrorReporter* error_reporter = nullptr;
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* model_input = nullptr;
int input_length;

// Create an area of memory to use for input, output, and intermediate arrays.
// The size of this will depend on the model you're using, and may need to be
// determined by experimentation.
constexpr int kTensorArenaSize = 60 * 1024;
uint8_t tensor_arena[kTensorArenaSize];

// Whether we should clear the buffer next time we fetch data
bool should_clear_buffer = false;
}  // namespace

// BLE Battery Service
BLEService CounterService("180F");

// BLE Battery Level Characteristic
BLEUnsignedCharCharacteristic CounterValueChar("2A19",  // standard 16-bit characteristic UUID
    BLERead | BLENotify); // remote clients will be able to get notifications if this characteristic changes

int oldCounter = 0;  // last battery level reading from analog input
long previousMillis = 0;  // last time the battery level was checked, in ms


void setup()
{
  Serial.begin(9600);    // initialize serial communication
  // Set up logging. Google style is to avoid globals or statics because of
  // lifetime uncertainty, but since this has a trivial destructor it's okay.
  static tflite::MicroErrorReporter micro_error_reporter; // NOLINT
  error_reporter = &micro_error_reporter;

  // Wait until we know the serial port is ready
  while (!Serial) {
  }

 // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting BLE failed!");

    while (1);
  }

  /* Set a local name for the BLE device
     This name will appear in advertising packets
     and can be used by remote devices to identify this BLE device
     The name can be changed but maybe be truncated based on space left in advertisement packet
  */
  BLE.setLocalName("CounterMonitor");
  BLE.setAdvertisedService(CounterService); // add the service UUID
  CounterService.addCharacteristic(CounterValueChar); // add the battery level characteristic
  BLE.addService(CounterService); // Add the battery service
  CounterValueChar.writeValue(oldCounter); // set initial value for this characteristic

  /* Start advertising BLE.  It will start continuously transmitting BLE
     advertising packets and will be visible to remote BLE central devices
     until it receives a new connection */

  // start advertising
  BLE.advertise();

  error_reporter->Report("Bluetooth device active, waiting for connections...");

  delay(500);
  error_reporter->Report("\n\r\n\rMagic Wand - TensorFlow Lite demo");
  
  // Map the model into a usable data structure. This doesn't involve any
  // copying or parsing, it's a very lightweight operation.
  model = tflite::GetModel(g_magic_wand_model_data);
  if (model->version() != TFLITE_SCHEMA_VERSION)
  {
    error_reporter->Report(
        "TFLite Model provided is schema version %d, which not equal "
        "to supported version %d.",
        model->version(), TFLITE_SCHEMA_VERSION);
    return;
  }
  else {
    error_reporter->Report(
        "TFLite Model provided is schema version %d.", model->version());
  }

  // Pull in only the operation implementations we need.
  // This relies on a complete list of all the ops needed by this graph.
  // An easier approach is to just use the AllOpsResolver, but this will
  // incur some penalty in code space for op implementations that are not
  // needed by this graph.
  static tflite::MicroMutableOpResolver micro_mutable_op_resolver;  // NOLINT
  micro_mutable_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_DEPTHWISE_CONV_2D,
      tflite::ops::micro::Register_DEPTHWISE_CONV_2D());
  micro_mutable_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_MAX_POOL_2D,
      tflite::ops::micro::Register_MAX_POOL_2D());
  micro_mutable_op_resolver.AddBuiltin(tflite::BuiltinOperator_CONV_2D,
                                       tflite::ops::micro::Register_CONV_2D());
  micro_mutable_op_resolver.AddBuiltin(
      tflite::BuiltinOperator_FULLY_CONNECTED,
      tflite::ops::micro::Register_FULLY_CONNECTED());
  micro_mutable_op_resolver.AddBuiltin(tflite::BuiltinOperator_SOFTMAX,
                                       tflite::ops::micro::Register_SOFTMAX());


  // Build an interpreter to run the model with
  static tflite::MicroInterpreter static_interpreter(
      model, micro_mutable_op_resolver, tensor_arena, kTensorArenaSize,
      error_reporter);
  interpreter = &static_interpreter;

  // Allocate memory from the tensor_arena for the model's tensors
  interpreter->AllocateTensors();

  // Obtain pointer to the model's input tensor
  model_input = interpreter->input(0);
  //if ((model_input->dims->size != 4) || (model_input->dims->data[0] != 1) ||
    //  (model_input->dims->data[1] != 128) ||
      //(model_input->dims->data[2] != kChannelNumber) ||
      //(model_input->type != kTfLiteFloat32)) {
    //error_reporter->Report("Bad input tensor parameters in model");
    //return;
  //}

  input_length = model_input->bytes / sizeof(float);

  TfLiteStatus setup_status = SetupAccelerometer(error_reporter);
  if (setup_status != kTfLiteOk) {
    error_reporter->Report("Set up failed\n");
  }
  else {
    //error_reporter->Report("Magic starts!\n");
    String magicstr = R"(
___  ___            _            _             _             
|  \/  |           (_)          | |           | |            
| .  . | __ _  __ _ _  ___   ___| |_ __ _ _ __| |_ ___       
| |\/| |/ _` |/ _` | |/ __| / __| __/ _` | '__| __/ __|      
| |  | | (_| | (_| | | (__  \__ \ || (_| | |  | |_\__ \_ _ _ 
\_|  |_/\__,_|\__, |_|\___| |___/\__\__,_|_|   \__|___(_|_|_)
               __/ |                                         
              |___/                                          
    )";
    error_reporter->Report(magicstr.c_str());
    error_reporter->Report("\r\nPredicted gestures:\n\r");
  }
}

void loop()
{
   // wait for a BLE central
  BLEDevice central = BLE.central();
  // if a central is connected to the peripheral:
  if (central) {
    //Serial.print("Connected to central: ");
    // print the central's BT address:
    //Serial.println(central.address());
    // turn on the LED to indicate the connection:
    //digitalWrite(LED_BUILTIN, HIGH);
    int counter = 0;
    // check the battery level every 200ms
    // while the central is connected:
    
    while (central.connected()) {
      long currentMillis = millis();
     // Attempt to read new data from the accelerometer
      bool got_data = ReadAccelerometer(error_reporter, model_input->data.f,
                                    input_length, should_clear_buffer);
  
      if (should_clear_buffer) {
       error_reporter->Report("\r\nPredicted gestures:\n\r");
      }

      // Don't try to clear the buffer again
      should_clear_buffer = false;
      // If there was no new data, wait until next time
      if (!got_data) {
        return;
      }

      // Run inference, and report any error
      TfLiteStatus invoke_status = interpreter->Invoke();
      if (invoke_status != kTfLiteOk) {
        error_reporter->Report("Invoke failed on index: %d\n", begin_index);
        return;
      }
      // Analyze the results to obtain a prediction
      int gesture_index = PredictGesture(interpreter->output(0)->data.f);
      // Clear the buffer next time we read data
      should_clear_buffer = gesture_index < 3;
      // Produce an output
      HandleOutput(error_reporter, gesture_index);
      // if 200ms have passed, check the battery level:
      if (currentMillis - previousMillis >= 200) {
        previousMillis = currentMillis;
        if (gesture_index == 0)
        {
          counter = 1;
        }
        else if (gesture_index == 1)
        {
          counter = 2;
        }
        else if (gesture_index== 2)
        {
          counter = 3;
        }
              // if the battery level has changed
        //Serial.print("Counter valuer is now: "); // print it
        //Serial.println(counter);
        CounterValueChar.writeValue(counter);  // and update the battery level characteristic
        oldCounter = counter;           // save the level for next comparison
        if(counter != 0)
        {
          delay(2500);
        }
      }
    }
    // when the central disconnects, turn off the LED:
    //digitalWrite(LED_BUILTIN, LOW);
    //Serial.print("Disconnected from central: ");
    //Serial.println(central.address());
  }

  
}