color_list = [
    "000000",
    "1D2B53",
    "7E2553",
    "008751",
    "AB5236",
    "5F574F",
    "C2C3C7",
    "FFF1E8",
    "FF004D",
    "FFA300",
    "FFEC27",
    "00E436",
    "29ADFF",
    "83769C",
    "FF77A8",
    "FFCCAA",
]

hex_color_list = [int(color, 16) for color in color_list]

i = 0

for color in hex_color_list:
    r = (color >> 16) & 0xFF
    g = (color >> 8) & 0xFF
    b = color & 0xFF
    color = ((r >> 5) & 0x1F) << 11 | ((g >> 5) & 0x3F) << 5 | ((b >> 5) & 0x1F)
    print("{}:{}\n".format(i, color))
    i=i+1
   
print(hex_color_list) 