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
DFT_EXECUTABLE = 'main_dft.exe'

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
    signal = np.column_stack((x_real, x_imag))

    if noise_level is not None:
        signal = add_noise(signal, noise_level)

    return signal

# Generate the real and imaginary chirp signal
def generate_chirp_signal(fs, f_start, f_end, duration, noise_level=None):
    t = np.linspace(0, duration, int(fs * duration), endpoint=False).astype(np.float32) 
    x_real = np.cos(2 * np.pi * (f_start + (f_end - f_start) * t / (2 * duration)) * t).astype(np.float32)   
    x_imag = np.sin(2 * np.pi * (f_start + (f_end - f_start) * t / (2 * duration)) * t).astype(np.float32)   
    signal = np.column_stack((x_real, x_imag))

    if noise_level is not None:
        signal = add_noise(signal, noise_level)

    return signal

# Save the signal to a CSV file
def save_signal_to_csv(signal, filepath):
    np.savetxt(filepath, signal, delimiter=',')

# Call the C executable for DFT, IDFT, or CFAR
def call_c_program(executable, input_csv, output_csv, operation, NDFT, cfar_params=None):
    executable_path = os.path.join(bin_dir, executable)
    input_path = os.path.join(data_dir, input_csv)
    output_path = os.path.join(data_dir, output_csv)

    command = [executable_path, str(NDFT), input_path, output_path, operation]

    # Add CFAR parameters if provided
    if cfar_params:
        command.extend(map(str, cfar_params))

    try:
        subprocess.run(command, check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error occurred while running {executable}: {e}")
        raise

# Load the result from CSV
def load_csv_data(filepath):
    return np.loadtxt(filepath, delimiter=',')

# Plot the CFAR result: squared DFT magnitude and CFAR threshold
def plot_cfar(squared_magnitude, cfar_threshold, frequencies):
    plt.figure(figsize=(10, 6))

    # Plot squared DFT magnitude
    plt.plot(frequencies, squared_magnitude, label='Squared DFT Magnitude', color='blue')

    # Plot CFAR threshold
    plt.plot(frequencies, cfar_threshold, label='CFAR Threshold', color='red', linestyle='--')

    plt.title('CFAR Detection')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Amplitude')
    plt.grid(True)
    plt.legend()
    plt.show()

# Main function
def main(signal_type='sine', noise_level=None, cfar_params=None):
    # Create necessary directories
    create_directory(data_dir)

    # Generate the selected signal type (sine or chirp) with optional noise
    if signal_type == 'sine':
        signal = generate_sine_signal(FS, SIGNAL_FREQ, SIGNAL_DURATION, noise_level)
    elif signal_type == 'chirp':
        signal = generate_chirp_signal(FS, CHIRP_FREQ_START, CHIRP_FREQ_END, SIGNAL_DURATION, noise_level)
    else:
        raise ValueError("Invalid signal type. Choose 'sine' or 'chirp'.")

    # Save the signal to CSV
    save_signal_to_csv(signal, os.path.join(data_dir, INPUT_CSV))

    # Number of samples
    NDFT = signal.shape[0]

    # Call the C program to compute CFAR
    call_c_program(DFT_EXECUTABLE, INPUT_CSV, OUTPUT_CSV, 'cfar', NDFT, cfar_params)

    # Load the CFAR result from the CSV
    cfar_result = load_csv_data(os.path.join(data_dir, OUTPUT_CSV))

    # Separate the squared magnitude and CFAR threshold
    squared_magnitude = cfar_result[:, 0]
    cfar_threshold = cfar_result[:, 1]

    # Generate frequency bins for plotting
    frequencies = np.fft.fftshift(np.fft.fftfreq(NDFT, 1 / FS))

    # Plot the CFAR result
    plot_cfar(squared_magnitude, cfar_threshold, frequencies)

if __name__ == "__main__":
    Pfa = 1e-3 # howto calculate the Pfa?
    T = -np.log(Pfa)
    print(T)
    main(signal_type='sine', noise_level=0.9, cfar_params=[T, 20, 5])
