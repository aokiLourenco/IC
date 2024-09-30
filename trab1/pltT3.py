import matplotlib.pyplot as plt

def read_histogram(filename):
    histogram = {}
    with open(filename, 'r') as file:
        for line in file:
            bin_value, count = map(int, line.split())
            histogram[bin_value] = count
    return histogram

def plot_histogram(histogram, title):
    bins = list(histogram.keys())
    counts = list(histogram.values())

    plt.figure(figsize=(10, 6))
    plt.bar(bins, counts, width=256, color='blue', edgecolor='black')
    plt.xlabel('Amplitude Bins')
    plt.ylabel('Frequency')
    plt.title(title)
    plt.tight_layout()
    plt.show()

# Read histograms
left_channel_histogram = read_histogram('samples/left_channel_histogram.txt')
right_channel_histogram = read_histogram('samples/right_channel_histogram.txt')
mid_channel_histogram = read_histogram('samples/mid_channel_histogram.txt')
side_channel_histogram = read_histogram('samples/side_channel_histogram.txt')

while True:
    # Menu for selecting which histogram to display
    print("\nSelect which histogram to display:")
    print("1. Left Channel Histogram")
    print("2. Right Channel Histogram")
    print("3. MID Channel Histogram")
    print("4. SIDE Channel Histogram")
    print("5. Exit")
    choice = input("Enter your choice (1-5): ")

    if choice == '1':
        plot_histogram(left_channel_histogram, 'Left Channel Histogram')
    elif choice == '2':
        plot_histogram(right_channel_histogram, 'Right Channel Histogram')
    elif choice == '3':
        plot_histogram(mid_channel_histogram, 'MID Channel Histogram')
    elif choice == '4':
        plot_histogram(side_channel_histogram, 'SIDE Channel Histogram')
    elif choice == '5':
        print("Exiting the program.")
        break
    else:
        print("Invalid choice. Please enter a number between 1 and 5.")