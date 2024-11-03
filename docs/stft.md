**Discrete Short-Time Fourier Transform (STFT)**

$\text{STFT}(m, k) = \sum_{n=0}^{N-1} x[n + m \cdot H] \cdot w[n] \cdot e^{-j \frac{2 \pi k n}{N}}$

where:
- \( m \) is the time step, representing each window position, with each step separated by \( H \) samples (the hop size).
- \( k \) is the frequency bin index, representing different frequency components.
- \( N \) is the window length (number of samples in each segment).
- \( $x[n + m \cdot H]$ \) is the segment of the signal starting at sample \( m \cdot H \).
- \( w[n] \) is the window function applied to each segment to smooth the edges. 
- \( $e^{-j \frac{2 \pi k n}{N}}$ \) is the discrete Fourier transform, where \( k \) is the frequency bin and \( n \) is the sample index within the window.

