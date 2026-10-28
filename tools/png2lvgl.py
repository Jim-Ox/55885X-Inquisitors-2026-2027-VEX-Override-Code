"""
turns a png/jpg into an lvgl 9 image you can show on the brain

usage: python tools/png2lvgl.py picture.png my_image [--size 110] [--key-white]
makes src/images/my_image.c, then in c++ use LV_IMAGE_DECLARE(my_image) and &my_image
needs pillow: pip install pillow
"""
import argparse
import os
from PIL import Image, ImageDraw


def key_background(img, tolerance):
    # flood fill from the corners so white parts inside the robot stay solid
    img = img.convert("RGBA")
    w, h = img.size
    for corner in [(0, 0), (w - 1, 0), (0, h - 1), (w - 1, h - 1)]:
        r, g, b, a = img.getpixel(corner)
        if a and min(r, g, b) >= 255 - tolerance:
            ImageDraw.floodfill(img, corner, (0, 0, 0, 0), thresh=tolerance)
    return img


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("input")
    parser.add_argument("name")
    parser.add_argument("--size", type=int, default=110, help="max width/height in pixels")
    parser.add_argument("--key-white", action="store_true", help="make the white background transparent")
    args = parser.parse_args()

    img = Image.open(args.input).convert("RGBA")
    if args.key_white:
        img = key_background(img, 12)
    bbox = img.getbbox()
    if bbox:
        img = img.crop(bbox)
    img.thumbnail((args.size, args.size), Image.LANCZOS)
    w, h = img.size

    # lvgl ARGB8888 is stored as b, g, r, a bytes
    rgba = img.tobytes()
    data = bytearray(len(rgba))
    data[0::4] = rgba[2::4]
    data[1::4] = rgba[1::4]
    data[2::4] = rgba[0::4]
    data[3::4] = rgba[3::4]

    rows = []
    for i in range(0, len(data), 24):
        rows.append("    " + ", ".join(f"0x{v:02x}" for v in data[i:i + 24]) + ",")

    out_dir = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "src", "images")
    os.makedirs(out_dir, exist_ok=True)
    path = os.path.join(out_dir, f"{args.name}.c")
    with open(path, "w", newline="\n") as f:
        f.write('#include "liblvgl/lvgl.h"\n\n')
        f.write(f"static const uint8_t {args.name}_map[] = {{\n")
        f.write("\n".join(rows))
        f.write("\n};\n\n")
        f.write(f"const lv_image_dsc_t {args.name} = {{\n")
        f.write("    .header = {\n")
        f.write("        .magic = LV_IMAGE_HEADER_MAGIC,\n")
        f.write("        .cf = LV_COLOR_FORMAT_ARGB8888,\n")
        f.write("        .flags = 0,\n")
        f.write(f"        .w = {w},\n")
        f.write(f"        .h = {h},\n")
        f.write(f"        .stride = {w * 4},\n")
        f.write("    },\n")
        f.write(f"    .data_size = sizeof({args.name}_map),\n")
        f.write(f"    .data = {args.name}_map,\n")
        f.write("};\n")
    print(f"wrote {os.path.normpath(path)} ({w}x{h})")


if __name__ == "__main__":
    main()
