# dynmaic_binary_row
A brand new data structure performs at least 40% faster than a skiplist in terms of insertion, deletion, and lookup
# To validate the result:
[root@localhost AIID_POWER]# ls -ltr
total 140
-rw-r--r--. 1 root root   418 Apr 24 05:25 local_history.patch
-rw-r--r--. 1 root root   607 Apr 24 05:25 local_history1.patch
-rw-r--r--. 1 root root   620 Apr 28 03:27 2024042801.patch
-rw-r--r--. 1 root root 14167 Apr 29 02:45 CMakeCache.txt
-rw-r--r--. 1 root root  1648 Apr 29 02:45 cmake_install.cmake
drwxr-xr-x. 2 root root   182 Apr 30 03:16 base_struct
-rw-r--r--. 1 root root   519 Apr 30 04:12 base.h
-rw-r--r--. 1 root root   520 Apr 30 05:38 CMakeLists.txt
-rw-r--r--. 1 root root  9429 May  6 23:59 Makefile
-rwxr-xr-x. 1 root root 37392 May  6 23:59 phaeton_binary_list
drwxr-xr-x. 5 root root   152 May  7 05:31 cmake-build-debug
-rw-r--r--. 1 root root  3560 May  7 05:47 main.cpp
-rwxr-xr-x. 1 root root 37096 May  7 05:48 phaeton_skip_list
drwxr-xr-x. 5 root root  4096 May  7 05:48 CMakeFiles
[root@localhost AIID_POWER]# ./phaeton_binary_list
BinaryList Insert 7000000 Nums and Search&Delete 1910000 Nums cost:20536
[root@localhost AIID_POWER]# ./phaeton_skip_list
SkipList Insert 7000000 Nums and Search&Delete 1910000 Nums cost:30148

