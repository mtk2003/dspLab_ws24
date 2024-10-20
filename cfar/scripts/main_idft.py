import os
import numpy as np
import matplotlib.pyplot as plt
import subprocess

# Constants and Paths
FS = 48e3  # Sampling frequency
SIGNAL_FREQ = 15000  # Signal frequency for cosine wave
CHIRP_FREQ_START = 5000  # Chirp start frequency
CHIRP_FREQ_END = 13000  # Chirp end frequency
DURATION = 10e-3  # Duration of the signal in seconds
INPUT_CSV = 'input.csv'
OUTPUT_CSV = 'output.csv'
DFT_EXECUTABLE = 'main_dft.exe'

# Define the paths relative to the script's directory
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
    return t, np.column_stack((x_real, x_imag))

# Generate the real and imaginary chirp signal
def generate_chirp_signal(fs, f_start, f_end, duration):
    t = np.linspace(0, duration, int(fs * duration), endpoint=False).astype(np.float32) 
    x_real = np.cos(2 * np.pi * (f_start + (f_end - f_start) * t / (2 * duration)) * t).astype(np.float32)   
    x_imag = np.sin(2 * np.pi * (f_start + (f_end - f_start) * t / (2 * duration)) * t).astype(np.float32)   
    return t, np.column_stack((x_real, x_imag))

# Compute FFT
def compute_fft(signal):
    x_real, x_imag = signal[:, 0], signal[:, 1]
    fft_complex = np.fft.fft(x_real + 1j * x_imag)
    return fft_complex

# Save FFT result to CSV
def save_fft_to_csv(fft_complex, filepath):
    fft_real = np.real(fft_complex)
    fft_imag = np.imag(fft_complex)
    fft_result = np.column_stack((fft_real, fft_imag))
    np.savetxt(filepath, fft_result, delimiter=',')

# Call the C program for DFT/IDFT
def call_c_program(executable, input_csv, output_csv, operation, NDFT):
    try:
        subprocess.run([os.path.join(bin_dir, executable), str(NDFT), 
                        os.path.join(data_dir, input_csv), 
                        os.path.join(data_dir, output_csv), operation], check=True)
    except subprocess.CalledProcessError as e:
        print(f"Error occurred while running {executable}: {e}")
        raise

# Load the result from CSV
def load_csv_data(filepath):
    return np.loadtxt(filepath, delimiter=',')

# Calculate errors between original and reconstructed signals
def calculate_errors(original, reconstructed):
    error = original - reconstructed
    mse = np.mean(np.abs(error) ** 2)
    rmse = np.sqrt(mse)
    max_abs_error = np.max(np.abs(error))
    return mse, rmse, max_abs_error

# Plot original and reconstructed signal
def plot_signals(t, original_real, reconstructed_real, original_imag, reconstructed_imag):
    plt.figure(figsize=(12, 8))

    # Plot real parts
    plt.subplot(2, 1, 1)
    plt.plot(t, original_real, label='Original Signal (Real)', color='blue')
    plt.plot(t, reconstructed_real, label='Reconstructed Signal (Real)', color='green', linestyle='--')
    plt.title('Real Part: Original vs Reconstructed')
    plt.xlabel('Time (s)')
    plt.ylabel('Amplitude')
    plt.grid(True)
    plt.legend()

    # Plot imaginary parts
    plt.subplot(2, 1, 2)
    plt.plot(t, original_imag, label='Original Signal (Imaginary)', color='red')
    plt.plot(t, reconstructed_imag, label='Reconstructed Signal (Imaginary)', color='purple', linestyle='--')
    plt.title('Imaginary Part: Original vs Reconstructed')
    plt.xlabel('Time (s)')
    plt.ylabel('Amplitude')
    plt.grid(True)
    plt.legend()

    plt.tight_layout()
    plt.show()

# Main function
def main(signal_type='cosine'):
    # Create necessary directories
    create_directory(data_dir)

    # Step 1: Generate the signal (cosine or chirp)
    if signal_type == 'sine':
        t, signal = generate_sine_signal(FS, SIGNAL_FREQ, DURATION)
    elif signal_type == 'chirp':
        t, signal = generate_chirp_signal(FS, CHIRP_FREQ_START, CHIRP_FREQ_END, DURATION)
    else:
        raise ValueError("Invalid signal type. Choose 'cosine' or 'chirp'.")

    # Step 2: Compute FFT and save to CSV
    fft_complex = compute_fft(signal)
    save_fft_to_csv(fft_complex, os.path.join(data_dir, INPUT_CSV))

    # Step 3: Call C program to compute IDFT
    NDFT = len(fft_complex)
    call_c_program(DFT_EXECUTABLE, INPUT_CSV, OUTPUT_CSV, 'idft', NDFT)

    # Step 4: Load the IDFT result and compute errors
    idft_result = load_csv_data(os.path.join(data_dir, OUTPUT_CSV))
    idft_result_complex = idft_result[:,0] + 1j*idft_result[:,1]
    mse, rmse, max_abs_error = calculate_errors(signal[:,0] + 1j*signal[:,1], idft_result_complex)

    # Print error metrics
    print(f"Mean Squared Error (MSE): {mse}")
    print(f"Root Mean Squared Error (RMSE): {rmse}")
    print(f"Maximum Absolute Error: {max_abs_error}")

    # plot
    plot_signals(t, signal[:, 0], idft_result[:,0], signal[:, 1], idft_result[:,1])

if __name__ == "__main__":
    main(signal_type='sine') # You can change to 'sine' or 'chirp'
