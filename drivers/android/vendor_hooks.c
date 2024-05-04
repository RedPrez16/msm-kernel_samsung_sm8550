// SPDX-License-Identifier: GPL-2.0-only
/* vendor_hook.c
 *
 * Android Vendor Hook Support
 *
 * Copyright 2020 Google LLC
 */

#ifndef __GENKSYMS__
#include <uapi/linux/android/binder.h>
#include <uapi/linux/hdreg.h>
#include <linux/bpf.h>
#include <linux/bpf_verifier.h>
#include <linux/coredump.h>
#include <linux/fileattr.h>
#include <linux/fsverity.h>
#include <linux/igmp.h>
#include <linux/ipc_namespace.h>
#include <linux/mtd/mtd.h>
#include <linux/pagemap.h>
#include <linux/pr.h>
#include <linux/skmsg.h>
#include <linux/statfs.h>
#include <linux/time_namespace.h>
#include <linux/timekeeper_internal.h>
#include <linux/zlib.h>
#include <net/ip6_fib.h>
#include <net/ip_tunnels.h>
#include <net/macsec.h>
#include <net/ioam6.h>
#include <net/netfilter/nf_flow_table.h>
#include <net/smc.h>
#include <net/tc_act/tc_gate.h>
#include <../fs/mount.h>
#include <../kernel/audit.h>
#include <../kernel/locking/mutex.h>
#include <../net/can/af_can.h>
#include <../net/tipc/bearer.h>
#include <../kernel/printk/printk_ringbuffer.h>
#endif

#define CREATE_TRACE_POINTS
#include <trace/hooks/vendor_hooks.h>
#include <linux/tracepoint.h>

#include <trace/hooks/user.h>
#include <trace/hooks/fpsimd.h>
#include <trace/hooks/binder.h>
#include <trace/hooks/futex.h>
#include <trace/hooks/dtask.h>
#include <trace/hooks/cpuidle.h>
#include <trace/hooks/topology.h>
#include <trace/hooks/mpam.h>
#include <trace/hooks/gic.h>
#include <trace/hooks/wqlockup.h>
#include <trace/hooks/debug.h>
#include <trace/hooks/sysrqcrash.h>
#include <trace/hooks/printk.h>
#include <trace/hooks/gic_v3.h>
#include <trace/hooks/epoch.h>
#include <trace/hooks/cpufreq.h>
#include <trace/hooks/mm.h>
#include <trace/hooks/preemptirq.h>
#include <trace/hooks/ftrace_dump.h>
#include <trace/hooks/ufshcd.h>
#ifdef __GENKSYMS__
#include <trace/hooks/block.h>
#endif
#include <trace/hooks/cgroup.h>
#include <trace/hooks/sys.h>
#include <trace/hooks/iommu.h>
#include <trace/hooks/net.h>
#include <trace/hooks/timer.h>
#include <trace/hooks/pm_domain.h>
#include <trace/hooks/cpuidle_psci.h>
#include <trace/hooks/vmscan.h>
#include <trace/hooks/avc.h>
#include <trace/hooks/creds.h>
#include <trace/hooks/memory.h>
#include <trace/hooks/module.h>
#ifdef __GENKSYMS__
#include <trace/hooks/selinux.h>
#endif
#include <trace/hooks/syscall_check.h>
#include <trace/hooks/logbuf.h>
#include <trace/hooks/remoteproc.h>
#include <trace/hooks/hung_task.h>
#include <trace/hooks/bug.h>
#include <trace/hooks/softlockup.h>
#include <trace/hooks/power.h>
#include <trace/hooks/fault.h>
#include <trace/hooks/traps.h>
#include <trace/hooks/fips140.h>
#include <trace/hooks/thermal.h>
#include <trace/hooks/rwsem.h>
#include <trace/hooks/timekeeping.h>
#include <trace/hooks/audio_usboffload.h>
#include <trace/hooks/drm_framebuffer.h>
#include <trace/hooks/drm_atomic.h>
#include <trace/hooks/psci.h>
#include <trace/hooks/usb.h>
#include <trace/hooks/regmap.h>
#include <trace/hooks/dmabuf.h>
#include <trace/hooks/mmc.h>
#include <trace/hooks/evdev.h>
#include <trace/hooks/signal.h>
#include <trace/hooks/cfg80211.h>
#include <trace/hooks/bl_hib.h>
#include <trace/hooks/xhci.h>
#include <trace/hooks/dm.h>
#include <trace/hooks/direct_io.h>
#include <trace/hooks/loop.h>
#include <trace/hooks/psi.h>
/*
 * Export tracepoints that act as a bare tracehook (ie: have no trace event
 * associated with them) to allow external modules to probe them.
 */
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_refrigerator);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_sk_alloc);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_sk_free);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_nf_conn_alloc);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_nf_conn_free);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_arch_set_freq_scale);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_is_fpsimd_save);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_transaction_init);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_priority_skip);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_set_priority);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_restore_priority);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_wakeup_ilocked);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_do_send_sig_info);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_alter_futex_plist_add);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_futex_sleep_start);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_do_futex);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_futex_wait_start);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_futex_wait_end);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_futex_wake_traverse_plist);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_futex_wake_this);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_futex_wake_up_q_finish);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mutex_wait_start);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mutex_wait_finish);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mutex_init);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_process_killed);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_killed_process);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_task_blocks_on_rtmutex);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rtmutex_waiter_prio);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rtmutex_wait_start);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rtmutex_wait_finish);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mutex_opt_spin_start);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mutex_opt_spin_finish);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mutex_can_spin_on_owner);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_read_wait_start);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_read_wait_finish);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_write_wait_start);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_write_wait_finish);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_opt_spin_start);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_opt_spin_finish);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_can_spin_on_owner);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_sched_show_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpu_idle_enter);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpu_idle_exit);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mpam_set);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_gic_resume);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_gic_suspend);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_wq_lockup_pool);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ipi_stop);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_sysrq_crash);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_printk_hotplug);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_printk_caller_id);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_printk_caller);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_printk_ext_header);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_gic_v3_set_affinity);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_gic_set_affinity);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_gic_v3_affinity_init);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_show_suspend_epoch_val);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_show_resume_epoch_val);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_freq_table_limits);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpufreq_resolve_freq);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpufreq_fast_switch);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpufreq_target);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpufreq_offline);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_set_skip_swapcache_flags);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_set_gfp_zone_flags);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_set_readahead_gfp_mask);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_update_readahead_gfp_mask);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_rmqueue_bulk);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_preempt_disable);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_preempt_enable);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_irqs_disable);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_irqs_enable);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_cpu_cgroup_attach);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_cpu_cgroup_can_attach);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_cpu_cgroup_online);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ftrace_oops_enter);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ftrace_oops_exit);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ftrace_size_check);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ftrace_format_check);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ftrace_dump_buffer);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_fill_prdt);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_ufs_complete_init);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_ufs_reprogram_all_keys);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_prepare_command);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_update_sysfs);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_send_command);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_compl_command);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cgroup_set_task);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_syscall_prctl_finished);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_send_uic_command);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_send_tm_command);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_check_int_errors);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_update_sdev);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_clock_scaling);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_use_mcq_hooks);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_mcq_max_tag);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_mcq_map_tag);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_mcq_set_sqid);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_mcq_handler);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_mcq_make_hba_operational);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_mcq_hba_capabilities);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_mcq_print_trs);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_mcq_send_command);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_mcq_config);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_mcq_has_oustanding_reqs);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_mcq_get_outstanding_reqs);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_mcq_abort);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_mcq_clear_cmd);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_mcq_clear_pending);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ufs_mcq_retry_complete);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cgroup_attach);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_iommu_setup_dma_ops);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_iommu_iovad_alloc_iova);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_iommu_iovad_free_iova);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ptype_head);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_kfree_skb);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_timer_calc_index);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_allow_domain_state);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpuidle_psci_enter);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpuidle_psci_exit);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_cgroup_force_kthread_migration);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_wait_for_work);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_proc_transaction_entry);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_select_worklist_ilocked);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_proc_transaction_finish);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_sync_txn_recvd);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_update_topology_flags_workfn);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_cpufreq_transition);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_freq_qos_add_request);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_freq_qos_update_request);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_freq_qos_remove_request);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_set_balance_anon_file_reclaim);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_show_max_freq);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_handle_failed_page_trylock);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_page_trylock_set);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_page_trylock_clear);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_page_trylock_get_result);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_do_page_trylock);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_dm_bufio_shrink_scan_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cleanup_old_buffers_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_page_referenced_check_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_drain_all_pages_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cma_drain_all_pages_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_pcplist_add_cma_pages_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_free_unref_page_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_kvmalloc_node_use_vmalloc);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_should_alloc_pages_retry);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_unreserve_highatomic_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rmqueue_bulk_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_ra_tuning_max_page);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_tune_mmap_readaround);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_shrink_slab_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_selinux_avc_insert);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_selinux_avc_node_delete);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_selinux_avc_node_replace);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_selinux_avc_lookup);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_commit_creds);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_exit_creds);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_override_creds);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_revert_creds);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_record_mutex_lock_starttime);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_record_rtmutex_lock_starttime);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_record_rwsem_lock_starttime);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_record_pcpu_rwsem_starttime);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_percpu_rwsem_wq_add);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_set_memory_nx);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_set_memory_rw);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_set_module_permit_before_init);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_set_module_permit_after_init);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_oom_check_panic);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_check_mmap_file);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_check_file_open);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_check_bpf_syscall);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_logbuf);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_logbuf_pr_cont);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rproc_recovery);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_check_uninterruptible_tasks);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_check_uninterruptible_tasks_dn);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_meminfo_proc_show);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_exit_mm);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_alloc_pages_slowpath);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_show_mem);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_print_slabinfo_header);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_do_shrink_slab);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cache_show);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_report_bug);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_watchdog_timer_softlockup);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_try_to_freeze_todo);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_try_to_freeze_todo_logging);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_try_to_freeze_todo_unfrozen);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_die_kernel_fault);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_do_sea);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_do_mem_abort);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_do_sp_pc_abort);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_do_undefinstr);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_do_ptrauth_fault);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_panic_unhandled);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_arm64_serror_panic);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_do_serror);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_vmpressure);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_sha256);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_aes_expandkey);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_aes_encrypt);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_aes_decrypt);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_modify_thermal_request_freq);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_modify_thermal_target_freq);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_thermal_register);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_thermal_unregister);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rproc_recovery_set);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_disable_thermal_cooling_stats);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_enable_thermal_power_throttle);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_modify_thermal_throttle_update);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_init);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_wake);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_write_finished);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_alter_rwsem_list_add);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_thermal_power_cap);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_tk_based_time_sync);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_kswapd_per_node);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_audio_usb_offload_vendor_set);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_audio_usb_offload_ep_action);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_audio_usb_offload_synctype);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_audio_usb_offload_suspend);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_audio_usb_offload_connect);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_audio_usb_offload_disconnect);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_atomic_remove_fb);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_drm_atomic_check_modeset);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_get_thermal_zone_device);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_psci_tos_resident_on);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_psci_cpu_suspend);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_usb_new_device_added);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_regmap_update);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_alter_mutex_list_add);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mutex_unlock_slowpath);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rwsem_wake_finish);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_dma_buf_release);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_dmabuf_heap_flags_validation);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_pass_input_event);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mmc_check_status);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mmap_region);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_try_to_unmap_one);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_shrink_node_memcgs);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mmc_sdio_pm_flag_set);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_tune_scan_type);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_tune_swappiness);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_partial_init);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_mmc_cache_card_properties);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_print_transaction_info);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_handle_tlb_conf);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_memcgv2_init);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_memcgv2_calc_decayed_watermark);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_update_watermark);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mmc_blk_reset);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mmc_blk_mq_rw_recovery);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_sd_update_bus_speed_mode);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mmc_attach_sd);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_sdhci_get_cd);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mmc_gpio_cd_irqt);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mmc_update_partition_status);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mmc_sd_update_cmdline_timing);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mmc_sd_update_dataline_timing);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_mmc_partition_status);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_mmc_sd_cmdline_timing);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_mmc_sd_dataline_timing);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cfg80211_set_context);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cfg80211_get_context);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_save_track_hash);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mem_cgroup_id_remove);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mem_cgroup_css_offline);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mem_cgroup_css_online);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mem_cgroup_free);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mem_cgroup_alloc);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_adjust_alloc_flags);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_kmalloc_slab);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_cpuset_fork);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_looper_state_registered);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_thread_read);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_free_proc);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_thread_release);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_has_work_ilocked);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_read_done);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_gic_v2_resume);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_exit_signal);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_alloc_new_buf_locked);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_reply);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_trans);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_preset);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_tune_memcg_scan_type);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_update_thermal_stats);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_proc_transaction);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_new_ref);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_binder_del_ref);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_show_mapcount_pages);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_do_traversal_lruvec);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_update_page_mapcount);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_add_page_to_lrulist);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_del_page_from_lrulist);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_page_should_be_protected);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mark_page_accessed);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_mmc_ffu_update_cid);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_alloc_uid);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_free_user);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_usb_dev_suspend);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_usb_dev_resume);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_xhci_suspend);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_xhci_resume);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_modify_thermal_cpu_get_power);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_page_cache_forced_ra);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_alloc_pages_reclaim_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_alloc_pages_failure_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cpufreq_acct_update_power);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rmqueue);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_tune_inactive_ratio);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_check_hibernation_swap);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_save_cpu_resume);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_save_hib_resume_bdev);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_alloc_highpage_movable_gfp_adjust);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_anon_gfp_adjust);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_alloc_flags_cma_adjust);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rmqueue_cma_fallback);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_try_cma_fallback);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_dma_buf_stats_teardown);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_madvise_cold_or_pageout);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_madvise_cold_or_pageout_abort);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cma_alloc_retry);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_encrypt_page);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_init_aes_encrypt);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_skip_swap_map_write);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_post_image_save);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_dm_update_clone_bio);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_rvh_ctl_dirty_rate);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_direct_io_update_bio);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_loop_prepare_cmd);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_psi_event);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_psi_group);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_rmqueue_smallest_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_free_one_page_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_use_cma_first_check);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_slab_page_alloced);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_kmalloc_order_alloced);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_compact_finished);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_check_page_look_around_ref);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_look_around);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_look_around_migrate_page);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_test_clear_look_around_ref);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_set_page_migrating);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_clear_page_migrating);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_cma_alloc_bypass);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_enable_thermal_genl_check);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_xhci_urb_suitable_bypass);
/*
 * For type visibility
 */
const struct readahead_control *GKI_struct_readahead_control;
EXPORT_SYMBOL_GPL(GKI_struct_readahead_control);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_exit_signal_whether_wake);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_exit_check);
EXPORT_TRACEPOINT_SYMBOL_GPL(android_vh_freeze_whether_wake);
