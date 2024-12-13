import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
import os
import subprocess

# File paths
base_dir = os.path.join(os.path.dirname(__file__), '../data')
input_file = os.path.join(base_dir, 'input.csv')
output_file = os.path.join(base_dir, 'output.csv')
exe_path = os.path.join(os.path.dirname(__file__), '../bin/IF_main.exe')

# Ensure the directory for data files exists
os.makedirs(base_dir, exist_ok=True)

# Chunk size
chunk_size = 256

# Signal definition
select_signal = 'chirp'

if select_signal == 'chirp':
    fs = 48e3
    tc = 1
    f0 = 5e3
    f1 = 11e3
    bw = f1 - f0
    t = np.arange(0, tc, 1 / fs)
    x = np.exp(1j * 2 * np.pi * (f0 * t + (bw / (2 * tc)) * t**2))

elif select_signal == 'barker':
    fs = 48e3
    tc = 1
    t = np.arange(0, tc, 1 / fs)
    f0 = 7e3

    # Barker 7 code 1,1,1,-1,-1,1,-1
    order = np.pi  # PSK pi QPSK pi/2
    jump_seq = order * np.array([0, 0, 0, 1, 1, 2, 3])

    chip_dur = len(t) / len(jump_seq)
    phi = np.zeros(len(t))

    for j in range(len(jump_seq)):
        start_idx = int((j - 1) * chip_dur)
        end_idx = min(int(j * chip_dur), len(t))
        phi[start_idx:end_idx] = jump_seq[j]

    x = np.exp(1j * phi) * np.exp(1j * 2 * np.pi * f0 * t)

x += 0.001 * (np.random.randn(len(x)) + 1j * np.random.randn(len(x)))
win = np.ones(len(x))
x = x * win

# Calculate original spectrum
nFFT = len(x)
sig_spec = np.fft.fft(x, n=nFFT)
f_vec = np.fft.fftshift(np.fft.fftfreq(nFFT, d=1/fs))

plt.figure()
plt.plot(f_vec, 20 * np.log10(np.abs(np.fft.fftshift(sig_spec))))
plt.xlabel('Frequency in [Hz]')
plt.ylabel('Amplitude in [dB]')
plt.grid(which='minor')
plt.title('Input Signal Spectrum')
plt.show()

# Write data to CSV
input_data = np.column_stack((np.real(x), np.imag(x)))
pd.DataFrame(input_data).to_csv(input_file, header=False, index=False)

# Call the C executable
cmd = f'{exe_path} {input_file} {output_file} {chunk_size} {float(fs)}'
status = subprocess.run(cmd, shell=True, capture_output=True, text=True)

if status.returncode != 0:
    print(status.stderr)

# Read processed signal
processed_signal = pd.read_csv(output_file, header=None).to_numpy()

# Plot numpy vs C version
fig, axs = plt.subplots(2, 1, figsize=(10, 8))

axs[0].plot(t[:-1], np.diff(np.unwrap(np.angle(x))) * fs / (2 * np.pi))
#axs[0].plot(t[:-1], np.angle(x)[:-1])
axs[0].set_xlabel('Time in [sec]')
axs[0].set_ylabel('Instantaneous Frequency in [Hz]')
axs[0].set_title('Original Instantaneous Frequency')
axs[0].grid(which='minor')

axs[1].plot(t[:-1], processed_signal)
axs[1].set_xlabel('Time in [sec]')
axs[1].set_ylabel('Instantaneous Frequency in [Hz]')
axs[1].set_title('Block Processed Instantaneous Frequency')
axs[1].grid(which='minor')

plt.tight_layout()
plt.show()
