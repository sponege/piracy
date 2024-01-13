from PIL import Image

with Image.open("../old/noparty-old.png") as im:
    pix = im.load()
    for y in range(im.height):
        for x in range(im.width):
            # print(pix[x, y])
            if pix[x, y][3] < 128:
                pix[x, y] = (0, 0, 0, 0)
    im.show()
    im.save("noparty.png")