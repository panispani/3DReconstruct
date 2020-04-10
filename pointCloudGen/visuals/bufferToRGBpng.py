'''
Read color image from buffer format and save as 8-bit RBG png
Input file contents:
height depth
height*depth numbers

Usage example:
python bufferToRGBpng.py color0001.txt color0001.png
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
    arr = np.zeros((h, w, 3), dtype=np.uint8)
    idx = 2
    for c in range(3):
        for i in range(h):
            for j in range(w):
                arr[i][j][c] = x[idx]
                idx += 1
    im = Image.fromarray(arr)
    im.save(outputfile)
