import numpy as np
import matplotlib.pyplot as plt
import soundfile as sf
import subprocess
import os

# File paths
base_dir = os.path.join(os.path.dirname(__file__), '../data')
origWAV = os.path.join(base_dir, 'orig.wav')
inputWAV = os.path.join(base_dir, 'input.wav')
outputWAV = os.path.join(base_dir, 'output.wav')
exe_path = os.path.join(os.path.dirname(__file__), '../bin/LMS_main.exe')

# LMS parameters
numFIR = 32      # Number of FIR coefficients
nChunk = 128     # Buffer size
mu = 0.008        # Learning rate
fi = 5000        # Carrier frequency of the interference (Hz)
fm = 1           # Modulation frequency (Hz)

# Read the original audio
x, fs = sf.read(origWAV)

# Handle mono or stereo audio
if x.ndim > 1:
    x = x[:, 0]  # Use only the first channel

# Add amplitude-modulated interference to the signal
n = np.arange(len(x))
t = n / fs  # Time vector
modulation = 0.5 * (1 + np.sin(2 * np.pi * fm * t))  # Modulation envelope
random_phase = np.random.uniform(0, 2 * np.pi)
print("Random phase for interference:", random_phase)
interference = modulation * np.cos(2 * np.pi * fi * t + random_phase)  # AM interference
x_noisy = x + interference
x_noisy = x_noisy / np.max(np.abs(x_noisy))  # Normalize to prevent clipping

# Save the noisy audio with interference
sf.write(inputWAV, x_noisy.astype(np.float32), fs)

# Build the command to execute the C program for LMS
cmd = f'"{exe_path}" "{inputWAV}" "{outputWAV}" {numFIR} {nChunk} {mu} {fi}'
print("Running command:", cmd)

# Execute the LMS filter in C
status = subprocess.run(cmd, shell=True, capture_output=True, text=True)

# Check if the command executed successfully
if status.returncode != 0:
    print("Error:", status.stderr)
    exit(1)
else:
    print("LMS filter applied successfully.")

# Read the filtered audio signal from the LMS process
y_lms, _ = sf.read(outputWAV)

# Perform direct subtraction using the known interference as comparison
direct_subtracted = x_noisy - interference

nfft = len(x_noisy)
fvec = np.fft.fftfreq(nfft, d=1/fs)
fvec = np.fft.fftshift(fvec)

# FFT of noisy input
X_f = np.fft.fftshift(np.fft.fft(x_noisy, n=nfft))
X_f_magnitude = np.abs(X_f)
X_f_magnitude[X_f_magnitude == 0] = 1e-10

# FFT of LMS output
Y_lms_f = np.fft.fftshift(np.fft.fft(y_lms, n=nfft))
Y_lms_magnitude = np.abs(Y_lms_f)
Y_lms_magnitude[Y_lms_magnitude == 0] = 1e-10

# FFT of direct subtraction output
Y_direct_f = np.fft.fftshift(np.fft.fft(direct_subtracted, n=nfft))
Y_direct_magnitude = np.abs(Y_direct_f)
Y_direct_magnitude[Y_direct_magnitude == 0] = 1e-10

plt.figure(figsize=(12, 8))

# Input signal spectrum
plt.subplot(3, 1, 1)
plt.plot(fvec, 20 * np.log10(X_f_magnitude))
plt.grid(True, which='both', linestyle='--', linewidth=0.5)
plt.xlabel('Frequency (Hz)')
plt.ylabel('Amplitude (dB)')
plt.title('Input Signal with AM Interference')

# LMS output spectrum
plt.subplot(3, 1, 2)
plt.plot(fvec, 20 * np.log10(Y_lms_magnitude))
plt.grid(True, which='both', linestyle='--', linewidth=0.5)
plt.xlabel('Frequency (Hz)')
plt.ylabel('Amplitude (dB)')
plt.title('LMS Filtered Output Signal')

# Direct subtraction output spectrum
plt.subplot(3, 1, 3)
plt.plot(fvec, 20 * np.log10(Y_direct_magnitude))
plt.grid(True, which='both', linestyle='--', linewidth=0.5)
plt.xlabel('Frequency (Hz)')
plt.ylabel('Amplitude (dB)')
plt.title('Direct Subtraction Output Signal')

plt.tight_layout()
plt.show()
