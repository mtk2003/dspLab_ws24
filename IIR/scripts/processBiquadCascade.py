import os
import subprocess
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import hilbert

# Set the working directory to the script's location
os.chdir(os.path.dirname(__file__))

# Paths to files and directories
data_folder = '../data'
exe_folder = '../bin'
input_file = os.path.join(data_folder, 'input.csv')
output_file = os.path.join(data_folder, 'output.csv')
iir_coeffs_file = os.path.join(data_folder, 'iir_biquads.csv')
c_executable = os.path.join(exe_folder, 'iir_main.exe')

# Signal parameters
fs = 48e3  
f0 = 5e3   
f1 = 8e3  
bw = f1 - f0  
tc = 1     
fi = 14e3  

# Generate the input signal
t = np.arange(0, tc, 1/fs)
x = np.sin(2 * np.pi * (f0 * t + (bw / (2 * tc)) * t**2)) + np.cos(2 * np.pi * fi * t)

# Compute FFT parameters
nfft = len(x)  
fvec = np.fft.fftshift(np.fft.fftfreq(nfft, d=1/fs))  # Frequency vector

# Scale the input signal
scale_factor = 1e6
x_scaled = x * scale_factor

# Save signal to CSV
np.savetxt(input_file, x_scaled, delimiter=",")

# Plot input signal
plt.figure()
plt.plot(t, x)
plt.title("Input Signal")
plt.xlabel("Time (s)")
plt.ylabel("Amplitude")
plt.grid(which='both', linestyle='--', alpha=0.6)
plt.show()

# Plot original spectrum
original_spectrum = 20 * np.log10(np.abs(np.fft.fftshift(np.fft.fft(x, n=nfft))))
plt.figure()
plt.plot(fvec, original_spectrum)
plt.title("Original Spectrum")
plt.xlabel("Frequency (Hz)")
plt.ylabel("Amplitude (dB)")
plt.grid(which='both', linestyle='--', alpha=0.6)
plt.show()

biquads = pd.read_csv(iir_coeffs_file, header=None)
num_biquads = len(biquads)
print(f"Number of biquads: {num_biquads}")

# Define chunk size
chunk_size = 128

# Command to run the C executable
cmd = [
    os.path.abspath(c_executable),
    os.path.abspath(input_file),
    os.path.abspath(output_file),
    os.path.abspath(iir_coeffs_file),
    str(num_biquads),
    str(chunk_size)
]

# Run the C executable
print("Running C executable...")
try:
    result = subprocess.run(cmd, capture_output=True, text=True, check=True)
    print("C executable finished successfully.")
    print(result.stdout)
except subprocess.CalledProcessError as e:
    print("Error in C executable:")
    print(e.stderr)
except FileNotFoundError as e:
    print(f"Executable not found: {e}")
except Exception as e:
    print(f"Unexpected error occurred: {e}")

# Read the processed signal
if not os.path.isfile(output_file):
    raise FileNotFoundError(f"Output file not found: {os.path.abspath(output_file)}")
y = pd.read_csv(output_file, header=None).values.flatten()

# Scale the output signal back
y_scaled = y / scale_factor

# Plot filtered spectrum
filtered_spectrum = 20 * np.log10(np.abs(np.fft.fftshift(np.fft.fft(y_scaled, n=nfft))))
plt.figure()
plt.plot(fvec, filtered_spectrum)
plt.title("Filtered Spectrum")
plt.xlabel("Frequency (Hz)")
plt.ylabel("Amplitude (dB)")
plt.grid(which='both', linestyle='--', alpha=0.6)
plt.show()

# Plot instantaneous frequency
inst_freq = np.diff(np.unwrap(np.angle(hilbert(y_scaled)))) * (fs / (2 * np.pi))
plt.figure()
plt.plot(t[:-1], inst_freq)
plt.title("Instantaneous Frequency")
plt.xlabel("Time (s)")
plt.ylabel("Frequency (Hz)")
plt.grid(which='both', linestyle='--', alpha=0.6)
plt.show()
