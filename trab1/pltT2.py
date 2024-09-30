import matplotlib.pyplot as plt

with open('samples/audio_sample.txt', 'r') as file:
    sample_rate, channel_count, sample_count = map(int, file.readline().split())
    samples = [int(line.strip()) for line in file]

# Calculate the time axis
time_axis = [i / sample_rate for i in range(sample_count)]

plt.figure(figsize=(12, 6))
plt.plot(time_axis, samples)
plt.xlabel('Time (seconds)')
plt.ylabel('Amplitude')
plt.title('Audio Waveform')
plt.show()
