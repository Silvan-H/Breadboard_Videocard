import serial
import time
import sys

# Configuration
PORT = 'COM3'
BAUD = 115200
IMAGE_FILE = 'bbg_seezl.bin'
CHUNK_SIZE = 64  # Send 10 bytes at a time (matches Arduino ACK rate)

print("Opening serial port...")
ser = serial.Serial(PORT, BAUD, timeout=10)
time.sleep(3)  # Wait for Arduino to reset

print("Reading image file...")
with open(IMAGE_FILE, 'rb') as f:
    data = f.read(30000)

if len(data) != 30000:
    print(f"ERROR: File is {len(data)} bytes, expected 30000!")
    sys.exit(1)

print(f"Loaded {len(data)} bytes")

# Wait for Arduino ready message
print("Waiting for Arduino ready...")
while True:
    if ser.in_waiting:
        line = ser.readline().decode('utf-8', errors='ignore')
        print(line.strip())
        if 'READY' in line:
            break

print("Sending data...")
bytes_sent = 0

for i in range(0, len(data), CHUNK_SIZE):
    chunk = data[i:i+CHUNK_SIZE]
    ser.write(chunk)
    bytes_sent += len(chunk)
    
    # Wait for ACK
    ack = ser.read(1)
    if ack != b'K':
        print(f"Warning: Expected ACK, got {ack}")
    
    if bytes_sent % 1024 == 0:
        print(f"Sent {bytes_sent}/30000 bytes")

print("Data sent! Monitoring progress...")

# Monitor remaining output
while True:
    if ser.in_waiting:
        line = ser.readline().decode('utf-8', errors='ignore')
        print(line.strip())
        if 'DONE' in line:
            break

ser.close()
print("\nComplete!")