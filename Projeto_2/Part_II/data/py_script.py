import cv2
import numpy as np
import matplotlib.pyplot as plt

# Path to the histogram image
hist_image_path = 'histogram_data.png'

# Read the histogram image
hist_image = cv2.imread(hist_image_path, cv2.IMREAD_GRAYSCALE)

# Check if the image was loaded successfully
if hist_image is None:
    raise FileNotFoundError(f"Failed to load image from {hist_image_path}")

# Extract histogram data from the image
# Assuming the histogram image has a white background and black bars
hist_height, hist_width = hist_image.shape
hist_data = np.zeros(256, dtype=int)

for x in range(hist_width):
    for y in range(hist_height):
        if hist_image[y, x] == 0:  # Black pixel
            bin_index = int(x * 256 / hist_width)
            hist_data[bin_index] += 1

# Normalize the histogram data
hist_data = hist_data / hist_data.sum()

# Plot the histogram using Matplotlib
plt.figure(figsize=(10, 6))
plt.bar(range(256), hist_data, color='blue', edgecolor='black')
plt.xlabel('Pixel Intensity Value')
plt.ylabel('Normalized Frequency')
plt.title('Frequency Distribution of Grayscale Image')
plt.grid(True)
plt.show()