import matplotlib.pyplot as plt

# Read the quantized audio samples from the file
quantized_samples = []
with open("samples/quantized_audio_sample.txt", "r") as file:
    for line in file:
        samples = list(map(int, line.strip().split()))
        quantized_samples.append(samples)

# Extract the left, right, mid, and side channel data
left_channel = [sample[0] for sample in quantized_samples]
right_channel = [sample[1] for sample in quantized_samples]
mid_channel = [sample[2] for sample in quantized_samples]
side_channel = [sample[3] for sample in quantized_samples]

# Plot the waveforms
plt.figure(figsize=(12, 8))

plt.subplot(4, 1, 1)
plt.plot(left_channel, label='Left Channel')
plt.title('Left Channel')
plt.xlabel('Sample Index')
plt.ylabel('Amplitude')
plt.legend()

plt.subplot(4, 1, 2)
plt.plot(right_channel, label='Right Channel')
plt.title('Right Channel')
plt.xlabel('Sample Index')
plt.ylabel('Amplitude')
plt.legend()

plt.subplot(4, 1, 3)
plt.plot(mid_channel, label='Mid Channel')
plt.title('Mid Channel')
plt.xlabel('Sample Index')
plt.ylabel('Amplitude')
plt.legend()

plt.subplot(4, 1, 4)
plt.plot(side_channel, label='Side Channel')
plt.title('Side Channel')
plt.xlabel('Sample Index')
plt.ylabel('Amplitude')
plt.legend()

plt.tight_layout()
plt.show()