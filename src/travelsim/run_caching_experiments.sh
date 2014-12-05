
# Manual network simulation with constant distribution for trip interval and count and network modifications enabled
client-manual-network-sim 1418385 360 1 1 1 > manual_1_1_1.txt

# Manual network simulation with uniform or normal distribution for trip interval and count and network modifications enabled
client-manual-network-sim 1418385 360 1 0 1 > manual_1_0_1.txt

# Manual network simulation with constant distribution for trip interval and count and network modifications disabled
client-manual-network-sim 1418385 360 0 1 1 > manual_0_1_1.txt

# Manual network simulation with uniform or normal distribution for trip interval and count and network modifications disabled
client-manual-network-sim 1418385 360 0 0 1 > manual_0_0_1.txt

# Very dense network with network modification activity enabled
time ./client-auto-network-sim 200 30000 200 1 23901846 360 1 > very_dense_nm_1_cache_1.txt
time ./client-auto-network-sim 200 30000 200 1 23901846 360 0 > very_dense_nm_1_cache_0.txt

# Dense network with network modification activity enabled
time ./client-auto-network-sim 200 15000 200 1 21478797 360 1 > dense_nm_1_cache_1.txt
time ./client-auto-network-sim 200 15000 200 1 21478797 360 0 > dense_nm_1_cache_0.txt

# Dense network with network modification activity disabled
time ./client-auto-network-sim 200 15000 200 0 77897242 360 1 > dense_nm_0_cache_1.txt
time ./client-auto-network-sim 200 15000 200 0 77897242 360 0 > dense_nm_0_cache_0.txt

# Sparse network with network modification activity enabled 
time ./client-auto-network-sim 200 700 200 0 10295624 360 1 > sparse_nm_0_cache_1.txt
time ./client-auto-network-sim 200 700 200 0 10295624 360 0 > sparse_nm_0_cache_0.txt

