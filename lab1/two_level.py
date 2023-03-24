import m5
from m5.objects import *
from cache import *

# init system
system = System()

# clk_domain
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = '1GHz'
system.clk_domain.voltage_domain = VoltageDomain()

# mem mode
system.mem_mode = 'timing'              
system.mem_ranges = [AddrRange('512MB')] 

# CPU
system.cpu = TimingSimpleCPU()

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
binary = 'tests/test-progs/hello/bin/x86/linux/hello'
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

