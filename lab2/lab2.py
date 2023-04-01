import argparse
import m5
from m5.objects import *
from cache import *

# define parameters
parser = argparse.ArgumentParser()
parser.add_argument("-c")
parser.add_argument("--cpu_type")
parser.add_argument("--cpu_clock")
parser.add_argument("--l2_size")
parser.add_argument("--issue_width")

args = parser.parse_args()

# cache class
class L1Cache(Cache):
    assoc = 2
    tag_latency = 2
    data_latency = 2
    response_latency = 2
    mshrs = 4
    tgts_per_mshr = 20

class L1ICache(L1Cache):
    size = '64kB'

class L1DCache(L1Cache):
    size = '64kB'

# init system
system = System()

# clk_domain
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = args.cpu_clock
system.clk_domain.voltage_domain = VoltageDomain()

# mem mode
system.mem_mode = 'timing'              
system.mem_ranges = [AddrRange('1GB')] 

# CPU
assert(args.cpu_type == 'DerivO3CPU' or args.cpu_type == 'MinorCPU')
if args.cpu_type == 'DerivO3CPU':
    system.cpu = DerivO3CPU()
    system.cpu.issueWidth = args.issue_width
elif args.cpu_type == 'MinorCPU':
    system.cpu = MinorCPU()

if args.l2_size != '0kB':
    class L2Cache(Cache):
        size = args.l2_size
        assoc = 8
        tag_latency = 20
        data_latency = 20
        response_latency = 20
        mshrs = 20
        tgts_per_mshr = 12

    # crossbar
    system.membus = SystemXBar()
    system.l2bus = L2XBar()

    # cache
    system.cpu.icache = L1ICache()
    system.cpu.dcache = L1DCache()
    system.L2cache = L2Cache()

    # connect cpu to membus
    system.cpu.icache_port = system.cpu.icache.cpu_side
    system.cpu.dcache_port = system.cpu.dcache.cpu_side
    system.cpu.icache.mem_side = system.l2bus.cpu_side_ports
    system.cpu.dcache.mem_side = system.l2bus.cpu_side_ports
    system.l2bus.mem_side_ports = system.L2cache.cpu_side
    system.L2cache.mem_side = system.membus.cpu_side_ports

else:
    # crossbar
    system.membus = SystemXBar()

    # cache
    system.cpu.icache = L1ICache()
    system.cpu.dcache = L1DCache()

    # connect cpu to membus
    system.cpu.icache_port = system.cpu.icache.cpu_side
    system.cpu.dcache_port = system.cpu.dcache.cpu_side
    system.cpu.icache.mem_side = system.membus.cpu_side_ports
    system.cpu.dcache.mem_side = system.membus.cpu_side_ports

# interrupt controller
system.cpu.createInterruptController()
if m5.defines.buildEnv['TARGET_ISA'] == "x86":
    system.cpu.interrupts[0].pio = system.membus.mem_side_ports
    system.cpu.interrupts[0].int_requestor = system.membus.cpu_side_ports
    system.cpu.interrupts[0].int_responder = system.membus.mem_side_ports

# mem_ctrl
system.mem_ctrl = MemCtrl()
system.mem_ctrl.dram = DDR3_1600_8x8()
system.mem_ctrl.dram.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.mem_side_ports

# system port
system.system_port = system.membus.cpu_side_ports


# workload definition
binary = args.c
system.workload = SEWorkload.init_compatible(binary)
process = Process()
process.cmd = [binary]
system.cpu.workload = process
system.cpu.createThreads()

# root of system
root = Root(full_system = False, system = system)
m5.instantiate()

print("Beginning simulation!")
exit_event = m5.simulate()
print('Exiting @ tick %i because %s' % (m5.curTick(), exit_event.getCause()))

