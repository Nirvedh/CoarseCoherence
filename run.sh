build/X86/gem5.opt --debug-flags=RubySlicc --debug-file=Ruby_rctest1.out configs/example/se.py -c ../programs/eg_pthread -o 3 --ruby --topology Crossbar -n 4 --l1i_size=32kB --l1d_size=32kB --l2_size=512MB --num-l2caches=1
#build/X86/gem5.opt configs/example/se.py -c ../programs/eg_pthread -o 3 --ruby --topology Crossbar -n 4 --l1i_size=32kB --l1d_size=i32kB --l2_size=512MB --num-l2caches=1 
#build/X86/gem5.opt configs/example/ruby_random_test.py --ruby --topology Crossbar -n 4 --l1i_size=32kB --l1d_size=32kB --l2_size=512MB --num-l2caches=1
