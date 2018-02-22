speedups (higher is better) wrt `std::vector`.


## when the vector will be filled with exactly the number of (compile time) expected elements

![simple_exactfill.png](simple_exactfill.png)

## when the vector will be filled with less than the number of (compile time) expected elements

![simple_overfill.png](simple_underfill.png)

## when the vector will be filled with more than the number of (compile time) expected elements

In this case, the `boost::container::static_vector` cannot be used.

![simple_underfill.png](simple_overfill.png)
