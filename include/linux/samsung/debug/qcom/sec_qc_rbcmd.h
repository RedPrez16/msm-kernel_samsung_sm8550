#ifndef __SEC_QC_RBCMD_H__
#define __SEC_QC_RBCMD_H__

#if IS_ENABLED(CONFIG_POWER_RESET_MSM)
#include <linux/input/qpnp-power-on.h>
#else
enum pon_restart_reason {
	PON_RESTART_REASON_UNKNOWN		= 0x00,
	PON_RESTART_REASON_RECOVERY		= 0x01,
	PON_RESTART_REASON_BOOTLOADER		= 0x02,
	PON_RESTART_REASON_RTC			= 0x03,
	PON_RESTART_REASON_DMVERITY_CORRUPTED	= 0x04,
	PON_RESTART_REASON_DMVERITY_ENFORCE	= 0x05,
	PON_RESTART_REASON_KEYS_CLEAR		= 0x06,
	PON_RESTART_REASON_SILENT		= 0x0a,
	PON_RESTART_REASON_NON_SILENT		= 0x0b,
	PON_RESTART_REASON_FORCED_SILENT	= 0x0c,
	PON_RESTART_REASON_FORCED_NON_SILENT	= 0x0d,
};
#endif

/* qualcomm pon restart reason magic */
enum {
#if IS_ENABLED(CONFIG_SEC_QPNP_PON_SPARE_BITS) && (CONFIG_SEC_QPNP_PON_SPARE_BITS == 6)
	/* 32 ~ 63 for OEMs/ODMs secific features */
	PON_RESTART_REASON_OEM_MIN			= 0x20,
	PON_RESTART_REASON_OEM_MAX			= 0x3F,

	/* FIXME: DUMP_SINK feature is not used MSM8953 families */
	PON_RESTART_REASON_DUMP_SINK_BOOTDEV		= 0x07,
	PON_RESTART_REASON_DUMP_SINK_SDCARD		= 0x08,
	PON_RESTART_REASON_DUMP_SINK_USB		= 0x09,

	PON_RESTART_REASON_FORCE_UPLOAD_ON		= 0x10,
	PON_RESTART_REASON_FORCE_UPLOAD_OFF		= 0x11,
#endif /* CONFIG_SEC_QPNP_PON_SPARE_BITS == 6 */

	PON_RESTART_REASON_CP_CRASH			= 0x12,
	PON_RESTART_REASON_MANUAL_RESET			= 0x13,
	PON_RESTART_REASON_NORMALBOOT			= 0x14,
	PON_RESTART_REASON_DOWNLOAD			= 0x15,
	PON_RESTART_REASON_NVBACKUP			= 0x16,
	PON_RESTART_REASON_NVRESTORE			= 0x17,
	PON_RESTART_REASON_NVERASE			= 0x18,
	PON_RESTART_REASON_NVRECOVERY			= 0x19,
	PON_RESTART_REASON_SECURE_CHECK_FAIL		= 0x1A,
	PON_RESTART_REASON_WATCH_DOG			= 0x1B,
	PON_RESTART_REASON_KERNEL_PANIC			= 0x1C,
	PON_RESTART_REASON_THERMAL			= 0x1D,
	PON_RESTART_REASON_POWER_RESET			= 0x1E,
	PON_RESTART_REASON_WTSR				= 0x1F,
	PON_RESTART_REASON_RORY_START			= 0x20,
	/* TODO: 0x20 ~ 0x2A are reserved for RORY */
	PON_RESTART_REASON_RORY_END			= 0x2A,
	PON_RESTART_REASON_MULTICMD			= 0x2B,
	PON_RESTART_REASON_CROSS_FAIL			= 0x2C,
	PON_RESTART_REASON_LIMITED_DRAM_SETTING		= 0x2D,
	PON_RESTART_REASON_SLT_COMPLETE			= 0x2F,
	PON_RESTART_REASON_DBG_LOW			= 0x30,
	PON_RESTART_REASON_DBG_MID			= 0x31,
	PON_RESTART_REASON_DBG_HIGH			= 0x32,
	PON_RESTART_REASON_CP_DBG_ON			= 0x33,
	PON_RESTART_REASON_CP_DBG_OFF			= 0x34,
	PON_RESTART_REASON_CP_MEM_RESERVE_ON		= 0x35,
	PON_RESTART_REASON_CP_MEM_RESERVE_OFF		= 0x36,
	PON_RESTART_REASON_FIRMWAREUPDATE		= 0x37,
	PON_RESTART_REASON_SWITCHSEL			= 0x38, /* SET_SWITCHSEL_MODE */
	/* TODO: 0x38 ~ 0x3D are reserved for SWITCHSEL */
	PON_RESTART_REASON_SWITCHSEL_END		= 0x3D,
	PON_RESTART_REASON_RUSTPROOF			= 0x3D, /* SET_RUSTPROOF_MODE */
	PON_RESTART_REASON_MBS_MEM_RESERVE_ON		= 0x3E,
	PON_RESTART_REASON_MBS_MEM_RESERVE_OFF		= 0x3F,

#if IS_ENABLED(CONFIG_SEC_QPNP_PON_SPARE_BITS) && (CONFIG_SEC_QPNP_PON_SPARE_BITS == 6)
	PON_RESTART_REASON_MAX				= 0x40
#else
	PON_RESTART_REASON_USER_DRAM_TEST		= 0x40,
	PON_RESTART_REASON_QUEST_UEFI_START		= 0x41,
	PON_RESTART_REASON_QUEST_NMCHECKER_START	= 0x42,
	PON_RESTART_REASON_QUEST_NMCHECKER_SMD_START	= 0x43,
	PON_RESTART_REASON_QUEST_DRAM_START		= 0x44,
	PON_RESTART_REASON_QUEST_UEFI_DRAM_START	= 0x45,
	PON_RESTART_REASON_QUEST_DRAM_FAIL		= 0x46,
	PON_RESTART_REASON_QUEST_DRAM_TRAINIG_FAIL	= 0x47,
	
	PON_RESTART_REASON_FORCE_UPLOAD_ON		= 0x48,
	PON_RESTART_REASON_FORCE_UPLOAD_OFF		= 0x49,
	PON_RESTART_REASON_DUMP_SINK_BOOTDEV		= 0x4D,
	PON_RESTART_REASON_DUMP_SINK_SDCARD		= 0x4E,
	PON_RESTART_REASON_DUMP_SINK_USB		= 0x4F,
	
	PON_RESTART_REASON_QUEST_REWORK			= 0x50,
	PON_RESTART_REASON_QUEST_QUEFI_USER_START	= 0x51,
	PON_RESTART_REASON_QUEST_SUEFI_USER_START	= 0x52,
	PON_RESTART_REASON_QUEST_EDL_QUEST_DONE		= 0x53,
	PON_RESTART_REASON_QUEST_ERASE_PARAM		= 0x54,
	PON_RESTART_REASON_QUEST_QUEFI_PLUS_USER_START	= 0x55,
	PON_RESTART_REASON_QUEST_SUEFI_PLUS_USER_START	= 0x56,
	PON_RESTART_REASON_USER_DRAM_TEST_PLUS	= 0x57,
	PON_RESTART_REASON_USER_FLEX_CLK_START	= 0x58,
	PON_RESTART_REASON_USER_SVS_CLK_START	= 0x59,
	PON_RESTART_REASON_USER_NOMINAL_CLK_START	= 0x5A,
	PON_RESTART_REASON_USER_TURBO_CLK_START	= 0x5B,

	PON_RESTART_REASON_RECOVERY_UPDATE		= 0x60,
	PON_RESTART_REASON_BOTA_COMPLETE		= 0x61,

	PON_RESTART_REASON_CDSP_BLOCK			= 0x62,
	PON_RESTART_REASON_CDSP_ON			= 0x63,

	PON_RESTART_REASON_MAX				= 0x80
#endif
};

#define PON_RESTART_REASON_NOT_HANDLE	PON_RESTART_REASON_MAX

/* samsung reboot reason magic */
enum {
	RESTART_REASON_NORMAL			= 0x00000000,
	RESTART_REASON_BOOTLOADER		= 0x77665500,
	RESTART_REASON_REBOOT			= 0x77665501,
	RESTART_REASON_RECOVERY			= 0x77665502,
	RESTART_REASON_RTC			= 0x77665503,
	RESTART_REASON_DMVERITY_CORRUPTED	= 0x77665508,
	RESTART_REASON_DMVERITY_ENFORCE		= 0x77665509,
	RESTART_REASON_KEYS_CLEAR		= 0x7766550A,
	RESTART_REASON_SEC_DEBUG_MODE		= 0x776655EE,
	RESTART_REASON_END			= 0xFFFFFFFF,
};

#define RESTART_REASON_NOT_HANDLE	RESTART_REASON_END

struct sec_reboot_param;

#if IS_ENABLED(CONFIG_SEC_QC_RBCMD)
extern int sec_qc_rbcmd_register_pon_rr_writer(struct notifier_block *nb);
extern void sec_qc_rbcmd_unregister_pon_rr_writer(struct notifier_block *nb);

extern int sec_qc_rbcmd_register_sec_rr_writer(struct notifier_block *nb);
extern void sec_qc_rbcmd_unregister_sec_rr_writer(struct notifier_block *nb);

extern void sec_qc_rbcmd_set_restart_reason(unsigned int pon_rr, unsigned int sec_rr, struct sec_reboot_param *param);
#else
static inline int sec_qc_rbcmd_register_pon_rr_writer(struct notifier_block *nb) { return -ENODEV; }
static inline void sec_qc_rbcmd_unregister_pon_rr_writer(struct notifier_block *nb) {}

static inline int sec_qc_rbcmd_register_sec_rr_writer(struct notifier_block *nb) { return -ENODEV; }
static inline void sec_qc_rbcmd_unregister_sec_rr_writer(struct notifier_block *nb) {}

static inline void sec_qc_rbcmd_set_restart_reason(unsigned int pon_rr, unsigned int sec_rr, struct sec_reboot_param *param) {}
#endif

#endif /* __SEC_QC_RBCMD_H__ */
