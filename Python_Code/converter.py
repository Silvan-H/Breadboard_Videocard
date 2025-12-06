from PIL import Image

# Mapping from original byte to 2-bit code
MAP = {
    0x00: 0b00,
    0x55: 0b01,
    0xaa: 0b10,
    0xff: 0b11
}

image = Image.open("Frog_Anim.png").convert("RGB")
pixels = image.load()

with open("Frog_Anim.bin", "wb") as out_file:
    for i in range(8):
        for y in range(150):
            for x in range(200):
                r, g, b = pixels[x,(y + (150 * i))]

                # Quantize each color channel
                r2 = MAP.get(r, 0)
                g2 = MAP.get(g, 0)
                b2 = MAP.get(b, 0)

                # Pack into one byte: RRR GGG BBB → 00rrggbb
                packed = (r2 << 4) | (g2 << 2) | b2

                out_file.write(bytes([packed]))

