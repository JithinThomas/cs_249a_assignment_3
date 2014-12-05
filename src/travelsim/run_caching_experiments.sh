
# Dense network with network modification activity enabled
time ./client-auto-network-sim 200 15000 200 1 21478797 360 1 > dense_nm_1_cache_1.txt
time ./client-auto-network-sim 200 15000 200 1 21478797 360 0 > dense_nm_1_cache_0.txt

# Dense network with network modification activity disabled
time ./client-auto-network-sim 200 15000 200 0 77897242 360 1 > dense_nm_0_cache_1.txt
time ./client-auto-network-sim 200 15000 200 0 77897242 360 0 > dense_nm_0_cache_0.txt

# Sparse network with network modification activity enabled 
time ./client-auto-network-sim 200 700 200 1 54317770 360 1 > sparse_nm_1_cache_1.txt
time ./client-auto-network-sim 200 700 200 1 54317770 360 0 > sparse_nm_1_cache_0.txt

# Sparse network with network modification activity enabled 
time ./client-auto-network-sim 200 700 200 0 10295624 360 1 > sparse_nm_0_cache_1.txt
time ./client-auto-network-sim 200 700 200 0 10295624 360 0 > sparse_nm_0_cache_0.txt

