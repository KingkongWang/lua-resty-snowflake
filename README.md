# lua-resty-snowflake （snowflake算法在OpenResty下的实现）
* 将lib中文件复制到lualib目录相同文件夹下
* make命令生成libsnowflake.so


* 添加lua代码：
```
  local snowflake = require "resty.snowflake"  
  local worker_id = 1
  local datacenter_id = 1
  local sf = snowflake:new(worker_id, datacenter_id)
  local next_id = sf:next_id()
```
