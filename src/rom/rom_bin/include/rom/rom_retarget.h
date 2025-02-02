#ifndef _ROM_ROM_RETARGET_H_
#define _ROM_ROM_RETARGET_H_

#ifdef __CONFIG_ROM

#include "rom/kernel/FreeRTOS/FreeRTOSConfig.h"
#include "rom/kernel/os/os_errno.h"
#include "rom/kernel/os/os_time.h"
#include "rom/kernel/os/os_thread.h"
#include "rom/kernel/os/os_queue.h"
#include "rom/kernel/os/os_semaphore.h"
#include "rom/kernel/os/os_mutex.h"
#include "rom/kernel/os/os_timer.h"

#include "rom/driver/chip/private/hal_os.h"
#include "../../src/driver/chip/sdmmc/_sdhost.h" /* TODO: move to public path */

#include "rom/driver/chip/hal_util.h"
#include "rom/driver/chip/flashchip/flash_chip_cfg.h"
#include "rom/driver/chip/flashchip/flash_chip.h"
#include "rom/driver/chip/hal_flash.h"
#include "rom/driver/chip/hal_flashctrl.h"
#include "rom/driver/chip/hal_icache.h"
#include "rom/driver/chip/sdmmc/hal_sdhost.h"

#include "rom/pm/pm.h"

#include "rom/image/fdcm.h"
#include "rom/image/flash.h"
#include "rom/image/image.h"

#endif /* __CONFIG_ROM */

#endif /*_ROM_ROM_RETARGET_H_*/
