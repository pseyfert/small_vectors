speedups (higher is better) wrt `std::vector`.


## when the vector will be filled with exactly the number of (compile time) expected elements

![complex_exactfill.png](complex_exactfill.png)

## when the vector will be filled with less than the number of (compile time) expected elements

![complex_overfill.png](complex_underfill.png)

## when the vector will be filled with more than the number of (compile time) expected elements

In this case, the `boost::container::static_vector` cannot be used.

![complex_underfill.png](complex_overfill.png)
