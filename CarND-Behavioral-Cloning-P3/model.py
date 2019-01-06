import csv

def get_samples_from_file(csv_name):
    raw_samples = []
    with open(csv_name) as csv_file:
        csv_reader = csv.reader(csv_file)
        for line in csv_reader:
            raw_samples.append(line)
    return raw_samples[1:]


import numpy as np
import random
from skimage.io import imread
from keras.utils import Sequence

class AugmentedSequence(Sequence):
    def __init__(self, xs, ys, zs, batch_size):
        self.x, self.y, self.z = xs, ys, zs
        self.batch_size = batch_size
    def __len__(self):
        return int(len(self.x) / float(self.batch_size))
    def __getitem__(self, idx):
        x_batch = self.x[idx * self.batch_size : (idx + 1) * self.batch_size]
        y_batch = self.y[idx * self.batch_size : (idx + 1) * self.batch_size]
        z_batch = self.z[idx * self.batch_size : (idx + 1) * self.batch_size]
        i_batch = np.array([imread(x_item) if z_item else np.fliplr(imread(x_item))
                            for x_item, z_item in zip(x_batch, z_batch)])
        return i_batch, y_batch
    def on_epoch_end(self):
        combined = list(zip(self.x, self.y, self.z))
        random.shuffle(combined)
        self.x[:], self.y[:], self.z[:] = zip(*combined)
    
def sequencer(samples, batch_size=32):
    x_set = []
    y_set = []
    z_set = []
    for sample in samples:
        x_set.append("/opt/carnd_p3/data/IMG/" + sample[0].split('/')[-1])
        y_set.append(1.3 * float(sample[3]))
        z_set.append(True)
        x_set.append("/opt/carnd_p3/data/IMG/" + sample[0].split('/')[-1])
        y_set.append(-1.3 * float(sample[3]))
        z_set.append(False)
    return AugmentedSequence(x_set, y_set, z_set, batch_size)


from sklearn.model_selection import train_test_split
from keras.models import Sequential
from keras.layers import Cropping2D, Lambda, Conv2D, Dense, Flatten, Dropout

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

batch_size = 32
samples = get_samples_from_file("/opt/carnd_p3/data/driving_log.csv")
random.shuffle(samples)
train_samples, validation_samples = train_test_split(samples, test_size = 0.2)
train_sequence = sequencer(train_samples, batch_size)
validation_sequence = sequencer(validation_samples, batch_size)

model.compile(loss="mse", optimizer="adam")
model.fit_generator(train_sequence,
                    steps_per_epoch=len(train_sequence),
                    epochs=20,
                    verbose=1,
                    validation_data=validation_sequence,
                    validation_steps=len(validation_sequence),
model.save("model.h5")
