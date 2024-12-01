import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import subprocess
import os
import warnings

base_dir = os.path.join(os.path.dirname(__file__), '../data')
inputFile = os.path.join(base_dir, 'input.csv')
outputFile = os.path.join(base_dir, 'output.csv')
firCoeffsFile = os.path.join(base_dir, 'firCoeffsNew.csv')
exe_path = os.path.join(os.path.dirname(__file__), '../bin/ds_main.exe')

# Number of FIR coefficients and processing chunk
firCoeffs = pd.read_csv(firCoeffsFile, header=None)
numFIR = firCoeffs.shape[0]
nSamplesPerChunk = 128       # Number of samples to process per ISR
downSamplingFactor = 4       # Downsampling factor
fmix = 7e3                   # Mixing frequency
fs = 48e3
fsds = fs/downSamplingFactor # Down-sampled frequency

# Signal parameters
fstart = 8e3    # Desired siganl start frequency
fstop = 9e3     # Desired signal stop frequency
tc = 1          # Chirp duration
fi = 18e3       # Interference signal
fmixNorm = fmix/fs # Normalized mixing frequency

t = np.linspace(0, tc, int(tc * fs))
xd = np.cos(2 * np.pi * t*(fstart + (fstop - fstart) / (2 * tc) * t)) # Desired signal
xi = 0.05*np.cos(2 * np.pi * fi * t) # Interference signal
x = xd + xi # Signal to store in csv
#pd.DataFrame(x).to_csv(inputFile, header=False, index=False) # Write input signal to csv
np.savetxt(inputFile, x, delimiter=",")

# Call the C executable
cmd = f'"{exe_path}" "{inputFile}" "{outputFile}" "{firCoeffsFile}" {numFIR} {nSamplesPerChunk} {downSamplingFactor} {fmixNorm}'
status = subprocess.run(cmd, shell=True, capture_output=True, text=True)

# Check if the command executed successfully
if status.returncode != 0:
    print("Error:", status.stderr)
else:
    print("Signal processed successfully.")

# Read processed signal
y = pd.read_csv(outputFile, header=None).values.flatten()
yfft = np.fft.fft(y)

# FFT parameters fft plots
nfftx = len(x)
nffty = len(y)
fftFreqsx = np.fft.fftfreq(nfftx, d= 1/fs) # FFT frequencies of x for plotting
fftFreqsy = np.fft.fftfreq(nffty, d = 1/fsds) # FFT frequencies of y for plotting
xfft = np.fft.fft(x) # FFT of input signal
yfft = np.fft.fft(y) # FFT of output signal

# Plots
fig, axs = plt.subplots(2,1 , figsize=(6,8))

# Spectrum of input signal
axs[0].plot(np.fft.fftshift(fftFreqsx), 20*np.log10(np.abs(np.fft.fftshift(xfft)) +1e-12), label= 'Input signal')
axs[0].set_xlabel('Frequency in [Hz]')
axs[0].set_ylabel('Amplitude in [dB]')
axs[0].set_title('Spectrum of input signal')
axs[0].grid()
axs[0].legend()

# Spectrum of processed signal
axs[1].plot(np.fft.fftshift(fftFreqsy), 20*np.log10(np.abs(np.fft.fftshift(yfft)) +1e-12), label= 'Output signal')
axs[1].set_xlabel('Frequency in [Hz]')
axs[1].set_ylabel('Amplitude in [dB]')
axs[1].set_title('Spectrum of output signal')
axs[1].grid()
axs[1].legend()

plt.tight_layout()
plt.show()

# Debug print
print("Length of input signal: " , len(x))
print("Length of output signal: " , len(y))








