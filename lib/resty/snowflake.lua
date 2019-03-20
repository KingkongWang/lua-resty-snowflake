local ffi = require "ffi"
local ffi_load = ffi.load
local ffi_new = ffi.new
local ok, sf = pcall(ffi_load, "snowflake")
assert(ok, sf)

ffi.cdef[[
    typedef struct snowflake{
     
        int worker_id;
        int datacenter_id;
        int sequence;
        int64_t last_timestamp;
        bool initialized;
    }snowflake_t;

    bool snowflake_init(snowflake_t*, int, int);
    bool snowflake_next_id(snowflake_t*,int64_t*);
]]

local _M = {_VERSION = '0.01'}
local mt = { __index = _M }

function _M.new(worker_id, datacenter_id)
    assert(worker_id >= 0 and worker_id < 0x1f)
    assert(datacenter_id >= 0 and datacenter_id < 0x1f)

    local snowflake = ffi_new("snowflake_t")
    local flag = sf.snowflake_init(snowflake, worker_id, datacenter_id)
    if not flag then
        return nil
    end
    print('worker_id:', snowflake.worker_id)
    print('datacenter_id:', snowflake.datacenter_id)
    print('seq:', snowflake.sequence)
    print('last_timestamp:', snowflake.last_timestamp)
    print('initialized:', snowflake.initialized)


    return setmetatable({ 
        context = snowflake
    }, mt)

end

function _M.next_id(self)
    local id = ffi_new("int64_t[1]")
    local ok = sf.snowflake_next_id(self.context, id)
    assert(ok)

    print("next id:", id[0])
    return id[0]
end

return _M
