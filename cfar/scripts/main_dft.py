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

# Generate the real and imaginary sine/cosine signal
def generate_sine_signal(fs, signal_freq, duration):
    t = np.linspace(0, duration, int(fs * duration), endpoint=False).astype(np.float32)  
    x_real = np.cos(2 * np.pi * signal_freq * t).astype(np.float32)    
    x_imag = np.sin(2 * np.pi * signal_freq * t).astype(np.float32)  
    x_imag = np.zeros_like(x_real)  
    return np.column_stack((x_real, x_imag))

# Generate the real and imaginary chirp signal
def generate_chirp_signal(fs, f_start, f_end, duration):
    t = np.linspace(0, duration, int(fs * duration), endpoint=False).astype(np.float32) 
    x_real = np.cos(2 * np.pi * (f_start + (f_end - f_start) * t / (2 * duration)) * t).astype(np.float32)   
    x_imag = np.sin(2 * np.pi * (f_start + (f_end - f_start) * t / (2 * duration)) * t).astype(np.float32)   
    return np.column_stack((x_real, x_imag))

# Save the signal to a CSV file
def save_signal_to_csv(signal, filepath):
    np.savetxt(filepath, signal, delimiter=',')

# Call the C executable for DFT or IDFT
def call_c_program(executable, input_csv, output_csv, operation, NDFT):
    executable_path = os.path.join(bin_dir, executable)
    input_path = os.path.join(data_dir, input_csv)
    output_path = os.path.join(data_dir, output_csv)
    
    try:
        subprocess.run([executable_path, str(NDFT), input_path, output_path, operation], check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error occurred while running {executable}: {e}")
        raise

# Load the result from CSV
def load_csv_data(filepath):
    return np.loadtxt(filepath, delimiter=',')

# Plot the DFT magnitude
def plot_dft_magnitude(frequencies, magnitude, fft_magnitude):
    plt.figure()
    plt.plot(frequencies, 20*np.log10(magnitude), label='C DFT Magnitude')
    plt.plot(frequencies, 20*np.log10(fft_magnitude), label='FFT Magnitude')
    plt.title('Magnitude of DFT')
    plt.xlabel('Frequency (Hz)')
    plt.ylabel('Magnitude')
    plt.grid(True)
    plt.legend()
    plt.show()

# Main function
def main(signal_type='sine'):
    # Create necessary directories
    create_directory(data_dir)

    # Generate the selected signal type (sine or chirp) using the defined duration
    if signal_type == 'sine':
        signal = generate_sine_signal(FS, SIGNAL_FREQ, SIGNAL_DURATION)
    elif signal_type == 'chirp':
        signal = generate_chirp_signal(FS, CHIRP_FREQ_START, CHIRP_FREQ_END, SIGNAL_DURATION)
    else:
        raise ValueError("Invalid signal type. Choose 'sine' or 'chirp'.")

    # Save the signal to CSV
    save_signal_to_csv(signal, os.path.join(data_dir, INPUT_CSV))

    # Number of samples
    NDFT = signal.shape[0]

    # Call the C program to compute the DFT
    call_c_program(DFT_EXECUTABLE, INPUT_CSV, OUTPUT_CSV, 'dft', NDFT)

    # Load the DFT result from the CSV
    dft_result = load_csv_data(os.path.join(data_dir, OUTPUT_CSV))

    # Separate real and imaginary parts and combine into complex numbers
    real_part = dft_result[:, 0]
    imag_part = dft_result[:, 1]
    dft_complex = real_part + 1j * imag_part

    # Compute magnitude and frequencies
    dft_shifted = np.fft.fftshift(dft_complex)
    magnitude = np.abs(dft_shifted)
    fft_complex = np.fft.fft(signal[:, 0] + 1j * signal[:, 1])
    fft_magnitude = np.fft.fftshift(np.abs(fft_complex))
    frequencies = np.fft.fftshift(np.fft.fftfreq(NDFT, 1 / FS))

    # Plot the magnitude
    plot_dft_magnitude(frequencies, magnitude, fft_magnitude)

if __name__ == "__main__":
    main(signal_type='chirp')  # You can change to 'sine' or 'chirp'
