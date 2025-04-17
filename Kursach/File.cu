#include "cuda_runtime.h"
#include "device_launch_parameters.h"
#include "cmath"

#include <stdio.h>
#include <fstream>

int Height;
int Width;
int Seed;


cudaError_t generatePerlineNoise(float* c);
bool SaveArrayInTxt(float* array);

__device__ float Frac(float xFloat)
{
    int xInt = fabs(xFloat);
    return fabs(xFloat) - xInt;
}

__device__ float Modulo(float a, float b) {
    int intaonb = a / b;
    return a - b * intaonb;
}

__device__ float Dot(float2 vectorLeft, float2 vectorRight)
{
    return vectorLeft.x * vectorRight.x + vectorLeft.y * vectorRight.y;
}

__device__ float Rand(float2 coordinate, int seed)
{
    float a = 12.9898;

    float b = 78.233;

    float c = 43758.5453;

    float dt = (coordinate.x + seed) * a + (coordinate.y + seed) * b;

    float sn = Modulo(dt, 2 * 3.141592653589793f);

    return Frac(sin(sn) * c);
}

__device__ float Rand(int2 xInt)
{
    float2 xFloat;
    xFloat.x = xInt.x;
    xFloat.y = xInt.y;
    return Rand(xFloat, 5);
}

__device__ float GenerateNoiseWithResolution(int2 uv)
{
    float randomNumber = Rand(uv);
    return randomNumber;
}

__device__ double cubicInterpolate(double p[4], double x) {
    return p[1] + 0.5 * x * (p[2] - p[0] + x * (2.0 * p[0] - 5.0 * p[1] + 4.0 * p[2] -
        p[3] + x * (3.0 * (p[1] - p[2]) + p[3] - p[0])));
}

__device__ double bicubicInterpolate(double p[4][4], double x, double y) {
    double arr[4];
    arr[0] = cubicInterpolate(p[0], y);
    arr[1] = cubicInterpolate(p[1], y);
    arr[2] = cubicInterpolate(p[2], y);
    arr[3] = cubicInterpolate(p[3], y);
    return cubicInterpolate(arr, x);
}

__device__ float BilinearInterpolation(float f00, float f01, float f10, float f11, int x0, int x1, int y0, int y1, float2 uv)
{
    float fR1 = (x1 - uv.x) / (x1 - x0) * f00 + (uv.x - x0) / (x1 - x0) * f10;
    float fR2 = (x1 - uv.x) / (x1 - x0) * f01 + (uv.x - x0) / (x1 - x0) * f11;
    return (y1 - uv.y) / (y1 - y0) * fR1 + (uv.y - y0) / (y1 - y0) * fR2;
}

__device__ float GenerateOctaveWithBicubic(float2 uv, int coeficient, int seed)
{
    double p[4][4];
    float u, v;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            float2 coordCoef = make_float2(0, 0);
            coordCoef.x = float(int(uv.x / (1.f / coeficient)) + i - 1) / coeficient + coeficient;
            coordCoef.y = float(int(uv.y / (1.f / coeficient)) + j - 1) / coeficient + coeficient;
            p[i][j] = Rand(coordCoef, seed);
        }
    }

    u = (uv.x - float(int(uv.x / (1.f / coeficient))) / coeficient) / (float(int(uv.x / (1.f / coeficient)) + 1) / coeficient - float(int(uv.x / (1.f / coeficient))) / coeficient);
    v = (uv.y - float(int(uv.y / (1.f / coeficient))) / coeficient) / (float(int(uv.y / (1.f / coeficient)) + 1) / coeficient - float(int(uv.y / (1.f / coeficient))) / coeficient);

    return bicubicInterpolate(p, u, v);
}

__device__ float GenerateOctaveWithBilinear(float2 uv, int coeficient)
{
    return BilinearInterpolation(GenerateNoiseWithResolution(make_int2(uv.x * coeficient, uv.y * coeficient)), GenerateNoiseWithResolution(make_int2(uv.x * coeficient, uv.y * coeficient + 1)),
        GenerateNoiseWithResolution(make_int2(uv.x * coeficient + 1, uv.y * coeficient)), GenerateNoiseWithResolution(make_int2(uv.x * coeficient + 1, uv.y * coeficient + 1)), uv.x * coeficient,
        uv.x * coeficient + 1, uv.y * coeficient, uv.y * coeficient + 1, make_float2(uv.x * coeficient, uv.y * coeficient));
}

__device__ float PerlinNoise(float2 uv, int seed)
{
    float color = 0.f;
    int numberOfCycles = 7;
    for (int i = numberOfCycles; i >= 1; i--)
    {
        color += GenerateOctaveWithBicubic(uv, pow(2, i), seed) / pow(2, i);
    }
    if (color > 1.f)
        color = 1.f;
    return color;
}

__device__ float Corrector(float2 uv)
{
    float corrector = 1.f - ((uv.x - 0.5f) * (uv.x - 0.5f) + (uv.y - 0.5f) * (uv.y - 0.5f));
    if (corrector < 0) corrector = 0.f;
    return corrector * corrector;
}

__global__ void addKernel(float* c, int* height, int* width, int* seed)
{
    int i = threadIdx.x;
    int j = blockIdx.x;
    float corrector = Corrector(make_float2((float)i / height[0], (float)j / width[0]));
    float perlinNoise = PerlinNoise(make_float2((float)i / height[0], (float)j / width[0]), seed[0]);
    c[i * width[0] + j] = pow(perlinNoise, 2);
}

int GenerateAndSavePerlineNoise(int height, int width, int seed)
{
    Height = height;
    Width = width;
    Seed = seed;

    float* c = new float[Height * Width];

    cudaError_t cudaStatus = generatePerlineNoise(c);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "Main function of Cuda failed!");
        return 1;
    }

    SaveArrayInTxt(c);

    cudaStatus = cudaDeviceReset();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceReset failed!");
        return 1;
    }

    return 0;
}

cudaError_t generatePerlineNoise(float* c)
{
    float* dev_c = 0;
    int height[2]; height[0] = Height; height[1] = Height;
    int width[2]; width[0] = Width; width[1] = Width;
    int seed[2]; seed[0] = Seed; seed[1] = Seed;
    int* dev_height;
    int* dev_width;
    int* dev_seed;
    cudaError_t cudaStatus;

    // Choose which GPU to run on, change this on a multi-GPU system.
    cudaStatus = cudaSetDevice(0);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaSetDevice failed!  Do you have a CUDA-capable GPU installed?");
        goto Error;
    }

    cudaStatus = cudaMalloc((void**)&dev_c, Height * Width * sizeof(float));
    cudaMalloc((void**)&dev_height, 2 * sizeof(int));
    cudaMalloc((void**)&dev_width, 2 * sizeof(int));
    cudaMalloc((void**)&dev_seed, 2 * sizeof(int));
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMalloc failed!");
        goto Error;
    }

    cudaMemcpy(dev_height, height, 2 * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(dev_width, width, 2 * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(dev_seed, seed, 2 * sizeof(int), cudaMemcpyHostToDevice);


    addKernel <<< Height, Width >>> (dev_c, dev_height, dev_width, dev_seed);

    // Check for any errors launching the kernel
    cudaStatus = cudaGetLastError();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "addKernel launch failed: %s\n", cudaGetErrorString(cudaStatus));
        goto Error;
    }

    // cudaDeviceSynchronize waits for the kernel to finish, and returns
    // any errors encountered during the launch.
    cudaStatus = cudaDeviceSynchronize();
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaDeviceSynchronize returned error code %d after launching addKernel!\n", cudaStatus);
        goto Error;
    }

    // Copy output vector from GPU buffer to host memory.
    cudaStatus = cudaMemcpy(c, dev_c, Height * Width * sizeof(float), cudaMemcpyDeviceToHost);
    if (cudaStatus != cudaSuccess) {
        fprintf(stderr, "cudaMemcpy failed!");
        goto Error;
    }

Error:
    cudaFree(dev_c);

    return cudaStatus;
}

bool SaveArrayInTxt(float* arrayOfElements)
{
    FILE* file;
    if ((file = fopen("perlineNoise.txt", "w")) == NULL) {
        printf("error\n");
        return false;
    }
    else {
        for (int i = 0; i < Height; i++) {
            for (int j = 0; j < Width; j++)
                fprintf(file, "%f\t", arrayOfElements[i * Width + j]);
            fprintf(file, "\n");
        }
    }
    fclose(file);
    printf("Successful \n");
    return true;
}
