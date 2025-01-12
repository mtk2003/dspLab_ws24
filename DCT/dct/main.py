import numpy as np
import matplotlib.pyplot as plt

# DCT implementation using a nested loop
def custom_dct(signal):
    n_samples = len(signal)
    dct_coeffs = np.zeros(n_samples)
    for k in range(n_samples):
        value = 0.0
        for n in range(n_samples):
            value += signal[n] * np.cos(np.pi / n_samples * (n + 0.5) * k)
        scale = np.sqrt(1 / n_samples) if k == 0 else np.sqrt(2 / n_samples)
        dct_coeffs[k] = value * scale
    return dct_coeffs

# IDCT implementation to reconstruct the signal
def custom_idct(dct_coeffs):
    n_samples = len(dct_coeffs)
    reconstructed = np.zeros(n_samples)
    for n in range(n_samples):
        value = 0.0
        for k in range(n_samples):
            scale = np.sqrt(1 / n_samples) if k == 0 else np.sqrt(2 / n_samples)
            value += scale * dct_coeffs[k] * np.cos(np.pi / n_samples * (n + 0.5) * k)
        reconstructed[n] = value
    return reconstructed

# DCT compression and reconstruction with overlap-add
def process_dct(signal, chunk_size, overlap, compression_ratio, plot_coefficients=False, verbose=False):
    hop_size = chunk_size - overlap
    num_chunks = (len(signal) - overlap) // hop_size
    reconstructed_signal = np.zeros(len(signal))
    normalization_factor = np.zeros(len(signal))  # Track overlapping contributions

    for i in range(num_chunks):
        start_idx = i * hop_size
        end_idx = start_idx + chunk_size
        chunk = signal[start_idx:end_idx]

        # Perform DCT
        dct_coeffs = custom_dct(chunk)

        # Apply compression by zeroing out small coefficients
        threshold = np.percentile(np.abs(dct_coeffs), (1 - compression_ratio) * 100)
        compressed_coeffs = np.where(np.abs(dct_coeffs) >= threshold, dct_coeffs, 0)

        if verbose:
            retained = np.count_nonzero(compressed_coeffs)
            print(f"Chunk {i + 1}/{num_chunks}: Threshold={threshold:.4f}, Retained Coefficients={retained}")

        if plot_coefficients:
            plot_dct_coeffs(dct_coeffs, compressed_coeffs, chunk_num=i + 1)

        # Reconstruct chunk using IDCT
        reconstructed_chunk = custom_idct(compressed_coeffs)

        # Overlap-add the chunk to the reconstructed signal
        reconstructed_signal[start_idx:end_idx] += reconstructed_chunk
        normalization_factor[start_idx:end_idx] += 1  # Count contributions from overlapping chunks

    # Normalize to compensate for overlapping contributions
    reconstructed_signal /= normalization_factor
    return reconstructed_signal

# Plot DCT coefficients for a single chunk
def plot_dct_coeffs(original, compressed, chunk_num):
    plt.figure(figsize=(10, 5))
    plt.plot(original, label="Original Coefficients", color="blue", linewidth=1.2)
    plt.plot(compressed, label="Compressed Coefficients", color="orange", linestyle="--", linewidth=1.2)
    plt.title(f"DCT Coefficients (Chunk {chunk_num})")
    plt.xlabel("Index")
    plt.ylabel("Value")
    plt.legend()
    plt.grid()
    plt.show()

# Plot original and reconstructed signals
def plot_signals(original, reconstructed, samplerate, duration):
    time = np.linspace(0, len(original) / samplerate, len(original), endpoint=False)
    plt.figure(figsize=(12, 6))
    plt.plot(time[:int(duration * samplerate)], original[:int(duration * samplerate)], label="Original Signal")
    plt.plot(time[:int(duration * samplerate)], reconstructed[:int(duration * samplerate)], linestyle="--", label="Reconstructed Signal")
    plt.title("Original vs Reconstructed Signal")
    plt.xlabel("Time (s)")
    plt.ylabel("Amplitude")
    plt.legend()
    plt.grid()
    plt.show()

# Main function for signal generation and processing
def main():
    samplerate = 44100
    duration = 0.05
    frequencies = [220, 440, 880, 1760, 533]
    amplitudes = [0.5, 0.25, 0.125, 0.0625, 0.6]

    # Generate signal as a sum of sine waves
    t = np.linspace(0, duration, int(samplerate * duration), endpoint=False)
    signal = sum(a * np.sin(2 * np.pi * f * t) for a, f in zip(amplitudes, frequencies))
    # Uncomment the line below to add noise to the signal
    signal += np.random.normal(0, 0.05, len(t))

    # DCT processing parameters
    chunk_size = 32
    overlap = 16
    compression_ratio = 1.0
    show_plots = False
    debug = True

    # Process and reconstruct the signal
    reconstructed_signal = process_dct(signal, chunk_size, overlap, compression_ratio, show_plots, debug)

    # Plot original and reconstructed signals
    plot_signals(signal, reconstructed_signal, samplerate, duration)

if __name__ == "__main__":
    main()
