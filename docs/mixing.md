### Time-Domain Mixing

$$
x_\text{mixed}(t) = x(t) \cdot \cos(2 \pi f_\text{mix} t)
$$

For a signal with a single frequency $f_s$:

$$
x(t) = A \cos(2 \pi f_s t)
$$

The mixed signal becomes:

$$
x_\text{mixed}(t) = A \cos(2 \pi f_s t) \cdot \cos(2 \pi f_\text{mix} t)
$$

Expanding using the trigonometric identity:

$$
\cos(u) \cdot \cos(v) = \frac{1}{2} \left[ \cos(u + v) + \cos(u - v) \right]
$$

we get:

$$
x_\text{mixed}(t) = \frac{A}{2} \left[ \cos(2 \pi (f_s + f_\text{mix}) t) + \cos(2 \pi (f_s - f_\text{mix}) t) \right]
$$

---

### Frequency-Domain Representation

In the frequency domain, the original signal $x(t)$ has a spectrum $X(f)$ centered at $\pm f_s$.

After mixing with $\cos(2 \pi f_\text{mix} t)$, the frequency domain representation becomes:

$$
X_\text{mixed}(f) = \frac{1}{2} \left[ X(f - f_\text{mix}) + X(f + f_\text{mix}) \right]
$$

This means the original spectrum is shifted:

1. **Downward by $f_\text{mix}$:**
   - $X(f - f_\text{mix})$, shifting the spectrum to $\pm (f_s - f_\text{mix})$.

2. **Upward by $f_\text{mix}$:**
   - $X(f + f_\text{mix})$, shifting the spectrum to $\pm (f_s + f_\text{mix})$.

---

### Amplitude Scaling

Each new frequency component has half the amplitude of the original signal, due to the $\frac{1}{2}$ scaling factor in the cosine product identity.
