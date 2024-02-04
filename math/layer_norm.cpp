#include <experimental/simd>
#include <thread>
#include <vector>
#include <iostream>

template <typename T>
void layer_norm(const size_t num_threads,
                const T *x, const size_t batch_size, const size_t column_size,
                const T *gamma, const T *beta, const T espsilon,
                T *y)
{
    using simd_type = std::experimental::native_simd<T>;

    std::vector<T> x_buffer(batch_size * num_threads);
    std::vector<T> x2_buffer(batch_size * num_threads);

    const size_t n_elems_per_thread = column_size / num_threads;
    auto reduce_fn = [&](int index)
    {
        for (int i = 0; i < batch_size; ++i)
        {
            /*
            simd_type simd_x_accu = 0;
            simd_type simd_x2_accu = 0;
            size_t simd_size = simd_x_accu.size();
            int start_j = index * n_elems_per_thread;
            int end_j = (index + 1) * n_elems_per_thread;
            for (int j = start_j; j < end_j; j += simd_size)
            {
                const T *addr = &x[i * column_size + j];
                simd_type simd_temp(addr,
                                    std::experimental::vector_aligned);
                simd_x_accu = simd_x_accu + simd_temp;
                simd_x2_accu = simd_x2_accu + simd_temp * simd_temp;
            }
            T reduced_x_sum = std::experimental::reduce(simd_x_accu);
            T reduced_x2_sum = std::experimental::reduce(simd_x2_accu);
            x_buffer[i * num_threads + index] = reduced_x_sum;
            x2_buffer[i * num_threads + index] = reduced_x2_sum;
            */
            T x_accu = 0;
            T x2_accu = 0;
            int start_j = index * n_elems_per_thread;
            int end_j = (index + 1) * n_elems_per_thread;
            for (int j = start_j; j < end_j; ++j)
            {
                T elem = x[i * column_size + j];
                x_accu += elem;
                x2_accu += elem;
            }
            x_buffer[i * num_threads + index] = x_accu;
            x2_buffer[i * num_threads + index] = x2_accu;
        }
    };

    std::vector<std::thread> thread_pool(num_threads);
    for (int i = 0; i < num_threads; ++i)
    {
        thread_pool[i] = std::thread(reduce_fn, i);
    }
    for (int i = 0; i < num_threads; ++i)
    {
        thread_pool[i].join();
    }

    // mean = x_sum / N
    // variance = x2_sum / N - (mean^2)
    std::vector<T> mean(batch_size);
    std::vector<T> std(batch_size);

    for (int i = 0; i < batch_size; ++i)
    {
        T reduced_x_sum = 0.0;
        T reduced_x2_sum = 0.0;
        for (int j = 0; j < num_threads; ++j)
        {
            reduced_x_sum += x_buffer[i * num_threads + j];
            reduced_x2_sum += x2_buffer[i * num_threads + j];
        }
        mean[i] = reduced_x_sum / static_cast<T>(column_size);
        std[i] = std::sqrt(reduced_x2_sum / static_cast<T>(column_size) - mean[i] * mean[i] + espsilon);
    }

    auto map_fn = [&](int index)
    {
        for (int i = 0; i < batch_size; ++i)
        {
            int start_j = index * n_elems_per_thread;
            int end_j = (index + 1) * n_elems_per_thread;
            for (int j = start_j; j < end_j; ++j)
            {
                y[i * column_size + j] = ((x[i * column_size + j] - mean[i]) / std[i]) * gamma[i] + beta[i];
            }
        }
    };

    for (int i = 0; i < num_threads; ++i)
    {
        thread_pool[i] = std::thread(map_fn, i);
    }
    for (int i = 0; i < num_threads; ++i)
    {
        thread_pool[i].join();
    }
}

void layer_norm_test()
{
    int batch_size = 2048;
    int column_size = 2048;
    float epsilon = 1e-6;
    std::vector<float>
        x(batch_size * column_size, 1.0);
    std::vector<float> gamma(batch_size, 1.0);
    std::vector<float> beta(batch_size, 0.0);
    std::vector<float> y(batch_size * column_size, 0.0);

    for (int num_threads : {1, 2, 4, 8, 16, 32})
    {
        auto start = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 10; ++i)
        {
            layer_norm<float>(num_threads, x.data(), batch_size, column_size,
                              gamma.data(), beta.data(), epsilon, y.data());
        }
        auto stop = std::chrono::high_resolution_clock::now();
        std::cout << "Threads: " << num_threads << "\n";
        std::cout
            << "Duration: "
            << std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count() / 10
            << " microseconds\n";
    }
}

int main()
{
    layer_norm_test();
}