## Modifications

This is a C++17 port of [progschj/ThreadPool](https://github.com/progschj/ThreadPool)
by Jakob Progsch and Václav Zeman (zlib License).

Changes from the original:

- **Return type deduction**: replaced `std::result_of<F(Args...)>` with
  `std::invoke_result_t<F, Args...>` (`std::result_of` was deprecated in C++17 and removed in C++20).
- **Task packaging**: replaced `std::bind` + `std::packaged_task` with
  `std::apply` + `std::tuple` + lambda, enabling perfect forwarding and
  support for move-only argument types.
- **API**: renamed `enqueue` to `Enqueue`.
- **Error handling**: `Enqueue` on a stopped pool now returns an empty
  `std::future` instead of throwing `std::runtime_error`.
- **Build**: compiles with `-std=c++17`.

The original license text is preserved in the [COPYING](COPYING) file.
This modified version is distributed under the same zlib License.
