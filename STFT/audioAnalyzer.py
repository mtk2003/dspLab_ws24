import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import sounddevice as sd

# STFT Parameters
fs = 48000  
frame_size = 1024      
overlap = 512         
hop_size = frame_size - overlap  
num_ffts = 400      
num_fft_bins = frame_size // 2   
buffer_size = frame_size + overlap  

# Frequency and frame-index vectors for plotting
frequency_vector = np.linspace(0, fs / 2, num_fft_bins)
time_vector = np.arange(num_ffts)

# Circular buffer, its write_idx and STFT_mat for audio read and process
# They are define as global within the calling functions
circ_buffer = np.zeros(buffer_size)
write_idx = 0
stft_mat = np.zeros((num_fft_bins, num_ffts))

# Initialize global variables for plotting
fig, ax, img = None, None, None

# Read current frame form circular buffer
def get_frame():
    # Read indices for start and stop
    start_idx = (write_idx - frame_size) % buffer_size
    if start_idx + frame_size <= buffer_size:
        return circ_buffer[start_idx:start_idx + frame_size]
    else:
        end_part = circ_buffer[start_idx:]
        start_part = circ_buffer[:frame_size - len(end_part)]
        return np.concatenate((end_part, start_part))
    
# FFT of windowed buffer
def compute_windowed_fft(frame):
    w = np.hamming(len(frame))
    #w = np.ones_like(frame)
    windowed_signal = frame * w
    amplitude_spec = np.abs(np.fft.fft(windowed_signal))[:num_fft_bins]
    return amplitude_spec

# Process callback. Called each time new hop_size samples are available
# API: https://python-sounddevice.readthedocs.io/en/0.3.14/api.html#sounddevice.InputStream
def process_frame(indata,frames,time, status):
    if status:
        print(status)
    # Modification of global variables
    global write_idx, circ_buffer, stft_mat, img
    
    # Left/rigth channel
    # Number of channels must be set to two!
    mono_channel = indata[:, 1]
    print("Number of new audio samples: ", frames)

    # Write audio samples (hop_size) to circular buffer
    for sample in mono_channel:
        circ_buffer[write_idx] = sample
        write_idx = (write_idx + 1) % buffer_size

    # Get audio samples of current frame (overlap with previous frame)
    frame = get_frame()
    
    # Compute windowed FFT of current frame
    amplitude_spec = compute_windowed_fft(frame)

    # STFT matrix: Each iteration the latest FFT will be added as column to the right
    stft_mat = np.roll(stft_mat, -1, axis=1)
    stft_mat[:, -1] = amplitude_spec

    # Update axis
    img.set_array(stft_mat)

def main():
    global img, fig, ax

    # Mirophone Debug info
    #device_info = sd.query_devices(kind='input')
    #device_sample_rate = device_info['default_samplerate']
    #print(f"Default sample rate: {device_sample_rate} Hz")

    # Initialize animation plot
    fig, ax = plt.subplots()
    img = ax.imshow(stft_mat, aspect='auto', origin='lower', cmap='viridis', 
                    extent=[time_vector[0], time_vector[-1], frequency_vector[0], frequency_vector[-1]])
    cbar = plt.colorbar(img, ax=ax)
    cbar.set_label("Amplitude Spectrum")
    ax.set_title("Real-Time Spectrogram")
    ax.set_xlabel("Time (frames)")
    ax.set_ylabel("Frequency (Hz)")

    # Stream audio
    try:
        with sd.InputStream(samplerate=fs, channels=2, blocksize=hop_size, callback=process_frame):
            ani = animation.FuncAnimation(fig, lambda x: img.set_array(stft_mat), interval=12, cache_frame_data=False)
            plt.show()
    except Exception as e:
        print(f"Error with audio stream: {e}")

if __name__ == "__main__":
    main()
