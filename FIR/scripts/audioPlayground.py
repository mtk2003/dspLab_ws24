import numpy as np
import matplotlib.pyplot as plt
import soundfile as sf
import subprocess
import os
import warnings

base_dir = os.path.join(os.path.dirname(__file__), '../data')
origWAV = os.path.join(base_dir, 'orig.wav')
inputWAV = os.path.join(base_dir, 'input.wav')
outputWAV = os.path.join(base_dir, 'output.wav')
firCoeffsFile = os.path.join(base_dir, 'firCoeffsNew.csv')
exe_path = os.path.join(os.path.dirname(__file__), '../bin/FIR_main.exe')

# Number of FIR coefficients and processing chunk
numFIR = 244
nBuffer = 128

# Original audio
x, fs = sf.read(origWAV)

# Handle mono or stereo audio
if x.ndim > 1:
    x = x[:, 0]  

# Interference signal
fi = 10e3  
n = np.arange(len(x))
x = x + np.cos(2 * np.pi * fi * n / fs)  
x = x / np.max(np.abs(x))  

# Save audio signal with interference
sf.write(inputWAV, x.astype(np.float32), fs)

# Call the C executable
cmd = f'"{exe_path}" "{inputWAV}" "{outputWAV}" "{firCoeffsFile}" {numFIR} {nBuffer}'
status = subprocess.run(cmd, shell=True, capture_output=True, text=True)

# Check if the command executed successfully
if status.returncode != 0:
    print("Error:", status.stderr)
else:
    print("FIR filter applied successfully.")

# Read filtered audio signal
y, fs = sf.read(outputWAV)

# Perform FFT and handle zero values to prevent log10 issues
nfft = len(x)
fvec = np.fft.fftfreq(nfft, d=1/fs)
fvec = np.fft.fftshift(fvec)
X_f = np.fft.fftshift(np.fft.fft(x, n=nfft))
Y_f = np.fft.fftshift(np.fft.fft(y, n=nfft))
X_f_magnitude = np.abs(X_f)
X_f_magnitude[X_f_magnitude == 0] = 1e-10
Y_f_magnitude = np.abs(Y_f)
Y_f_magnitude[Y_f_magnitude == 0] = 1e-10

# Plotting
plt.figure(figsize=(10, 6))
plt.subplot(2, 1, 1)
plt.plot(fvec, 20 * np.log10(X_f_magnitude))
plt.grid(True, which='both', linestyle='--', linewidth=0.5)
plt.xlabel('Frequency (Hz)')
plt.ylabel('Amplitude (dB)')
plt.title('Input Signal with Interference')

plt.subplot(2, 1, 2)
plt.plot(fvec, 20 * np.log10(Y_f_magnitude))
plt.grid(True, which='both', linestyle='--', linewidth=0.5)
plt.xlabel('Frequency (Hz)')
plt.ylabel('Amplitude (dB)')
plt.title('Filtered Output Signal')

plt.tight_layout()
plt.show()
