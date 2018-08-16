# Notes
- DCache相关随笔，学习记录
-------------
目录
-------------
* BinLog
  - 什么是BinLog？
      - binlog是二进制日志文件，用于记录数据库的数据更新或潜在更新，在mysql主从复制中就是依靠binlog。
      - binlog记录的都是事务操作内容，比如一条语句 DELETE FROM TABLE WHERE i > 1，当然格式是二进制的。
      - binlog是在事务最终commit前写入的。
      - 执行SELECT等不涉及数据更新的语句是不会记入binlog得，而涉及到数据更新则会记录，对于支持事务的引擎，必须要提交了事务才会记录binlog。
      - binlog刷新到磁盘的时机跟sync_binlog参数相关，如果设置为0，则表示mysql不控制binlog的刷新，由文件系统去控制它缓存的刷新，而如果设置为不为0的值则表示每sync_binlog次事务，mysql调用文件系统的刷新操作刷新到binlog磁盘中。设为1是最安全的，在系统故障时最多丢失一个事务的更新，但是会对性能有所影响，一般情况下会设置为100或者0，牺牲一定的一致性来获得更好的性能。
  - BinLog文件格式解析
      - binlog格式分为statement(基于SQL语句的模式，某些语句和函数如UUID，LOAD DATA INFILE等在复制过程中可能导致数据不一致甚至出错)，row(基于行的模式，记录的是行的变化，很安全。但是binlog会比其他两种模式大很多，在一些大的表中清除大量数据时在binlog中会生成很多条语句，可能导致库延迟变大)以及mixed(混合模式，根据语句来选用statement还是row模式)三种，mysql5.5中默认的是statement模式，在主从同步中不建议用statement模式，因为有些语句不支持，一般推荐的是mixed格式。
      - binlog文件以一个值为0Xfe62696e的魔数开头
      - binlog由一系列的binlog event构成。每个binlog event包含header和data两部分
      - header：提供event的公共的类型信息，包括event的创建时间，服务器等
      - data：提供针对该event的具体信息，如具体数据的修改
  - BinLog应用场景：
      - mysql主从复制：mysql replication在master端开启binlog，master把它的二进制日志传递给slaver来达到master-slaver数据一致的目的
      - 数据恢复，通过使用mysqlbinlog工具可以恢复数据
