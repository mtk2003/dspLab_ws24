import os
import numpy as np
import pandas as pd
from scipy.signal import butter, zpk2sos

sampling_frequency = 48e3  
cutoff_frequency = 10e3     # Desired cutoff frequency in Hz
order = 128                # Filter order

# Normalize the cutoff frequency
normalized_cutoff = cutoff_frequency / (sampling_frequency / 2)

# Construct LP filter
z, p, k = butter(order, normalized_cutoff, output='zpk')  # Butterworth filter design
sos = zpk2sos(z, p, k)
prod_inv_stage_gain = 1

rows = []
for row in sos:
    b = row[:3]  # Extract b0, b1, b2
    a = row[3:]  # Extract a0, a1, a2
    
    # Calculate the stage gain
    stage_gain = np.sum(b) / np.sum(a)
    inv_stage_gain = 1 / stage_gain
    
    rows.append([
        inv_stage_gain,   
        b[0], b[1], b[2],  
        a[0], a[1], a[2]  
    ])

    prod_inv_stage_gain *= inv_stage_gain

data_folder = os.path.join(os.path.dirname(__file__), '../data')
os.makedirs(data_folder, exist_ok=True) 
csv_filename = os.path.join(data_folder, 'iir_biquads.csv')

# Save the coefficients and stage gain to the CSV file
columns = ['inv_gain', 'b0', 'b1', 'b2', 'a0', 'a1', 'a2']
df = pd.DataFrame(rows, columns=columns)
df.to_csv(csv_filename, index=False, header=False)  # Save without headers for MATLAB-like format

# Debug
print(f"IIR biquad coefficients with a0 included saved to {csv_filename}")
print(f"Filter details:")
print(f"- Sampling Frequency: {sampling_frequency} Hz")
print(f"- Cutoff Frequency: {cutoff_frequency} Hz")
print(f"- Filter Order: {order}")
print(f"- Number of Biquad Stages: {sos.shape[0]}")
print(f"- Product of Inverse Stage Gains: {prod_inv_stage_gain}")
