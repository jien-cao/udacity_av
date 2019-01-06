# **Behavioral Cloning** 

## TL;DR
### (1) model is the nvidia model mentioned in class; (2) drive data are combined from "easy track" (lake) and "hard track" (mountain); (3) trained model is shown to work well for both tracks.

---

**Behavioral Cloning Project**

The goals / steps of this project are the following:
* Use the simulator to collect data of good driving behavior
* Build, a convolution neural network in Keras that predicts steering angles from images
* Train and validate the model with a training and validation set
* Test that the model successfully drives around track one without leaving the road
* Summarize the results with a written report

## Rubric Points
### Here I will consider the [rubric points](https://review.udacity.com/#!/rubrics/432/view) individually and describe how I addressed each point in my implementation.  

---
### Files Submitted & Code Quality

#### 1. Submission includes all required files and can be used to run the simulator in autonomous mode

My project includes the following files:
* model.py containing the script to create and train the model
* drive.py for driving the car in autonomous mode
* model.h5 containing a trained convolution neural network 
* writeup_report.md or writeup_report.pdf summarizing the results
* video_easy_track.mp4 is autonomous driving on lake track
* video_hard_track.mp4 is autonomous driving on mountain track

#### 2. Submission includes functional code
Using the Udacity provided simulator and my drive.py file, the car can be driven autonomously around the track by executing 
```sh
python drive.py model.h5
```

#### 3. Submission code is usable and readable

The model.py file contains the code for training and saving the convolution neural network. The file shows the pipeline I used for training and validating the model, and it contains comments to explain how the code works.

### Model Architecture and Training Strategy

#### 1. An appropriate model architecture has been employed

My model is based on the nvidia model described in class (model.py lines 56-68).

Model consists of 5 convolutional layers with RELU activations, and 3 fully connected layers with drop out. 

Data images are cropped to remove top and bottom (unnecessary info), then normalized in the model using a lambda layer. 

#### 2. Attempts to reduce overfitting in the model

The model contains dropout layers in order to reduce overfitting, they are added between fully connected layers (model.py lines 62, 64, 66). 

The model was trained and validated on different data sets to ensure that the model was not overfitting (code line 73). The model was tested by running it through the simulator and ensuring that the vehicle could stay on the track.

#### 3. Model parameter tuning

The model used an adam optimizer, so the learning rate was not tuned manually (model.py line 77).

#### 4. Appropriate training data

Training data was chosen to keep the vehicle driving on the road. I used a combination of "easy track" (lake) and "hard track" (mountain).

Images are shuffled before splitting into training and validation sets.

For details about how I created the training data, see the next section. 

### Model Architecture and Training Strategy

#### 1. Solution Design Approach

The overall strategy for deriving a model architecture was to best clone driving behaviors to enable autonomous driving.

My first step was to use a convolution neural network model similar to the nvidia network described in class. I thought this model might be appropriate because the deep stack of 5 convolutional and 3 fully connected layers could extract useful features from camera images and correlate them to driving parameters.

In order to gauge how well the model was working, I split my image and steering angle data into a training and validation set. I found that my first model had a low mean squared error on the training set but a high mean squared error on the validation set. This implied that the model was overfitting. 

To combat the overfitting, I added drop-out layers between fully connected layers.  It's confirmed to be effective as the training and validation loss values are tracking better now.

I also tried various number of epochs and found the model performance to stablize at around epoch 8 - 9.  The model I saved and used is based on 10 epochs.

The final step was to run the simulator to see how well the car was driving around track one. There were a few spots where the vehicle fell off the track. My interpretation is that this suggested more aggressive steering was needed compared to the manual training session in Simulator.

My solution is to apply a multiplier of 1.3x on the steering values collected from the training session, i.e. the values in the coressponding column in the driving_log.csv file.

At the end of the process, the vehicle is able to drive autonomously around the track without leaving the road.

#### 2. Final Model Architecture

The final model architecture (model.py lines 56-68) consisted of a convolution neural network with the following layers and layer sizes:

```python
model = Sequential()
model.add(Cropping2D(((50, 20), (0, 0)), input_shape = (160, 320, 3)))
model.add(Lambda(lambda x : x / 255.0 - 0.5))
model.add(Conv2D(24, kernel_size=(5, 5), strides=(2, 2), activation="relu"))
model.add(Conv2D(36, kernel_size=(5, 5), strides=(2, 2), activation="relu"))
model.add(Conv2D(48, kernel_size=(5, 5), strides=(2, 2), activation="relu"))
model.add(Conv2D(64, kernel_size=(3, 3), strides=(1, 1), activation="relu"))
model.add(Conv2D(64, kernel_size=(3, 3), strides=(1, 1), activation="relu"))
model.add(Flatten())
model.add(Dropout(0.2))
model.add(Dense(100))
model.add(Dropout(0.2))
model.add(Dense(50))
model.add(Dropout(0.2))
model.add(Dense(10))
model.add(Dense(1))
```

#### 3. Creation of the Training Set & Training Process

I used a combination of the "easy track" (lake) and "hard track" (mountain) driving data to train and validate model.  For "easy track" (lake), I used the data included in class since it's relatively easy.  For "hard track" (mountain), I ran the simulator locally and uploaded the resulted data to the workspace.

In the end, I used a driving_log.csv containing all entries from the above two tracks and an IMG folder with all images from both tracks.

To train and validate for both tracks, I used a shuffle and then train_test_split to ensure training and validation both get data from both tracks.
```python
batch_size = 32
samples = get_samples_from_file("/opt/carnd_p3/data/driving_log.csv")
random.shuffle(samples)
train_samples, validation_samples = train_test_split(samples, test_size = 0.2)
train_sequence = sequencer(train_samples, batch_size)
validation_sequence = sequencer(validation_samples, batch_size)
```

I used a keras ```Sequence``` instead of generic generator, initially hoping to use ```fit_generator```'s ```shuffle=True``` API for a clean implementation of shuffing data.  However from actual testing, the GPU enabled workspace doesn't support this keras API.  I still kept the Sequence type and added a ```on_epoch_end``` to ensure shuffling happens between epochs.