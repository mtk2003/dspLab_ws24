K-means Clustering Algorithm

K-means is an iterative algorithm used to partition a dataset into k clusters, where each cluster is represented by a centroid. The objective of K-means is to minimize the distance between each data point and its assigned centroid.

Steps of the K-means Algorithm

Step 1: Initialize Centroids

1. Choose the number of clusters, k.


2. Select k initial centroids (cluster centers) randomly from the data points.



Step 2: Assign Data Points to the Nearest Centroid

For each data point :

1. Calculate the distance from to each centroid .

If data points are one-dimensional, the distance can be the absolute difference:




$d(x_i, C_j) = |x_i - C_j|$

$d(x_i, C_j) = \sqrt{(x_i^{\text{real}} - C_j^{\text{real}})^2 + (x_i^{\text{imag}} - C_j^{\text{imag}})^2}$

$\text{Cluster}(x_i) = \arg \min_{j} d(x_i, C_j)$

Step 3: Update Centroids

After assigning all data points to clusters, update each centroid to be the mean of the points in its cluster:

For each centroid :

1. Calculate the mean of all points assigned to cluster :



$C_j = \frac{1}{n_j} \sum_{x_i \in \text{Cluster}(j)} x_i$

Step 4: Check for Convergence

1. If no points changed clusters during Step 2, or if the maximum number of iterations is reached, the algorithm stops.


2. Otherwise, repeat Steps 2 and 3 with the updated centroids.



K-means Objective (Mathematics Behind the Goal)

The goal of K-means is to minimize the within-cluster sum of squares (WCSS), which represents the distance between each point and its assigned centroid:

$\text{WCSS} = \sum_{j=1}^{k} \sum_{x_i \in \text{Cluster}(j)} d(x_i, C_j)^2$

This objective encourages tight, cohesive clusters where each point is close to its cluster center.


---

Summary of Steps

1. Initialize: Randomly select k initial centroids.


2. Assign: Assign each data point to the nearest centroid.


3. Update: Recalculate each centroid as the mean of its assigned points.


4. Repeat: Continue until no points change clusters or the maximum iterations are reached.
