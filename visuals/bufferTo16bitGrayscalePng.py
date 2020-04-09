'''
Read depth image from buffer format and save as 16-bit grayscale png
Input file contents:
height depth
height*depth numbers

Usage example:
python bufferTo16bitGrayscalePng.py depth0001.txt depth0001.png
'''
import numpy as np
from PIL import Image
import sys

filename = sys.argv[1]
outputfile = sys.argv[2]

with open(filename,'r') as f:
    x = list(map(int, f.read().split()))
    h = x[0]
    w = x[1]
    arr = np.zeros((h, w), dtype=np.uint16)
    idx = 2
    for i in range(h):
        for j in range(w):
            arr[i][j] = x[idx]
            idx += 1
    im = Image.fromarray(arr)
    im.save(outputfile)
