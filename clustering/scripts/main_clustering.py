import os
import numpy as np
import matplotlib.pyplot as plt
import subprocess

# Define constants
FS = 48e3  # Sampling frequency
SIGNAL_FREQ = 15000  # Frequency for sine/cosine signal
CHIRP_FREQ_START = 2500  # Start frequency for chirp
CHIRP_FREQ_END = 16000   # End frequency for chirp
SIGNAL_DURATION = 100e-3  # Duration of the signal in seconds
INPUT_CSV = 'input.csv'
OUTPUT_CSV = 'output.csv'
C_PROGRAM_EXECUTABLE = 'main_dft.exe'  # C program executable

# Define the paths
base_dir = os.path.dirname(os.path.abspath(__file__))  # Get the path of the current script
data_dir = os.path.join(base_dir, '../data/')  # Data folder is at the same level as scripts
bin_dir = os.path.join(base_dir, '../bin/')    # Bin folder is also at the same level

# Ensure data directory exists
def create_directory(directory):
    if not os.path.exists(directory):
        os.makedirs(directory)

# Function to add Gaussian noise to the signal
def add_noise(signal, noise_level):
    noise_real = np.random.normal(0, noise_level, signal.shape[0])  # Noise for real part
    noise_imag = np.random.normal(0, noise_level, signal.shape[0])  # Noise for imaginary part
    signal[:, 0] += noise_real  # Add noise to real part
    signal[:, 1] += noise_imag  # Add noise to imaginary part
    return signal

# Generate the real and imaginary sine/cosine signal
def generate_sine_signal(fs, signal_freq, duration, noise_level=None):
    t = np.linspace(0, duration, int(fs * duration), endpoint=False).astype(np.float32)  
    x_real = np.cos(2 * np.pi * signal_freq * t).astype(np.float32)    
    x_imag = np.zeros_like(x_real)  # Set imaginary part to 0
    #x_imag = np.sin(2 * np.pi * signal_freq * t).astype(np.float32)
    signal = np.column_stack((x_real, x_imag))

    if noise_level is not None:
        signal = add_noise(signal, noise_level)

    return signal

# Generate the real and imaginary chirp signal
def generate_chirp_signal(fs, f_start, f_end, duration, noise_level=None):
    t = np.linspace(0, duration, int(fs * duration), endpoint=False).astype(np.float32) 
    x_real = np.cos(2 * np.pi * (f_start + (f_end - f_start) * t / (2 * duration)) * t).astype(np.float32)   
    x_imag = np.sin(2 * np.pi * (f_start + (f_end - f_start) * t / (2 * duration)) * t).astype(np.float32)   
    x_imag = np.zeros_like(x_real)
    signal = np.column_stack((x_real, x_imag))

    if noise_level is not None:
        signal = add_noise(signal, noise_level)

    return signal

# Save the signal to a CSV file
def save_signal_to_csv(signal, filepath):
    np.savetxt(filepath, signal, delimiter=',')

# Call the C executable for the specified operation (e.g., DFT, K-means)
def call_c_program(executable, input_csv, output_csv, operation, NDFT, num_clusters=None, max_iter=None):
    executable_path = os.path.join(bin_dir, executable)
    input_path = os.path.join(data_dir, input_csv)
    output_path = os.path.join(data_dir, output_csv)

    # Command for the specified operation
    command = [executable_path, str(NDFT), input_path, output_path, operation]
    if operation == 'kmeans':
        # Add additional parameters for K-means
        command.extend([str(num_clusters), str(max_iter)])

    try:
        subprocess.run(command, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error occurred while running {executable}: {e}")
        raise

# Load data from CSV
def load_csv_data(filepath):
    return np.loadtxt(filepath, delimiter=',')

# Plot the squared magnitudes from DFT and FFT, colorized by cluster label
def plot_clusters(frequencies, result_data, fft_magnitude):
    plt.figure()
    
    # Plot the DFT squared magnitude from result_data
    plt.plot(frequencies, (np.fft.fftshift(result_data[:, 0])), label='Squared Magnitude (C DFT)', color='blue')
    
    # Scatter plot for each cluster label
    labels = np.fft.fftshift(result_data[:, 1])
    scatter = plt.scatter(frequencies, (np.fft.fftshift(result_data[:, 0])), c=labels, cmap='viridis', alpha=0.7)
    
    # Adding colorbar to represent different clusters
    plt.colorbar(scatter, label="Cluster Label")
    
    # Add plot details
    plt.title('DFT Squared Magnitude with Cluster Labels')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Magnitude (squared linear)')
    plt.grid(True)
    plt.legend()
    plt.show()


# Main function for generating signal, running K-means, and plotting
def main(signal_type='sine', operation='kmeans',noise_level=None, num_clusters=3, max_iter=100):
    # Create necessary directories
    create_directory(data_dir)

    # Generate the selected signal type (sine or chirp) with optional noise
    if signal_type == 'sine':
        signal = generate_sine_signal(FS, SIGNAL_FREQ, SIGNAL_DURATION, noise_level)
    elif signal_type == 'chirp':
        signal = generate_chirp_signal(FS, CHIRP_FREQ_START, CHIRP_FREQ_END, SIGNAL_DURATION, noise_level)
    else:
        raise ValueError("Invalid signal type. Choose 'sine' or 'chirp'.")

    # Save the generated signal to input.csv
    save_signal_to_csv(signal, os.path.join(data_dir, INPUT_CSV))

    # Number of samples
    NDFT = signal.shape[0]

    # Call the C program for the specified operation (kmeans in this case)
    call_c_program(C_PROGRAM_EXECUTABLE, INPUT_CSV, OUTPUT_CSV, operation, NDFT, num_clusters, max_iter)

    # Load the clustering result from output.csv
    result_data = load_csv_data(os.path.join(data_dir, OUTPUT_CSV))

    # Compute the frequency axis
    frequencies = np.fft.fftshift(np.fft.fftfreq(NDFT, 1 / FS))

    # Compute the FFT of the original signal and get its squared magnitude
    fft_complex = np.fft.fft(signal[:, 0] + 1j * signal[:, 1])  # Use both real and imaginary parts
    fft_magnitude = np.abs(np.fft.fftshift(fft_complex)) ** 2

    # Plot the clustered data and compare it with the FFT magnitude
    plot_clusters(frequencies, result_data, fft_magnitude)

if __name__ == "__main__":
    main(signal_type='chirp', operation='kmeans',noise_level=0.0, num_clusters=2, max_iter=10)

# Debug
#ul, counts = np.unique(result_data[:,1], return_counts=True)
