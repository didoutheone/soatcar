import time
tA = time.clock()

images_directory = 'imgs'
model_file_name_to_use = '20180808-sc1-dsi'
video_file_to_generate = 'salient-movie.mp4'

import donkeycar
import donkeycar.parts.keras as dkk

#1
from keras.layers import Input, Dense, merge
from keras.models import Model
from keras.layers import Convolution2D, MaxPooling2D, Reshape, BatchNormalization
from keras.layers import Activation, Dropout, Flatten, Dense


#2

def default_categorical():
    img_in = Input(shape=(120, 160, 3), name='img_in')                      # First layer, input layer, Shape comes from camera.py resolution, RGB
    x = img_in
    x = Convolution2D(24, (5,5), strides=(2,2), activation='relu', name = 'conv1')(x)       # 24 features, 5 pixel x 5 pixel kernel (convolution, feauture) window, 2wx2h stride, relu activation
    x = Convolution2D(32, (5,5), strides=(2,2), activation='relu', name = 'conv2')(x)       # 32 features, 5px5p kernel window, 2wx2h stride, relu activatiion
    x = Convolution2D(64, (5,5), strides=(2,2), activation='relu', name = 'conv3')(x)       # 64 features, 5px5p kernal window, 2wx2h stride, relu
    x = Convolution2D(64, (3,3), strides=(2,2), activation='relu', name = 'conv4')(x)       # 64 features, 3px3p kernal window, 2wx2h stride, relu
    x = Convolution2D(64, (3,3), strides=(1,1), activation='relu', name = 'conv5')(x)       # 64 features, 3px3p kernal window, 1wx1h stride, relu

    # Possibly add MaxPooling (will make it less sensitive to position in image).  Camera angle fixed, so may not to be needed

    x = Flatten(name='flattened')(x)                                        # Flatten to 1D (Fully connected)
    x = Dense(100, activation='relu', name = 'dense1')(x)                                    # Classify the data into 100 features, make all negatives 0
    x = Dropout(.1)(x)                                                      # Randomly drop out (turn off) 10% of the neurons (Prevent overfitting)
    x = Dense(50, activation='relu', name = 'dense2')(x)                                     # Classify the data into 50 features, make all negatives 0
    x = Dropout(.1)(x)                                                      # Randomly drop out 10% of the neurons (Prevent overfitting)
    #categorical output of the angle
    angle_out = Dense(15, activation='softmax', name='angle_out')(x)        # Connect every input with every output and output 15 hidden units. Use Softmax to give percentage. 15 categories and find best one based off percentage 0.0-1.0
    
    #continous output of throttle
    throttle_out = Dense(1, activation='relu', name='throttle_out')(x)      # Reduce to 1 number, Positive number only
    
    model = Model(inputs=[img_in], outputs=[angle_out, throttle_out])
  
    return model

#3    
model = default_categorical()          # pour utiliser le model intégré au-dessus
#model = dkk.default_linear()            # pour utiliser le model 'linear' de donkeycar
model.load_weights(model_file_name_to_use)

#4
img_in = Input(shape=(120, 160, 3), name='img_in')
x = img_in
x = Convolution2D(24, (5,5), strides=(2,2), activation='relu', name='conv1')(x)
x = Convolution2D(32, (5,5), strides=(2,2), activation='relu', name='conv2')(x)
x = Convolution2D(64, (5,5), strides=(2,2), activation='relu', name='conv3')(x)
x = Convolution2D(64, (3,3), strides=(2,2), activation='relu', name='conv4')(x)
conv_5 = Convolution2D(64, (3,3), strides=(1,1), activation='relu', name='conv5')(x)
convolution_part = Model(inputs=[img_in], outputs=[conv_5])

#5
for layer_num in ('1', '2', '3', '4', '5'):
    convolution_part.get_layer('conv' + layer_num).set_weights(model.get_layer('conv' + layer_num).get_weights())

#6
from keras import backend as K

inp = convolution_part.input                                           # input placeholder
outputs = [layer.output for layer in convolution_part.layers]          # all layer outputs
functor = K.function([inp], outputs)

#7
import tensorflow as tf
tf.reset_default_graph()

#8
import numpy as np

#9
kernel_3x3 = tf.constant(np.array([
        [[[1]], [[1]], [[1]]], 
        [[[1]], [[1]], [[1]]], 
        [[[1]], [[1]], [[1]]]
]), tf.float32)

kernel_5x5 = tf.constant(np.array([
        [[[1]], [[1]], [[1]], [[1]], [[1]]], 
        [[[1]], [[1]], [[1]], [[1]], [[1]]], 
        [[[1]], [[1]], [[1]], [[1]], [[1]]],
        [[[1]], [[1]], [[1]], [[1]], [[1]]],
        [[[1]], [[1]], [[1]], [[1]], [[1]]]
]), tf.float32)

layers_kernels = {5: kernel_3x3, 4: kernel_3x3, 3: kernel_5x5, 2: kernel_5x5, 1: kernel_5x5}

layers_strides = {5: [1, 1, 1, 1], 4: [1, 2, 2, 1], 3: [1, 2, 2, 1], 2: [1, 2, 2, 1], 1: [1, 2, 2, 1]}

def compute_visualisation_mask(img):
    
    activations = functor([np.array([img])])
    upscaled_activation = np.ones((3, 6))
    
    for layer in [5, 4, 3, 2, 1]:
        averaged_activation = np.mean(activations[layer], axis=3).squeeze(axis=0) * upscaled_activation
        output_shape = (activations[layer - 1].shape[1], activations[layer - 1].shape[2])
        x = tf.constant(
            np.reshape(averaged_activation, (1,averaged_activation.shape[0],averaged_activation.shape[1],1)),
            tf.float32
        )
        conv = tf.nn.conv2d_transpose(
            x, layers_kernels[layer],
            output_shape=(1,output_shape[0],output_shape[1], 1), 
            strides=layers_strides[layer], 
            padding='VALID'
        )
        #result = session.run(conv)
        result = session.run(conv, feed_dict={})
        
        upscaled_activation = np.reshape(result, output_shape)
    
    final_visualisation_mask = upscaled_activation
    
    return (final_visualisation_mask - np.min(final_visualisation_mask))/(np.max(final_visualisation_mask) - np.min(final_visualisation_mask))

#10
import cv2
#import numpy as np #deja fait plus haut

#11
from matplotlib import pyplot as plt
#%matplotlib inline

#12
#import matplotlib.pyplot as plt #deja fait plus haut
from matplotlib import animation
#from IPython.display import display, HTML

plt.rcParams['animation.ffmpeg_path'] = 'ffmpeg\\bin\\ffmpeg.exe'

def plot_movie_mp4(image_array):
    dpi = 72.0
    xpixels, ypixels = image_array[0].shape[0], image_array[0].shape[1]
    fig = plt.figure(figsize=(ypixels/dpi, xpixels/dpi), dpi=dpi)
    im = plt.figimage(image_array[0])

    def animate(i):
        im.set_array(image_array[i])
        return (im,)

    anim = animation.FuncAnimation(fig, animate, frames=len(image_array))
    #display(HTML(anim.to_html5_video()))
    
    # Set up formatting for the movie files
    dict(artist='Me')
    fps = 20
    bitrate = 500
    Writer = animation.writers['ffmpeg']
    writer = Writer(fps=fps, metadata=dict(artist='Me'), bitrate=bitrate)
    anim.save(filename=video_file_to_generate, writer=writer)
    # https://matplotlib.org/api/_as_gen/matplotlib.animation.Animation.html#matplotlib.animation.Animation.save
    # https://matplotlib.org/examples/animation/basic_example_writer.html
 
    
#13
from glob import iglob

#20
imgs = []
alpha = 0.004
alpha = 0.6
beta = 1.0 - alpha
beta = 1.0
counter = 0

session = tf.Session()

for path in iglob(images_directory+'/*.jpg'):
#while True:
    # Pour eviter le tri des images qui fait scintiller la video
    img = cv2.imread(path)
    t0 = time.clock()
    salient_mask = compute_visualisation_mask(img)
    t1 = time.clock()
    black_image_mono = np.zeros((120,160,1), np.uint8)
    salient_mask_stacked = np.dstack((salient_mask,black_image_mono))
    salient_mask_stacked = np.dstack((salient_mask_stacked,black_image_mono))
    
    # Conversion du salient_mask (généré en Float32, valeurs de 0.0 à 1.0) en UInt8 (de 0 à 255) afin d'être mixé avec l'image de la camera qui est en UInt8.
    salient_mask_stacked = 255 * salient_mask_stacked # Scale by 255
    salient_mask_stacked = salient_mask_stacked.astype(np.uint8)
    
    blend = cv2.addWeighted(img, alpha, salient_mask_stacked.astype('uint8'), beta, 0.0)
    imgs.append(blend)
    
    counter += 1
    i1 = round( (t1 - t0)*1000 , 0)
    print('Etape {0} \t calculée en {1}ms \t {2}'.format(counter, i1, path))  

    # TODO : Optimisation !!!
    # Pour l'optimisation, cette page est interessante : https://stackoverflow.com/questions/47528401/tensorflow-why-my-code-is-running-slower-and-slower
    
    if counter >= 100:   # limite le nombre d'images à 400. Trop long sinon...
        break

session.close()

#21
print("Go film")
plot_movie_mp4(imgs)

import math
tB = time.clock()
iA = math.floor( (tB - tA)/3600 )
iB = math.floor( (tB - tA)/60 ) - (60*iA)
iC = math.floor( (tB - tA) ) - (60*iB) - (3600*iA)
print('Réalisé en {0}h {1}m {2}s'.format(iA, iB, iC))

#########################

