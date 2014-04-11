#ifndef __SYSCALLS_HPP
#define __SYSCALLS_HPP

#include <functional>
#include <memory>

typedef target_ulong target_asid;

inline target_asid get_asid(CPUState *env, target_ulong addr);
inline target_ulong get_return_val(CPUState *env);

struct CallbackData {
    
};

//ReturnPoints contain a contuation that does something

typedef std::unique_ptr<CallbackData> CallbackDataPtr;

static CallbackDataPtr make_callbackptr(CallbackData* data){
    return CallbackDataPtr(data);
}

static void null_callback(CallbackData*, CPUState*, target_asid){
}

struct ReturnPoint {
    ReturnPoint() = delete;
    ReturnPoint(target_ulong retaddr, target_asid process_id,
                CallbackData* data = nullptr,
                std::function<void(CallbackData*, CPUState*, target_asid)> callback = nullptr){
        this->retaddr = retaddr;
        this->process_id = process_id;
        opaque = make_callbackptr(data);
        this->callback = callback;
    }
    target_ulong retaddr;
    target_asid process_id;
    CallbackDataPtr opaque;
    std::function<void(CallbackData*, CPUState*, target_asid)> callback;
};

void appendReturnPoint(ReturnPoint& rp);

#endif
