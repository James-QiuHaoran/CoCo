# CoCo - Coordinated Container Scheduling with Last-level Cache and Memory Bandwidth Partitioning

Last-level cache (LLC) and memory bandwidth partitioning are commonly used in existing work to meet QoS requirements of all co-scheduled latency-critical applications consolidated on a physical server.
With the increasing popularity of cloud microservices and Function-as-a-Service paradigm, the number of containers consolidated together increases significantly.
However, due to the limitation of hardware features, existing work fails to support such number of applications.
To bridge this gap, this project proposes CoCo, coordinated container scheduling with LLC and Memory bandwidth partitioning.

## Machine Requirements

CoCo requires the support for Intel Cache Allocation Technology (CAT) and Memory Bandwidth Allocation (MBA).
The following is the support matrix:

+--------------------------------------------------------------------------------------------------------+
|                                                        | CMT | MBM | L3 CAT | L3 CDP | L2 CAT | MBA    |
|--------------------------------------------------------+-----+-----+--------+--------+--------+--------|
|Intel(R) Xeon(R) processor E5 v3                        | Yes | No  | Yes (1)| No     | No     | No     |
|--------------------------------------------------------+-----+-----+--------+--------+--------+--------|
|Intel(R) Xeon(R) processor D                            | Yes | Yes | Yes (2)| No     | No     | No     |
|--------------------------------------------------------+-----+-----+--------+--------+--------+--------|
|Intel(R) Xeon(R) processor E3 v4                        | No  | No  | Yes (3)| No     | No     | No     |
|--------------------------------------------------------+-----+-----+--------+--------+--------+--------|
|Intel(R) Xeon(R) processor E5 v4                        | Yes | Yes | Yes (2)| Yes    | No     | No     |
|--------------------------------------------------------+-----+-----+--------+--------+--------+--------|
|Intel(R) Xeon(R) Scalable Processors (6)                | Yes | Yes | Yes (2)| Yes    | No     | Yes (5)|
|--------------------------------------------------------+-----+-----+--------+--------+--------+--------|
|Intel(R) Xeon(R) 2nd Generation Scalable Processors (7) | Yes | Yes | Yes (2)| Yes    | No     | Yes (5)|
|--------------------------------------------------------+-----+-----+--------+--------+--------+--------|
|Intel(R) Atom(R) processor for Server C3000             | No  | No  | No     | No     | Yes (4)| No     |
+--------------------------------------------------------------------------------------------------------+

Details can be found at: https://github.com/intel/intel-cmt-cat

You can follow the installation instructions on how to set CAT/MBA up on the machine: https://github.com/intel/intel-cmt-cat/blob/master/INSTALL

## Build

```
cd scheduler
make all
```

## How to Run

- Pin consolidated applications running in Docker containers to cores through cgroup `cpuset` subsystem;
- Compute the *slowdown* value for each application using Intel CAT for LLC and MBA for memory bandwidth;
	- Workload setup instructions can be found in `workloads.md`;
	- Intel CAT/MBA configuraiton instructions can be found in `cat-mba-config.md`;
- Update weights in `scheduler/scheduler.c` (`cores_list` and `weights_list`);
- Re-compile the source code by running `make clean; make all`;
- Run the scheduler: `sudo ./scheduler/scheduler`;

## Contact

- Haoran Qiu (haoranq4@illinois.edu)
- Yongzhou Chen (yc28@illinois.edu)
