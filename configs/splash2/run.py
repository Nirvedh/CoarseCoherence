import m5
from m5.objects import *
import os, optparse, sys
from m5.util import addToPath, fatal

addToPath('../common')

# --------------------
# Define Command Line Options
# ====================

parser = optparse.OptionParser()

parser.add_option("-d", "--detailed", action="store_true")
parser.add_option("-t", "--timing", action="store_true")
parser.add_option("-m", "--maxtick", type="int")
parser.add_option("-n", "--numcpus",
                  help="Number of cpus in total", type="int")
parser.add_option("-f", "--frequency",
                  default = "1GHz",
                 help="Frequency of each CPU")
#parser.add_option("-p", "--protocol",
#                  default="moesi",
#                  help="The coherence protocol to use for the L1'a (i.e. MOESI, MOSI)")
parser.add_option("--l1size",
                  default = "32kB")
#parser.add_option("--l1latency",
#                  default = 1)
parser.add_option("--l2size",
                  default = "256kB")
#parser.add_option("--l2latency",
#                  default = 10)
parser.add_option("--rootdir",
                  help="Root directory of Splash2",
                  default="/afs/cs.wisc.edu/u/n/i/nirvedh/private/ece757/benchmarks/parsec-x86/parsec-3.0/ext/splash2")
parser.add_option("-b", "--benchmark",
                  help="Splash 2 benchmark to run")

(options, args) = parser.parse_args()

if args:
    print "Error: script doesn't take any positional arguments"
    sys.exit(1)

if not options.numcpus:
    print "Specify the number of cpus with -n"
    sys.exit(1)
    
# --------------------
# Define Splash2 Benchmarks
# ====================
class Cholesky(LiveProcess):
    cwd = options.rootdir + '/kernels/cholesky'
    executable = options.rootdir + '/kernels/cholesky/CHOLESKY'
    cmd = 'CHOLESKY -p' + str(options.numcpus) + ' '\
          + options.rootdir + '/kernels/cholesky/inputs/tk23.O'

class FFT(LiveProcess):
    cwd = options.rootdir + '/kernels/fft'
    executable = options.rootdir + '/kernels/fft/FFT'
    cmd = 'FFT -p' + str(options.numcpus) + ' -m18'

class LU_contig(LiveProcess):
    executable = options.rootdir + '/kernels/lu/contiguous_blocks/LU'
    cmd = 'LU -p' + str(options.numcpus)
    cwd = options.rootdir + '/kernels/lu/contiguous_blocks'

class LU_noncontig(LiveProcess):
    executable = options.rootdir + '/kernels/lu/non_contiguous_blocks/LU'
    cmd = 'LU -p' + str(options.numcpus)
    cwd = options.rootdir + '/kernels/lu/non_contiguous_blocks'

class Radix(LiveProcess):
    executable = options.rootdir + '/kernels/radix/RADIX'
    cmd = 'RADIX -n524288 -p' + str(options.numcpus)
    cwd = options.rootdir + '/kernels/radix'

class Barnes(LiveProcess):
    executable = options.rootdir + '/apps/barnes/BARNES'
    cmd = 'BARNES'
    input = options.rootdir + '/apps/barnes/input.p' + str(options.numcpus)
    cwd = options.rootdir + '/apps/barnes'

class FMM(LiveProcess):
    executable = options.rootdir + '/apps/fmm/FMM'
    cmd = 'FMM'
    if str(options.numcpus) == '1':
        input = options.rootdir + '/apps/fmm/inputs/input.2048'
    else:
        input = options.rootdir + '/apps/fmm/inputs/input.2048.p' + str(options.numcpus)
    cwd = options.rootdir + '/apps/fmm'

class Ocean_contig(LiveProcess):
    executable = options.rootdir + '/apps/ocean/contiguous_partitions/OCEAN'
    cmd = 'OCEAN -p' + str(options.numcpus)
    cwd = options.rootdir + '/apps/ocean/contiguous_partitions'

class Ocean_noncontig(LiveProcess):
    executable = options.rootdir + '/apps/ocean/non_contiguous_partitions/OCEAN'
    cmd = 'OCEAN -p' + str(options.numcpus)
    cwd = options.rootdir + '/apps/ocean/non_contiguous_partitions'

class Raytrace(LiveProcess):
    executable = options.rootdir + '/apps/raytrace/RAYTRACE'
    cmd = 'RAYTRACE -p' + str(options.numcpus) + ' ' \
          + options.rootdir + '/apps/raytrace/inputs/teapot.env'
    cwd = options.rootdir + '/apps/raytrace'

class Water_nsquared(LiveProcess):
    executable = options.rootdir + '/apps/water-nsquared/WATER-NSQUARED'
    cmd = 'WATER-NSQUARED'
    if options.numcpus==1:
        input = options.rootdir + '/apps/water-nsquared/input'
    else:
        input = options.rootdir + '/apps/water-nsquared/input.p' + str(options.numcpus)
    cwd = options.rootdir + '/apps/water-nsquared'
        
class Water_spatial(LiveProcess):
    executable = options.rootdir + '/apps/water-spatial/WATER-SPATIAL'
    cmd = 'WATER-SPATIAL'
    if options.numcpus==1:
        input = options.rootdir + '/apps/water-spatial/input'
    else:
        input = options.rootdir + '/apps/water-spatial/input.p' + str(options.numcpus)
    cwd = options.rootdir + '/apps/water-spatial'
        
# --------------------
# Base L1 Cache Definition
# ====================

class L1(BaseCache):
#    latency = options.l1latency
#    block_size = 64
    mshrs = 12
    tgts_per_mshr = 8
    #protocol = CoherenceProtocol(protocol=options.protocol)

# ----------------------
# Base L2 Cache Definition
# ----------------------

class L2(BaseCache):
#    block_size = 64
#    latency = options.l2latency
    mshrs = 92
    tgts_per_mshr = 16
    write_buffers = 8

# ----------------------
# Define the cpus
# ----------------------

busFrequency = Frequency(options.frequency)

if options.timing:
    cpus = [TimingSimpleCPU(cpu_id = i)
            for i in xrange(options.numcpus)]
elif options.detailed:
    cpus = [DerivO3CPU(cpu_id = i)
            for i in xrange(options.numcpus)]
else:
    cpus = [AtomicSimpleCPU(cpu_id = i)
            for i in xrange(options.numcpus)]

# ----------------------
# Create a system, and add system wide objects
# ----------------------        
system = System(cpu = cpus, physmem = PhysicalMemory(),
                membus = Bus(clock = busFrequency))

system.toL2bus = Bus(clock = busFrequency)
system.l2 = L2(size = options.l2size, assoc = 8)

# ----------------------
# Connect the L2 cache and memory together
# ----------------------

system.physmem.port = system.membus.port
system.l2.cpu_side = system.toL2bus.port
system.l2.mem_side = system.membus.port

# ----------------------
# Connect the L2 cache and clusters together
# ----------------------
for cpu in cpus:
    cpu.addPrivateSplitL1Caches(L1(size = options.l1size, assoc = 1),
                                L1(size = options.l1size, assoc = 4))
    cpu.mem = cpu.dcache
    # connect cpu level-1 caches to shared level-2 cache
    cpu.connectMemPorts(system.toL2bus)


# ----------------------
# Define the root
# ----------------------

root = Root(system = system)

# --------------------
# Pick the correct Splash2 Benchmarks
# ====================
if options.benchmark == 'Cholesky':
    root.workload = Cholesky()
elif options.benchmark == 'FFT':
    root.workload = FFT()
elif options.benchmark == 'LUContig':
    root.workload = LU_contig()
elif options.benchmark == 'LUNoncontig':
    root.workload = LU_noncontig()
elif options.benchmark == 'Radix':
    root.workload = Radix()
elif options.benchmark == 'Barnes':
    root.workload = Barnes()
elif options.benchmark == 'FMM':
    root.workload = FMM()
elif options.benchmark == 'OceanContig':
    root.workload = Ocean_contig()
elif options.benchmark == 'OceanNoncontig':
    root.workload = Ocean_noncontig()
elif options.benchmark == 'Raytrace':
    root.workload = Raytrace()
elif options.benchmark == 'WaterNSquared':
    root.workload = Water_nsquared()
elif options.benchmark == 'WaterSpatial':
    root.workload = Water_spatial()
else:
    panic("The --benchmark environment variable was set to something" \
          +" improper.\nUse Cholesky, FFT, LUContig, LUNoncontig, Radix" \
          +", Barnes, FMM, OceanContig,\nOceanNoncontig, Raytrace," \
          +" WaterNSquared, or WaterSpatial\n")

# --------------------
# Assign the workload to the cpus
# ====================

for cpu in cpus:
    cpu.workload = root.workload

# ----------------------
# Run the simulation
# ----------------------

if options.timing or options.detailed:
    root.system.mem_mode = 'timing'

# instantiate configuration
m5.instantiate(root)

# simulate until program terminates
if options.maxtick:
    exit_event = m5.simulate(options.maxtick)
else:
    exit_event = m5.simulate(m5.MaxTick)

print 'Exiting  <at>  tick', m5.curTick(), 'because', exit_event.getCause()
