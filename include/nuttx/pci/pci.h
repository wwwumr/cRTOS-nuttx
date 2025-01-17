/****************************************************************************
 * include/nuttx/pci/pci.h
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 ****************************************************************************/

#ifndef __INCLUDE_NUTTX_PCI_PCI_H
#define __INCLUDE_NUTTX_PCI_PCI_H

#ifdef CONFIG_PCI

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>

#include <sys/types.h>
#include <stdint.h>

#include <nuttx/fs/ioctl.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#define PCI_CFG_VENDOR_ID       0x000
#define PCI_CFG_DEVICE_ID       0x002
#define PCI_CFG_COMMAND         0x004
# define PCI_CMD_IO             (1 << 0)
# define PCI_CMD_MEM            (1 << 1)
# define PCI_CMD_MASTER         (1 << 2)
# define PCI_CMD_INTX_OFF       (1 << 10)
#define PCI_CFG_STATUS          0x006
# define PCI_STS_INT            (1 << 3)
# define PCI_STS_CAPS           (1 << 4)
#define PCI_CFG_REVERSION       0x008
#define PCI_CFG_BAR             0x010
# define PCI_BAR_IO             0x1
# define PCI_BAR_1M             0x2
# define PCI_BAR_64BIT          0x4
#define PCI_CFG_CAP_PTR         0x034

#define PCI_ID_ANY              0xffff
#define PCI_DEV_CLASS_OTHER     0xff

#define PCI_CAP_PM              0x01

#define PCI_CAP_MSI             0x05
# define PCI_MSI_MCR            0x02
# define PCI_MSI_MCR_SIZE       2
# define PCI_MSI_MCR_EN         (1 << 0)
# define PCI_MSI_MCR_64         (1 << 7)
# define PCI_MSI_MAR            0x04
# define PCI_MSI_MAR_SIZE       4
# define PCI_MSI_MDR            0x08
# define PCI_MSI_MDR_SIZE       2
# define PCI_MSI_MAR64_HI       0x08
# define PCI_MSI_MAR64_HI_SIZE  4
# define PCI_MSI_MDR64          0x0c
# define PCI_MSI_MDR64_SIZE     2
# define PCI_MSI_APIC_ID_OFFSET 0xc

#define PCI_CAP_VNDR            0x09

#define PCI_CAP_MSIX            0x11
# define PCI_MSIX_MCR           0x02
# define PCI_MSIX_MCR_SIZE      2
# define PCI_MSIX_MCR_EN        (1 << 15)
# define PCI_MSIX_MCR_FMASK     0x4000
# define PCI_MSIX_MCR_TBL_MASK  0x03ff
# define PCI_MSIX_TBL           0x04
# define PCI_MSIX_TBL_SIZE      4
# define PCI_MSIX_PBA           0x08
# define PCI_MSIX_PBA_SIZE      4
# define PCI_MSIX_BIR_MASK      0x07
# define PCI_MSIX_TBL_ENTRY_SIZE 0x10
# define PCI_MSIX_TBL_LO_ADDR   0x0
# define PCI_MSIX_TBL_HI_ADDR   0x4
# define PCI_MSIX_TBL_MSG_DATA  0x8
# define PCI_MSIX_TBL_VEC_CTL   0xc
# define PCI_MSIX_APIC_ID_OFFSET 0xc

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* The PCI driver interface */

struct pci_bus_s;
struct pci_dev_type_s;
struct pci_dev_s;

/* Bus related operations */

struct pci_bus_ops_s
{
    CODE int (*pci_enumerate)(FAR struct pci_bus_s *bus,
                              FAR struct pci_dev_type_s **types);

    CODE int (*pci_cfg_write)(FAR struct pci_dev_s *dev, uintptr_t addr,
                              FAR uint32_t val, unsigned int size);

    CODE uint32_t (*pci_cfg_read)(FAR struct pci_dev_s *dev, uintptr_t addr,
                                  unsigned int size);

    CODE void* (*pci_map_mem)(FAR struct pci_dev_s *dev, uintptr_t addr,
                                  unsigned long length);

    CODE int (*pci_msi_register)(FAR struct pci_dev_s *dev,
                                 uint16_t vector);

    CODE int (*pci_msix_register)(FAR struct pci_dev_s *dev,
                                  uint32_t vector, uint32_t index);
};

/* PCI bus private data. */

struct pci_bus_s
{
  FAR const struct pci_bus_ops_s *ops; /* operations */
};

/* PCI device type, defines by vendor ID and device ID */

struct pci_dev_type_s
{
  uint16_t      vendor;            /* Device vendor ID */
  uint16_t      device;            /* Device ID */
  uint32_t      class_rev;         /* Device reversion */
  const char    *name;             /* Human readable name */

  /* Call back function when a device is probed */

  CODE int (*probe)(FAR struct pci_bus_s *bus,
                    FAR struct pci_dev_type_s *type, uint16_t bdf);
};

/* PCI device private data. */

struct pci_dev_s
{
    FAR struct pci_bus_s       *bus;
    FAR struct pci_dev_type_s  *type;
    uint16_t                    bdf;
};

/****************************************************************************
 * Public Functions Prototypes
 ****************************************************************************/

#undef EXTERN
#if defined(__cplusplus)
#define EXTERN extern "C"
extern "C"
{
#else
#define EXTERN extern
#endif

/****************************************************************************
 * Name: pci_initialize
 *
 * Description:
 *  Initialize the PCI-E bus and enumerate the devices with give devices
 *  type array
 *
 * Input Parameters:
 *   bus    - An PCI bus
 *   types  - A array of PCI device types
 *   num    - Number of device types
 *
 * Returned Value:
 *   OK if the driver was successfully register; A negated errno value is
 *   returned on any failure.
 *
 ****************************************************************************/

int pci_initialize(FAR struct pci_bus_s *bus);

/****************************************************************************
 * Name: pci_enable_device
 *
 * Description:
 *  Enable device with MMIO
 *
 * Input Parameters:
 *   dev - device
 *
 * Return value:
 *   -EINVAL: error
 *   OK: OK
 *
 ****************************************************************************/

int pci_enable_device(FAR struct pci_dev_s *dev);

/****************************************************************************
 * Name: pci_find_cap
 *
 * Description:
 *  Search through the PCI-e device capability list to find given capability.
 *
 * Input Parameters:
 *   dev - Device
 *   cap - Bitmask of capability
 *
 * Returned Value:
 *   -1: Capability not supported
 *   other: the offset in PCI configuration space to the capability structure
 *
 ****************************************************************************/

int pci_find_cap(FAR struct pci_dev_s *dev, uint16_t cap);

/****************************************************************************
 * Name: pci_map_bar
 *
 * Description:
 *  Map address in a 32 bits bar in the flat memory address space
 *
 * Input Parameters:
 *   dev    - Device private data
 *   bar    - Bar number
 *   length - Map length, multiple of PAGE_SIZE
 *
 * Returned Value:
 *   NULL: error, Otherwise: Mapped address
 *
 ****************************************************************************/

void* pci_map_bar(FAR struct pci_dev_s *dev, uint32_t bar);

/****************************************************************************
 * Name: pci_map_bar64
 *
 * Description:
 *  Map address in a 64 bits bar in the flat memory address space
 *
 * Input Parameters:
 *   dev    - Device private data
 *   bar    - Bar number
 *   length - Map length, multiple of PAGE_SIZE
 *
 * Returned Value:
 *   NULL: error, Otherwise: Mapped address
 *
 ****************************************************************************/

void* pci_map_bar64(FAR struct pci_dev_s *dev, uint32_t bar);

/****************************************************************************
 * Name: pci_ioremap
 *
 * Description:
 *  Map PCI address region in the flat memory address space
 *
 * Input Parameters:
 *   dev       - Device private data
 *   from_addr - Address to map
 *   length    - Map length
 *
 * Returned Value:
 *   NULL: error, Otherwise: Mapped address
 *
 ****************************************************************************/

void* pci_ioremap(FAR struct pci_dev_s *dev,
                  uintptr_t from_addr, unsigned long length);

/****************************************************************************
 * Name: pci_get_bar
 *
 * Description:
 *  Get a 32 bits bar
 *
 * Input Parameters:
 *   dev    - Device private data
 *   bar    - Bar number
 *   ret    - Bar Content
 *
 * Returned Value:
 *    Content of the bar
 *
 ****************************************************************************/

uint32_t pci_get_bar(FAR struct pci_dev_s *dev, uint32_t bar);

/****************************************************************************
 * Name: pci_get_bar64
 *
 * Description:
 *  Get a 64 bits bar
 *
 * Input Parameters:
 *   dev    - Device private data
 *   bar    - Bar number
 *   ret    - Bar Content
 *
 * Returned Value:
 *    Content of the bar
 *
 ****************************************************************************/

uint64_t pci_get_bar64(FAR struct pci_dev_s *dev, uint32_t bar);

/****************************************************************************
 * Name: pci_set_bar
 *
 * Description:
 *  Set a 32 bits bar
 *
 * Input Parameters:
 *   dev    - Device private data
 *   bar    - Bar number
 *   val    - Bar Content
 *
 * Returned Value:
 *   0: success, <0: A negated errno
 *
 ****************************************************************************/

int pci_set_bar(FAR struct pci_dev_s *dev, uint32_t bar,
                uint32_t val);

/****************************************************************************
 * Name: pci_set_bar64
 *
 * Description:
 *  Set a 64 bits bar
 *
 * Input Parameters:
 *   dev    - Device private data
 *   bar    - Bar number
 *   val    - Bar Content
 *
 * Returned Value:
 *   0: success, <0: A negated errno
 *
 ****************************************************************************/

int pci_set_bar64(FAR struct pci_dev_s *dev, uint32_t bar,
                  uint64_t val);

/****************************************************************************
 * Name: pci_get_bar_size
 *
 * Description:
 *  Get a 32 bits bar size
 *
 * Input Parameters:
 *   dev    - Device private data
 *   bar    - Bar number
 *
 * Returned Value:
 *    Content of the bar
 *
 ****************************************************************************/

uint32_t pci_get_bar_size(FAR struct pci_dev_s *dev, uint32_t bar);

/****************************************************************************
 * Name: pci_get_bar64_size
 *
 * Description:
 *  Get a 64 bits bar size
 *
 * Input Parameters:
 *   dev    - Device private data
 *   bar    - Bar number
 *
 * Returned Value:
 *    Content of the bar
 *
 ****************************************************************************/

uint64_t pci_get_bar64_size(FAR struct pci_dev_s *dev, uint32_t bar);

/****************************************************************************
 * Name: pci_cfg_write
 *
 * Description:
 *  Write 8, 16, 32 bits data to PCI configuration space of device
 *  specified by dev
 *
 * Input Parameters:
 *   bdf    - Device private data
 *   val    - Value to be written
 *   size   - The number of bytes to be written
 *
 * Returned Value:
 *   0: success, <0: A negated errno
 *
 ****************************************************************************/

static inline int pci_cfg_write(FAR struct pci_dev_s *dev, uintptr_t addr,
                                uint32_t value, unsigned int size)
{
  DEBUGASSERT(size == 1 || size == 2 || size == 4);

  return dev->bus->ops->pci_cfg_write(dev, addr, value, size);
}

/****************************************************************************
 * Name: pci_cfg_read
 *
 * Description:
 *  Read 8, 16, 32 bits data from PCI configuration space of device
 *  specified by dev
 *
 * Input Parameters:
 *   dev    - Device private data
 *   size   - The requested number of bytes to be read
 *
 * Returned Value:
 *    Value in configuration space
 *
 ****************************************************************************/

static inline uint32_t pci_cfg_read(FAR struct pci_dev_s *dev, uintptr_t addr,
                                    unsigned int size)
{
  DEBUGASSERT(size == 1 || size == 2 || size == 4);

  return dev->bus->ops->pci_cfg_read(dev, addr, size);
}

/****************************************************************************
 * Name: pci_msix_register
 *
 * Description:
 *  Map a device MSI-X vector to a platform IRQ vector
 *
 * Input Parameters:
 *   dev - Device
 *   vector - IRQ number of the platform
 *   index  - Device MSI-X vector number
 *
 * Returned Value:
 *   <0: Mapping failed
 *    0: Mapping succeed
 *
 ****************************************************************************/

static inline int pci_msix_register(FAR struct pci_dev_s *dev,
                                    uint32_t vector, uint32_t index)
{
  return dev->bus->ops->pci_msix_register(dev, vector, index);
}

/****************************************************************************
 * Name: pci_msi_register
 *
 * Description:
 *  Map device MSI vectors to a platform IRQ vector
 *
 * Input Parameters:
 *   dev - Device
 *   vector - IRQ number of the platform
 *
 * Returned Value:
 *   <0: Mapping failed
 *    0: Mapping succeed
 *
 ****************************************************************************/

static inline int pci_msi_register(FAR struct pci_dev_s *dev, uint16_t vector)
{
  return dev->bus->ops->pci_msi_register(dev, vector);
}


#undef EXTERN
#if defined(__cplusplus)
}
#endif
#endif
#endif /* __INCLUDE_NUTTX_I2C_I2C_MASTER_H */
