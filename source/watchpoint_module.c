#include <linux/module.h>
#include <linux/kernel.h>	
#include <linux/init.h>      
#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>

struct perf_event * __percpu *work_breakpoint = NULL;
bool work_bp_registred = false;

static struct kobject *watchpoint_kobj = NULL;
static char buffer[160];
static u32 address = 0;
static const u8 CODE_OF_SUCCESS = 0;

static int try_to_update_bp(const u32 address_);
// Задаём адрес отслеживания через оболочку при modprobe или insmod
module_param_string(address, buffer, sizeof(buffer), 0664);

// Читаем адрес из /sys/kernel/watchpoint/address
static ssize_t wp_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) {
    pr_info("%s: You have read from /sys/kernel/%s/%s, adress: 0x%x\n", THIS_MODULE->name, kobj->name, attr->attr.name, address);
    return sprintf(buf, "%x\n", address);
}

// Записываем адрес в sys/kernel/watchpoint/address
static ssize_t wp_store(struct kobject *kobj, struct kobj_attribute *attr, const char *buf, size_t count) {
    u32 new_address;
    kstrtou32(buf, 0, &new_address);

    if (new_address == address) {
        pr_info("%s: Breakpoint is already set on address: 0x%x!\n", THIS_MODULE->name, address);
        return count;
    }

    pr_info("%s: Updating... new address: 0x%x\n", THIS_MODULE->name, new_address);

    // Даже при провале установки breakpoint на новый адрес, адрес на который пытались
    // установить breakpoint всё равно запишем в address, чтобы не было путанницы
    if (try_to_update_bp(new_address) != CODE_OF_SUCCESS) {
        pr_info("%s: Updating breakpoint on new address: 0x%x failed!\n", THIS_MODULE->name, new_address);        
    }
    
    address = new_address;
    pr_info("%s: Address: 0x%x recorded in /sys/kernel/%s/%s\n", THIS_MODULE->name, address, kobj->name, attr->attr.name);
    
    return count;
}

static struct kobj_attribute watchpoint_attr = __ATTR(address, 0664, wp_show, wp_store);

static void breakpoint_handler(struct perf_event *bp, struct perf_sample_data *data, struct pt_regs *regs) {
    pr_info("%s: Value on address 0x%x changed\n", THIS_MODULE->name,(void*)(uintptr_t)data->addr);
    pr_info("%s: Dump stack from breakpoint_handler:\n", THIS_MODULE->name);    
    dump_stack();
    pr_info("%s: End of dump stack from breakpoint_handler\n", THIS_MODULE->name);
}

static int try_to_update_bp(const u32 address_) {

    if (work_bp_registred) {
        unregister_wide_hw_breakpoint(work_breakpoint);
        work_bp_registred = false;
        pr_info("%s: Breakpoint on old address: 0x%x unregistred\n", THIS_MODULE->name, address);
    }

    struct perf_event_attr attr;
 
    hw_breakpoint_init(&attr);
    attr.bp_addr = address_;
    attr.bp_len = HW_BREAKPOINT_LEN_2;
    attr.bp_type = HW_BREAKPOINT_W | HW_BREAKPOINT_R;
 
    work_breakpoint = register_wide_hw_breakpoint(&attr, breakpoint_handler, NULL);

    if (IS_ERR((void __force *)work_breakpoint)) {
        pr_info("%s: Breakpoint registration on 0x%x failed!\n", THIS_MODULE->name, address_);
        return 1;
    }
    
    work_bp_registred = true;
    pr_info("%s: Breakpoint on 0x%x installed succesfully!\n", THIS_MODULE->name, address_);
 
    return CODE_OF_SUCCESS;
}

static int __init wp_init(void) {
    pr_info("Loading module \"%s\"...\n", THIS_MODULE->name);
    kstrtou32(buffer, 0, &address);
    pr_info("%s: Watchpoint address: 0x%x\n", THIS_MODULE->name, address);

    watchpoint_kobj = kobject_create_and_add("watchpoint", kernel_kobj);
    if (!watchpoint_kobj) {
        pr_info("%s: ERROR creating watchpoint!\n", THIS_MODULE->name);
        return -ENOMEM;
    }

    if (sysfs_create_file(watchpoint_kobj, &watchpoint_attr.attr)) {               
        kobject_put(watchpoint_kobj);
        pr_info("%s: ERROR creating watchpoint/address!\n", THIS_MODULE->name);
        return -ENOMEM;
    }

    if (try_to_update_bp(address) != CODE_OF_SUCCESS) {
        sysfs_remove_file(watchpoint_kobj, &watchpoint_attr.attr);
        kobject_put(watchpoint_kobj);
        pr_info("%s: ERROR creating breakpoint!\n", THIS_MODULE->name);
        return 1;  
    }
  
    return CODE_OF_SUCCESS;
}

static void __exit wp_exit(void) {
    if (work_bp_registred) {
        unregister_wide_hw_breakpoint(work_breakpoint);
        work_bp_registred = false;
    }

    sysfs_remove_file(watchpoint_kobj, &watchpoint_attr.attr);
    kobject_put(watchpoint_kobj);
    
    pr_info("Unloading module \"%s\"...\n", THIS_MODULE->name);
}

module_init(wp_init);
module_exit(wp_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Proper Sergei, proper_dev@yahoo.com");
MODULE_DESCRIPTION("Watchpoint module");
