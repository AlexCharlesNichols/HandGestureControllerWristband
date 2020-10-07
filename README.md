# Hand Gesture Controller Wristband Training Scripts

## Introduction

The scripts in this directory can be used to train a TensorFlow model that
classifies gestures based on accelerometer data. The code uses Python 3.7 and
TensorFlow 2.0. The resulting model is less than 20KB in size.

The following document contains instructions on using the scripts to train a
model, and capturing your own training data.

This project was inspired by the [TensorFlow Magic Wand example](https://https://github.com/tensorflow/tensorflow/tree/master/tensorflow/lite/micro/examples/magic_wand/train)
project.

## Training

### Dataset

The dataset can be found inside the data folder

### Training in Colab

The following [Google Colaboratory](https://colab.research.google.com)
notebook demonstrates how to train the model. It's the easiest way to get
started:

<table class="tfo-notebook-buttons" align="left">
  <td>
    <a target="_blank" href="https://colab.research.google.com/github/tensorflow/tensorflow/blob/master/tensorflow/lite/micro/examples/magic_wand/train/train_magic_wand_model.ipynb"><img src="https://www.tensorflow.org/images/colab_logo_32px.png" />Run in Google Colab</a>
  </td>
  <td>
    <a target="_blank" href="https://github.com/tensorflow/tensorflow/blob/master/tensorflow/lite/micro/examples/magic_wand/train/train_magic_wand_model.ipynb"><img src="https://www.tensorflow.org/images/GitHub-Mark-32px.png" />View source on GitHub</a>
  </td>
</table>

If you'd prefer to run the scripts locally, use the following instructions.

### Running the scripts

Use the following command to install the required dependencies:

```shell
pip install -r requirements.txt
```

There are two ways to train the model:

- Random data split, which mixes different people's data together and randomly
  splits them into training, validation, and test sets
- Person data split, which splits the data by person

#### Random data split

Using a random split results in higher training accuracy than a person split,
but inferior performance on new data.

```shell
$ python data_prepare.py

$ python data_split.py

$ python train.py --model CNN --person false
```

#### Person data split

Using a person data split results in lower training accuracy but better
performance on new data.

```shell
$ python data_prepare.py

$ python data_split_person.py

$ python train.py --model CNN --person true
```

#### Model type

In the `--model` argument, you can can provide `CNN` or `LSTM`. The CNN
model has a smaller size and lower latency.


