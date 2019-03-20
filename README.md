# lua-resty-snowflake

* 将lua文件复制到lualib/resty 目录下
* 生成libsnowflake.so，并配置到lua_package_cpath路径


* 添加lua代码：
```
  local snowflake = require "resty.snowflake"  
  local worker_id = 1
  local datacenter_id = 1
  local sf = snowflake.new(5,5)
  local next_id = sf:next_id()
```
