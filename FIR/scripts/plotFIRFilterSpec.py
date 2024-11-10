import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import firwin
import csv
import os

# Filter design parameters
fs = 48e3  
cutoff_freq = 3e3  
filter_order = 128 
nfft = 1024  
firCoeffsFile = os.path.join(os.path.dirname(__file__), '../data/firCoeffsNew.csv')  

def create_lowpass_fir(cutoff, fs, numtaps, filename):
    os.makedirs(os.path.dirname(filename), exist_ok=True)

    fir_coeffs = firwin(numtaps, cutoff / (fs / 2), window='hamming')  # Normalized cutoff

    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)
        for coeff in fir_coeffs:
            writer.writerow([coeff])

    return fir_coeffs

fir_coeffs = create_lowpass_fir(cutoff_freq, fs, filter_order, firCoeffsFile)
print(f"Lowpass FIR filter coefficients saved to '{firCoeffsFile}'")

freqs = np.fft.fftfreq(nfft, d=1/fs)
freqs = np.fft.fftshift(freqs)  
response = np.fft.fft(fir_coeffs, n=nfft)
response = np.fft.fftshift(response)  
response_magnitude = np.abs(response)
response_magnitude[response_magnitude == 0] = 1e-10  # Replace zeros with a small number
magnitude_db = 20 * np.log10(response_magnitude)


plt.figure(figsize=(12, 8))
plt.subplot(2, 1, 1)
plt.stem(range(len(fir_coeffs)), fir_coeffs, basefmt=" ")
plt.xlabel('Sample Index')
plt.ylabel('Amplitude')
plt.title('FIR Filter Coefficients (Impulse Response)')
plt.grid(True)

plt.subplot(2, 1, 2)
plt.plot(freqs, magnitude_db)
plt.xlabel('Frequency (Hz)')
plt.ylabel('Magnitude (dB)')
plt.title(f'Frequency Response of FIR Lowpass Filter (Cutoff: {cutoff_freq} Hz)')
plt.grid(True, which='both', linestyle='--', linewidth=0.5)
plt.xlim(-fs/2, fs/2)  

plt.tight_layout()
plt.show()
