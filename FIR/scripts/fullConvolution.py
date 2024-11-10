import numpy as np

input_signal = np.array([1, 2, 3, 4, 5])
filter_coeffs = np.array([3, 1, 2])

# Full convolution sum
def full_convolution(input_signal, filter_coeffs):
    input_length = len(input_signal)
    filter_length = len(filter_coeffs)
    output_length = input_length + filter_length - 1
    output_signal = np.zeros(output_length)

    for n in range(output_length):
        for k in range(filter_length):
            index = n - k
            if 0 <= index < input_length:
                output_signal[n] += filter_coeffs[k] * input_signal[index]
    
    return output_signal


# Different convolution options
conv_sum = full_convolution(input_signal, filter_coeffs)
numpy_output = np.convolve(input_signal, filter_coeffs, mode='full')
numpy_output_same = np.convolve(input_signal, filter_coeffs, mode='same')
numpy_correlate = np.correlate(input_signal, filter_coeffs, mode='full')
conv_sum_corr = full_convolution(input_signal, filter_coeffs[::-1])
print("Convolution Output:", conv_sum)
print("NumPy Convolution Output: ", numpy_output)
print("Numpy Convolution Output Mode=Same :", numpy_output_same)
print("Numpy Correlate Output: ", numpy_correlate)
print("Correlate Output: ", conv_sum_corr)
