# Magic wand example

This example shows how you can use TensorFlow Lite to run a 20 kilobyte neural
network model to recognize gestures with an accelerometer. It's designed to run
on systems with very small amounts of memory, such as microcontrollers.

The example application reads data from the accelerometer on an Arduino Nano 33
BLE Sense 

## Table of contents

-   [Getting started](#getting-started)
-   [Deploy to Arduino](#deploy-to-arduino)
-   [Train your own model](#train-your-own-model)

## Deploy to Arduino

The following instructions will help you build and deploy this sample
to [Arduino](https://www.arduino.cc/) devices.

The sample has been tested with the following devices:

- [Arduino Nano 33 BLE Sense](https://store.arduino.cc/usa/nano-33-ble-sense-with-headers)

### Install the Arduino_TensorFlowLite library

This example application is included as part of the official TensorFlow Lite
Arduino library. To install it, open the Arduino library manager in
`Tools -> Manage Libraries...` and search for `Arduino_TensorFlowLite`.


### Load and run the example

Once the library has been added, go to `File -> Examples`. You should see an
example near the bottom of the list named `TensorFlowLite`. Select
it and click `magic_wand` to load the example.

Use the Arduino Desktop IDE to build and upload the example. Once it is running,
you should see the built-in LED on your device flashing.

Open the Arduino Serial Monitor (`Tools -> Serial Monitor`).

You will see the following message:

```
Magic starts！
```

## Train your own model

To train your own model, or create a new model for a new set of gestures,
follow the instructions in [magic_wand/train/README.md](https://github.com/tensorflow/tensorflow/tree/master/tensorflow/lite/micro/examples/magic_wand/train/README.md).