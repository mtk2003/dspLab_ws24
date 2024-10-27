# Introduction

In radar and signal processing, detecting targets within noisy data is a fundamental challenge. The Discrete Fourier Transform (DFT) is commonly used to convert time-domain signals into the frequency domain, where peak detection can be performed. To maintain a consistent detection performance, algorithms like Constant False Alarm Rate (CFAR) are employed. Specifically, Cell Averaging CFAR (CA-CFAR) adapts the detection threshold based on the local noise estimate.

1. Processing the Complex DFT Output to Obtain the Power Spectrum.
2. Deriving the Exponential Distribution for the Noise Power.
3. Calculating the Cumulative Distribution Function (CDF) from the Probability Density Function (PDF).
4. Deriving the CFAR Detection Threshold Using Integrals.
5. Step-by-Step Integral Calculations and Transitions from PDF to CDF.

---

## 1. From Complex DFT Output to Power Spectrum

**Complex DFT Output:**

Given a time-domain signal \( x(n) \), its DFT is:

$$
X(k) = \sum_{n=0}^{N-1} x(n) e^{-j2\pi kn/N}, \quad k=0,1,\dots,N-1
$$

- \( X(k) \) is a complex number representing the amplitude and phase of frequency component \( k \).
- The DFT transforms the signal from the time domain to the frequency domain.

**Power Spectrum Calculation:**

To analyze the signal's power at each frequency, compute the magnitude squared of \( X(k) \):

$$
P(k) = |X(k)|^2 = \text{Re}\{X(k)\}^2 + \text{Im}\{X(k)\}^2
$$

- \( P(k) \) represents the power at frequency bin \( k \).
- The power spectrum provides a real-valued function of frequency, suitable for detection algorithms.

---

## 2. Deriving the Exponential Distribution for Noise Power

**Assumptions:**

- The signal is contaminated with additive white Gaussian noise (AWGN).
- The noise in the time domain is a complex Gaussian random variable with zero mean and variance \( \sigma^2 \).

**Noise in the Frequency Domain:**

- Due to the linearity of the DFT, the noise in the frequency domain remains a complex Gaussian random variable.
- At each frequency bin \( k \), the noise component \( N(k) \) is complex Gaussian with zero mean and variance \( \sigma^2 \).

**Magnitude Squared of Noise:**

- The magnitude squared of a complex Gaussian random variable follows an exponential distribution.

**Derivation of the Exponential Distribution:**

Let \( N(k) = N_{\text{re}}(k) + jN_{\text{im}}(k) \), where:

- \( N_{\text{re}}(k) \) and \( N_{\text{im}}(k) \) are independent real Gaussian random variables.
- Both have zero mean and variance \( \sigma^2/2 \) (since the total variance is \( \sigma^2 \)).

**Joint PDF of \( N_{\text{re}} \) and \( N_{\text{im}} \):**

$$
f_{N_{\text{re}}, N_{\text{im}}}(x, y) = \frac{1}{\pi \sigma^2} \exp\left( -\frac{x^2 + y^2}{\sigma^2} \right)
$$

**Magnitude Squared \( P = N_{\text{re}}^2 + N_{\text{im}}^2 \):**

We want to find the PDF of \( P \).

**Transformation to Polar Coordinates:**

- \( x = r\cos\theta \)
- \( y = r\sin\theta \)
- \( P = r^2 \)
- Jacobian determinant \( J = r \)

**Compute the PDF of \( P \):**

$$
f_P(p) = \int_{0}^{2\pi} f_{N_{\text{re}}, N_{\text{im}}}(r\cos\theta, r\sin\theta) |J|\, d\theta
$$

**Substitute \( x \) and \( y \):**

$$
f_P(p) = \int_{0}^{2\pi} \frac{1}{\pi \sigma^2} \exp\left( -\frac{r^2}{\sigma^2} \right) r\, d\theta
$$

Since the integrand is independent of \( \theta \), integrate over \( \theta \):

$$
f_P(p) = \frac{1}{\pi \sigma^2} \exp\left( -\frac{p}{\sigma^2} \right) p^{1/2} \int_{0}^{2\pi} d\theta = \frac{1}{\sigma^2} \exp\left( -\frac{p}{\sigma^2} \right)
$$

**Conclusion:**

- The PDF of \( P \) is:

  $$
  f_P(p) = \frac{1}{\sigma^2} \exp\left( -\frac{p}{\sigma^2} \right), \quad p \geq 0
  $$

- \( P \) follows an exponential distribution with parameter \( \lambda = \sigma^2 \).

---

## 3. From PDF to CDF

**Probability Density Function (PDF):**

$$
f_P(p) = \frac{1}{\sigma^2} \exp\left( -\frac{p}{\sigma^2} \right)
$$

**Cumulative Distribution Function (CDF):**

The CDF \( F_P(p) \) is the probability that \( P \) is less than or equal to \( p \):

$$
F_P(p) = P(P \leq p) = \int_{0}^{p} f_P(x)\, dx
$$

**Computing the Integral:**

$$
F_P(p) = \int_{0}^{p} \frac{1}{\sigma^2} \exp\left( -\frac{x}{\sigma^2} \right) dx
$$

**Integrate \( f_P(x) \):**

Let \( u = \frac{x}{\sigma^2} \), then \( du = \frac{dx}{\sigma^2} \), so \( dx = \sigma^2 du \).

Change the limits of integration:

- When \( x = 0 \), \( u = 0 \)
- When \( x = p \), \( u = \frac{p}{\sigma^2} \)

Substitute:

$$
F_P(p) = \int_{0}^{\frac{p}{\sigma^2}} \exp(-u)\, du = \left[ -\exp(-u) \right]_0^{\frac{p}{\sigma^2}} = 1 - \exp\left( -\frac{p}{\sigma^2} \right)
$$

**Conclusion:**

- The CDF of \( P \) is:

  $$
  F_P(p) = 1 - \exp\left( -\frac{p}{\sigma^2} \right)
  $$

---

## 4. Deriving the CFAR Detection Threshold

**Objective:**

- Determine the threshold \( T \) such that the probability of false alarm \( P_{\text{fa}} \) is maintained:

  $$
  P(P > T) = P_{\text{fa}}
  $$

**Calculating \( P(P > T) \):**

- Since \( P(P > T) = 1 - F_P(T) \):

  $$
  P(P > T) = \exp\left( -\frac{T}{\sigma^2} \right)
  $$

**Solving for \( T \):**

- Set \( P(P > T) = P_{\text{fa}} \):

  $$
  \exp\left( -\frac{T}{\sigma^2} \right) = P_{\text{fa}}
  $$

- Take the natural logarithm on both sides:

  $$
  -\frac{T}{\sigma^2} = \ln(P_{\text{fa}}) \quad \implies \quad T = -\sigma^2 \ln(P_{\text{fa}})
  $$

However, in practice, \( \sigma^2 \) is unknown and estimated from the reference cells.

**Including the Reference Cells (CA-CFAR):**

- **Reference Cells:** Used to estimate the local noise power.
- **Number of Reference Cells:** \( N_r \)
- **Sum of Reference Cell Powers:**

  $$
  S = \sum_{i=1}^{N_r} P_i
  $$

- Each \( P_i \) is an independent exponential random variable with parameter \( \lambda = \sigma^2 \).

**Distribution of \( S \):**

- \( S \) follows a Gamma distribution with shape parameter \( k = N_r \) and scale parameter \( \theta = \sigma^2 \):

  $$
  f_S(s) = \frac{1}{\Gamma(N_r)(\sigma^2)^{N_r}} s^{N_r - 1} \exp\left( -\frac{s}{\sigma^2} \right)
  $$

**Average Noise Power \( \bar{P} \):**

$$
\bar{P} = \frac{S}{N_r}
$$

- \( \bar{P} \) is also a scaled Gamma-distributed random variable.

**Threshold in CA-CFAR:**

- Set the threshold as:

  $$
  T = \alpha \bar{P}
  $$

- \( \alpha \) is the threshold multiplier to be determined.

**Probability of False Alarm:**

- Under the noise-only hypothesis, \( P_{\text{CUT}} \) and \( \bar{P} \) are independent.

- The probability of false alarm is:

  $$
  P_{\text{fa}} = P\left( P_{\text{CUT}} > \alpha \bar{P} \right)
  $$

**Deriving \( P_{\text{fa}} \):**

1. **Express \( P_{\text{fa}} \) as an Integral:**

   $$
   P_{\text{fa}} = \int_{0}^{\infty} P\left( P_{\text{CUT}} > \alpha p \right) f_{\bar{P}}(p)\, dp
   $$

2. **Compute \( P\left( P_{\text{CUT}} > \alpha p \right) \):**

   - Since \( P_{\text{CUT}} \) is exponential:

     $$
     P\left( P_{\text{CUT}} > \alpha p \right) = \exp\left( -\frac{\alpha p}{\sigma^2} \right)
     $$

3. **Substitute into the Integral:**

   $$
   P_{\text{fa}} = \int_{0}^{\infty} \exp\left( -\frac{\alpha p}{\sigma^2} \right) f_{\bar{P}}(p)\, dp
   $$

4. **Substitute \( f_{\bar{P}}(p) \):**

   Since \( \bar{P} = \frac{S}{N_r} \), and \( S \) follows a Gamma distribution, the PDF of \( \bar{P} \) is:

   $$
   f_{\bar{P}}(p) = \frac{N_r^{N_r}}{\Gamma(N_r)(\sigma^2)^{N_r}} p^{N_r - 1} \exp\left( -\frac{N_r p}{\sigma^2} \right)
   $$

5. **Write the Complete Integral:**

   $$
   P_{\text{fa}} = \frac{N_r^{N_r}}{\Gamma(N_r)(\sigma^2)^{N_r}} \int_{0}^{\infty} p^{N_r - 1} \exp\left( -\frac{(\alpha + N_r) p}{\sigma^2} \right) dp
   $$

6. **Compute the Integral Using the Gamma Function:**

   The integral is of the form:

   $$
   \int_{0}^{\infty} x^{k - 1} e^{-b x} dx = \frac{\Gamma(k)}{b^{k}}
   $$

   Here:

   - \( x = p \)
   - \( k = N_r \)
   - \( b = \frac{\alpha + N_r}{\sigma^2} \)

   So:

   $$
   \int_{0}^{\infty} p^{N_r - 1} \exp\left( -\frac{(\alpha + N_r) p}{\sigma^2} \right) dp = \frac{\Gamma(N_r)}{\left( \frac{\alpha + N_r}{\sigma^2} \right)^{N_r}}
   $$

7. **Substitute Back into \( P_{\text{fa}} \):**

   $$
   P_{\text{fa}} = \frac{N_r^{N_r}}{\Gamma(N_r)(\sigma^2)^{N_r}} \times \frac{\Gamma(N_r)}{\left( \frac{\alpha + N_r}{\sigma^2} \right)^{N_r}} = \left( \frac{N_r}{\alpha + N_r} \right)^{N_r}
   $$

**Solving for \( \alpha \):**

- Set \( P_{\text{fa}} = \left( \frac{N_r}{\alpha + N_r} \right)^{N_r} \)

- Take the natural logarithm on both sides:

  $$
  \ln(P_{\text{fa}}) = N_r \ln\left( \frac{N_r}{\alpha + N_r} \right)
  $$

- Divide both sides by \( N_r \):

  $$
  \frac{\ln(P_{\text{fa}})}{N_r} = \ln\left( \frac{N_r}{\alpha + N_r} \right)
  $$

- Exponentiate both sides:

  $$
  \exp\left( \frac{\ln(P_{\text{fa}})}{N_r} \right) = \frac{N_r}{\alpha + N_r}
  $$

- Simplify:

  $$
  P_{\text{fa}}^{1/N_r} = \frac{N_r}{\alpha + N_r}
  $$

- Rearranged:

  $$
  \alpha + N_r = \frac{N_r}{P_{\text{fa}}^{1/N_r}}
  $$

- Solve for \( \alpha \):

  $$
  \alpha = N_r \left( P_{\text{fa}}^{-1 / N_r} - 1 \right)
  $$

---

## 5. Step-by-Step Integral Calculations

**Integral in Detail:**

We start from:

$$
P_{\text{fa}} = \int_{0}^{\infty} \exp\left( -\frac{\alpha p}{\sigma^2} \right) f_{\bar{P}}(p)\, dp
$$

**Substitute \( f_{\bar{P}}(p) \):**

$$
f_{\bar{P}}(p) = \frac{N_r^{N_r}}{\Gamma(N_r)(\sigma^2)^{N_r}} p^{N_r - 1} \exp\left( -\frac{N_r p}{\sigma^2} \right)
$$

**Complete Integral:**

$$
P_{\text{fa}} = \frac{N_r^{N_r}}{\Gamma(N_r)(\sigma^2)^{N_r}} \int_{0}^{\infty} p^{N_r - 1} \exp\left( -\frac{(\alpha + N_r) p}{\sigma^2} \right) dp
$$

**Let \( b = \frac{\alpha + N_r}{\sigma^2} \).**

**Integral Simplifies to:**

$$
\int_{0}^{\infty} p^{N_r - 1} e^{-b p} dp = \frac{\Gamma(N_r)}{b^{N_r}}
$$

**Plug Back Into \( P_{\text{fa}} \):**

$$
P_{\text{fa}} = \frac{N_r^{N_r}}{(\sigma^2)^{N_r}} \times \frac{1}{b^{N_r}} = \left( \frac{N_r}{\alpha + N_r} \right)^{N_r}
$$

**Explanation of Each Step:**

- **Combining Exponential Terms:**

  The exponents in the exponential function are added when multiplying:

  $$
  e^{-A p} \times e^{-B p} = e^{-(A + B)p}
  $$

- **Gamma Function Integral:**

  The integral of the form \( \int_{0}^{\infty} x^{k - 1} e^{-b x} dx \) is a standard result in probability and statistics, known to equal \( \Gamma(k)/b^{k} \).

- **Substituting \( b \) Back:**

  The parameter \( b \) includes \( \alpha \), which we need to solve for.

- **Simplifying Fractions:**

  Careful algebraic manipulation is used to simplify the expression, canceling out \( (\sigma^2)^{N_r} \) and combining like terms.

---

## 6. Summary of the Process

- **Compute the Power Spectrum:**
  - From the complex DFT output \( X(k) \), calculate \( P(k) = |X(k)|^2 \).

- **Model the Noise Statistics:**
  - The noise in each frequency bin is complex Gaussian.
  - The magnitude squared follows an exponential distribution.

- **Derive the PDF and CDF:**
  - Derive the PDF \( f_P(p) \) and CDF \( F_P(p) \) of the noise power.

- **Set the Desired Probability of False Alarm \( P_{\text{fa}} \):**
  - Determine \( T \) such that \( P(P > T) = P_{\text{fa}} \).

- **Include Reference Cells in CFAR:**
  - Estimate the noise power \( \bar{P} \) from \( N_r \) reference cells.
  - Recognize that \( \bar{P} \) follows a Gamma distribution.

- **Derive the CFAR Threshold Multiplier \( \alpha \):**
  - Use integrals and the properties of the Gamma distribution to derive \( \alpha \).

- **Calculate the Detection Threshold:**
  - Set \( T = \alpha \bar{P} \).

- **Perform Detection:**
  - Compare \( P_{\text{CUT}} \) to \( T \) to decide if a target is present.

---

## 7. Practical Application Example

**Given Parameters:**

- \( N_r = 16 \)
- Desired \( P_{\text{fa}} = 1 \times 10^{-4} \)

**Compute \( \alpha \):**

$$
P_{\text{fa}}^{1/N_r} = \left( 1 \times 10^{-4} \right)^{1/16} \approx e^{\frac{\ln(10^{-4})}{16}} \approx e^{-0.5754}
$$

Compute:

$$
P_{\text{fa}}^{-1/N_r} = e^{0.5754} \approx 1.777
$$

Calculate \( \alpha \):

$$
\alpha = N_r \left( P_{\text{fa}}^{-1 / N_r} - 1 \right) = 16 \times (1.777 - 1) = 16 \times 0.777 = 12.432
$$

**Set Threshold:**

- For each cell under test:

  $$
  T = \alpha \bar{P} = 12.432 \times \bar{P}
  $$

---

