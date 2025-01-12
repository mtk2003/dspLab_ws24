# DCT Project

## How to Run

1. **Install Dependencies**:  
   Ensure [Poetry](https://python-poetry.org/docs/#installation) is installed, then run:
   ```bash
   poetry install
2. **Run the script**:
    ```bash
    poetry run dct

## DCT and IDCT

### Discrete Cosine Transform (DCT-II)
The formula for the \( k \)-th DCT coefficient is:

$$
X[k] = \alpha(k) \sum_{n=0}^{N-1} x[n] \cos\left(\frac{\pi}{N} \left(n + 0.5\right) k \right)
$$

Where:
- \( X[k] \): DCT coefficient for frequency \( k \)
- \( x[n] \): Original signal at time \( n \)
- \( N \): Number of samples
- \( \alpha(k) \): Normalization factor:
  $$
  \alpha(k) =
  \begin{cases}
  \sqrt{\frac{1}{N}}, & \text{if } k = 0 \\
  \sqrt{\frac{2}{N}}, & \text{if } k > 0
  \end{cases}
  $$

---

### Inverse Discrete Cosine Transform (IDCT)
The formula for reconstructing \( x[n] \) from \( X[k] \) is:

$$
x[n] = \sum_{k=0}^{N-1} \alpha(k) X[k] \cos\left(\frac{\pi}{N} \left(n + 0.5\right) k \right)
$$

Where:
- \( x[n] \): Reconstructed signal at time \( n \)
- \( X[k] \): DCT coefficient for frequency \( k \)
- \( \alpha(k) \): Same normalization factor as in DCT:
  $$
  \alpha(k) =
  \begin{cases}
  \sqrt{\frac{1}{N}}, & \text{if } k = 0 \\
  \sqrt{\frac{2}{N}}, & \text{if } k > 0
  \end{cases}
  $$

