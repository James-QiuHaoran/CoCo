## Intel CAT/MBA Configuration

### An Example of CAT CLOS Configuration

Initial:

```
L3CA COS definitions for Socket x: 
    L3CA COS0 => MASK 0xfffff
    L3CA COS1 => MASK 0xfffff
    L3CA COS2 => MASK 0xfffff
    L3CA COS3 => MASK 0xfffff
    L3CA COS4 => MASK 0xfffff
    L3CA COS5 => MASK 0xfffff
    L3CA COS6 => MASK 0xfffff
    L3CA COS7 => MASK 0xfffff
    L3CA COS8 => MASK 0xfffff
    L3CA COS9 => MASK 0xfffff
    L3CA COS10 => MASK 0xfffff
    L3CA COS11 => MASK 0xfffff
    L3CA COS12 => MASK 0xfffff
    L3CA COS13 => MASK 0xfffff
    L3CA COS14 => MASK 0xfffff
    L3CA COS15 => MASK 0xfffff
```

Using four CLOSs:

- CLOS0 and CLOS4-15: 2 bits
- CLOS1: 3 bits
- CLOS2: 6 bits
- CLOS3: 9 bits

Commands to configure:

```
sudo ./allocation_app_l3cat 0 0xc0000
sudo ./allocation_app_l3cat 1 0x38000
sudo ./allocation_app_l3cat 2 0x07e00
sudo ./allocation_app_l3cat 3 0x001ff
sudo ./allocation_app_l3cat 4 0xc0000
sudo ./allocation_app_l3cat 5 0xc0000
sudo ./allocation_app_l3cat 6 0xc0000
sudo ./allocation_app_l3cat 7 0xc0000
sudo ./allocation_app_l3cat 8 0xc0000
sudo ./allocation_app_l3cat 9 0xc0000
sudo ./allocation_app_l3cat 10 0xc0000
sudo ./allocation_app_l3cat 11 0xc0000
sudo ./allocation_app_l3cat 12 0xc0000
sudo ./allocation_app_l3cat 13 0xc0000
sudo ./allocation_app_l3cat 14 0xc0000
sudo ./allocation_app_l3cat 15 0xc0000
```

### Core-to-CLOS Mapping

Initially, all cores are associated to `CLOS0`.

Associate cores to CLOSs by:

```
sudo ./association_app clos_num core1 core2 ...
```

### An Example of MBA CLOS Configuration

Initially, 2048% available for Socket 0, 2, 4, 6, 8, 10, 12, 14:

```
MBA COS definitions for Socket 0:
    MBA COS0 => 2048% available
    MBA COS1 => 2048% available
    MBA COS2 => 2048% available
    MBA COS3 => 2048% available
    MBA COS4 => 2048% available
    MBA COS5 => 2048% available
    MBA COS6 => 2048% available
    MBA COS7 => 2048% available
    MBA COS8 => 2048% available
    MBA COS9 => 2048% available
    MBA COS10 => 2048% available
    MBA COS11 => 2048% available
    MBA COS12 => 2048% available
    MBA COS13 => 2048% available
    MBA COS14 => 2048% available
    MBA COS15 => 2048% available
```

Add configurations:

```
sudo ./allocation_app_mba 1 80
sudo ./allocation_app_mba 2 60
sudo ./allocation_app_mba 3 40
sudo ./allocation_app_mba 4 20
```

```
MBA COS definitions for Socket 0:
    MBA COS0 => 2048% available
    MBA COS1 => 80% available
    MBA COS2 => 60% available
    MBA COS3 => 40% available
    MBA COS4 => 20% available
    MBA COS5 => 1% available
```
