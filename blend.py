'''
Blend 2 images into one
Used to overlay depth and color frames
'''
from PIL import Image
import sys

backgroundfile = sys.argv[1]
overlayfile = sys.argv[2]

background = Image.open(backgroundfile)
overlay = Image.open(overlayfile)

background = background.convert("RGBA")
overlay = overlay.convert("RGBA")

new_img = Image.blend(background, overlay, 0.5)
new_img.save("new.png","PNG")
