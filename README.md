# Breadboard Videocard
This project was created as part of the Computer Architecture lecture at the University of Basel.
This github repository contains the code used to programm a Nor-Flash (SST39SF040) using an Arduino Nano. 

### TODO: ADD Code and Explanations for Image Conversion

## Specifications
- Image size (200x150) Pixels
- Register Address (3 bits Frame|8 bits y-coordinate|8 bits x-coordinate)
- Register Data (8 bits Color Data) (2 bit empty | 2 bit Red Hexcode | 2 bit Green Hexcode | 2 bit Blue Hexcode)

## Code Workflow:
### Load Image into Flash
- Run Erase Flash Arduino Script
- Run LoadImage Arduino Script (will await data)
- Run load_image.py (will serially feed data to Arduino for processing) -> Adjust Port and Binary File
- (Optional) Run ImageDump to view the Hexdata stored on the Flash (On the specified addresses)

### Load 8-Frame Animation into Flash
- Run Erase Flash Arduino Script
- Run LoadAnimation Arduino Script (will await data)
- Run load_animation.py (will serially feed data to Arduino for processing) -> Adjust Port and Binary File
- (Optional) Run AnimationDump to view the Hexdata stored on the Flash (On the specified addresses)
