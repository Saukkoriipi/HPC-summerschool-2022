#include <hip/hip_runtime.h>
#include <stdio.h>
#include <math.h>

// TODO: add a device kernel that calculates y = a * x + y

__global__ void saxpy_(int n, float a, float *x, float *y)
{
   int tid = threadIdx.x + blockIdx.x * blockDim.x;
   int stride = gridDim.x * blockDim.x;
   for (; tid<n; tid+=stride){
	y[tid] += a*x[tid];
   }
}


int main(void)
{
    int i;
    const int n = 10000;
    float a = 3.4;
    float x[n], y[n], y_ref[n];
    float *x_, *y_;

    // initialise data and calculate reference values on CPU
    for (i=0; i < n; i++) {
        x[i] = sin(i) * 2.3;
        y[i] = cos(i) * 1.1;
        y_ref[i] = a * x[i] + y[i];
    }

    // TODO: allocate vectors x_ and y_ on the GPU
    hipMalloc((void **) &x_, sizeof(float) * n);
    hipMalloc((void **) &y_, sizeof(float) * n);
    // TODO: copy initial values from CPU to GPU (x -> x_ and y -> y_)
    hipMemcpy(x_, x, sizeof(float)*n, hipMemcpyHostToDevice);
    hipMemcpy(y_, y, sizeof(float)*n, hipMemcpyHostToDevice);

    // TODO: define grid dimensions
    dim3 blocks(32);
    dim3 threads(256);
    // TODO: launch the device kernel
    hipLaunchKernelGGL(saxpy_, blocks, threads, 0, 0, n, a, x_, y_);

    // TODO: copy results back to CPU (y_ -> y)
    hipMemcpy(y, y_, sizeof(float)*n, hipMemcpyDeviceToHost);

    // Print reference and results
    printf("reference: %f %f %f %f ... %f %f\n",
            y_ref[0], y_ref[1], y_ref[2], y_ref[3], y_ref[n-2], y_ref[n-1]);
    printf("   result: %f %f %f %f ... %f %f\n",
            y[0], y[1], y[2], y[3], y[n-2], y[n-1]);


    // confirm that results are correct
    float error = 0.0;
    float tolerance = 1e-6;
    float diff;
    for (i=0; i < n; i++) {
        diff = abs(y_ref[i] - y[i]);
        if (diff > tolerance)
            error += diff;
    }
    printf("total error: %f\n", error);
    printf("  reference: %f at (42)\n", y_ref[42]);
    printf("     result: %f at (42)\n", y[42]);

    return 0;
}
