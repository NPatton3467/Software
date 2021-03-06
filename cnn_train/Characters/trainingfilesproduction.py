import os
import cv2
import math
import numpy as np
import time

'''
data, targets = numpy.load(<filename>)

npz structure:
[data, targets]

data is a matrix with dimensions [N, D]
--values are 0 or 1
N = number of images
D = 1600

targets is also a matrix with dimensions [N, 1]
--each value is charToIndex[character]*4+<offset>
'''

def rotate_image(image, angle):
    diagonal = int(math.sqrt(pow(image.shape[0], 2) + pow(image.shape[1], 2)))
    offset_x = (diagonal - image.shape[0])/2
    offset_y = (diagonal - image.shape[1])/2
    dst_image = np.zeros((diagonal, diagonal, 3), dtype='uint8')
    image_center = (diagonal/2, diagonal/2)

    R = cv2.getRotationMatrix2D(image_center, angle, 1.0)
    dst_image[int(offset_x):int(offset_x + image.shape[0]),\
            int(offset_y):int(offset_y + image.shape[1]),\
            :] = image
    dst_image = cv2.warpAffine(dst_image, R, (diagonal, diagonal), flags=cv2.INTER_LINEAR)

    # Calculate the rotated bounding rect
    x0 = offset_x
    x1 = offset_x + image.shape[0]
    x2 = offset_x
    x3 = offset_x + image.shape[0]

    y0 = offset_y
    y1 = offset_y
    y2 = offset_y + image.shape[1]
    y3 = offset_y + image.shape[1]

    corners = np.zeros((3,4))
    corners[0,0] = x0
    corners[0,1] = x1
    corners[0,2] = x2
    corners[0,3] = x3
    corners[1,0] = y0
    corners[1,1] = y1
    corners[1,2] = y2
    corners[1,3] = y3
    corners[2:] = 1
    
    c = np.dot(R, corners)
    
    x = int(c[0,0])
    y = int(c[1,0])
    left = x
    right = x
    up = y
    down = y

    for i in range(4):
        x = int(c[0,i])
        y = int(c[1,i])
        if (x < left): left = x
        if (x > right): right = x
        if (y < up): up = y
        if (y > down): down = y
    h = down - up
    w = right - left

    cropped = np.zeros((w, h, 3), dtype='uint8')
    cropped[:, :, :] = dst_image[left:(left+w), up:(up+h), :]
    return cropped
    
def rotateImg(angle, image):
    rotated = rotate_image(image, angle)
    rotated = cv2.resize(rotated, (40, 40), interpolation = cv2.INTER_CUBIC)

    return rotated

# Script parameters
directory = "D:/Workspace/UAV/Software/cnn_train/Characters/FontDatabase/English/Fnt"
# outputDirectory
outputDirectory = "D:/Workspace/UAV/Software/cnn_train/Characters/ProcessedFonts"

# Stores the existing classes
D = {}

# Numbers
D["001"] = "0"
D["002"] = "1"
D["003"] = "2"
D["004"] = "3"
D["005"] = "4"
D["006"] = "5"
D["007"] = "6"
D["008"] = "7"
D["009"] = "8"
D["010"] = "9"

# Capital Case
D["011"] = "A"
D["012"] = "B"
D["013"] = "C"
D["014"] = "D"
D["015"] = "E"
D["016"] = "F"
D["017"] = "G"
D["018"] = "H"
D["019"] = "I"
D["020"] = "J"
D["021"] = "K"
D["022"] = "L"
D["023"] = "M"
D["024"] = "N"
D["025"] = "O"
D["026"] = "P"
D["027"] = "Q"
D["028"] = "R"
D["029"] = "S"
D["030"] = "T"
D["031"] = "U"
D["032"] = "V"
D["033"] = "W"
D["034"] = "X"
D["035"] = "Y"
D["036"] = "Z"

# Inverted mapping
charToIndex = {v : (int(k)-1) for k, v in D.items()} # iteritems() in python 2.x

print(charToIndex)

imagePaths = []
for x in os.walk(directory):
    # Last 3 characters of the folder name corresponds to the number
    folderName = x[0].split('/')[-1]
    ID = folderName[-3:]
    if ID in D:
        paths = [x[0] + "/" + name for name in x[2]]
        imagePaths.extend(paths)
    
numRotations = 36
    
#Data array
numPts = len(imagePaths) * numRotations
data = np.zeros((numPts, 1600), dtype=np.uint8)
#Target Array
target = np.zeros((numPts, 1))

rotationInterval = 10
i = 0
currID = -1

for imagePath in imagePaths:
    # Get image name
    name = imagePath.split("/")[-1]
    ID = name[3:6]
    character = D[ID]
    
    if (currID != ID):
        currID = ID
        print("starting on class " + str(charToIndex[character]) +\
                " (character " + str(character) + ")")
    
    # Read and resize immediately
    image = cv2.imread(imagePath)
    image = cv2.resize(image, (40, 40))

    # Invert colours
    image = 255 - image

    for rotIdx in range(numRotations):
        angle = rotIdx * (360 / numRotations)
        rotatedImg = rotateImg(angle, image)

        # Convert to grayscale
        rotatedImg = cv2.cvtColor(rotatedImg, cv2.COLOR_RGB2GRAY)  

        # Threshold it into a binary image
        ret, rotatedImg = cv2.threshold(rotatedImg, 128, 1, cv2.THRESH_BINARY)
        
        # Reshape it into a vector
        rotatedImg = np.reshape(rotatedImg, (1, 1600))

        data[i,:] = rotatedImg
        target[i] = charToIndex[character]
        
        # Verification
        #print("Verifying " + str(i) + " target = " + str(target[i]))
        #cv2.imshow("Verification", np.reshape(data[i,:], (40, 40)) * 255)
        #cv2.waitKey(0)
        
        i = i + 1

print("there are " + str(np.max(target)+1) + " classes")
print(str(len(imagePaths)) + " original images")

np.save("Data",data)
np.save("Target",target)