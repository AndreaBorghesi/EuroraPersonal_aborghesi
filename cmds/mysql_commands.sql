-- mysql DB related commands 

-- get "database" size in MB
SELECT table_schema matlabDB, sum(data_length + index_length)/1024/1024 "size in MB" FROM information_schema.TABLES GROUP BY table_schema;

-- insert new index in existing table
ALTER TABLE core_measurements ADD INDEX (`node`); 
ALTER TABLE core_measurements ADD INDEX (`node`,`timestamp`); 
ALTER TABLE core_measurements ADD INDEX (`timestamp`); 
ALTER TABLE core_measurements ADD INDEX (`temp`); 
ALTER TABLE core_measurements ADD INDEX (`cpi`); 
ALTER TABLE core_measurements ADD INDEX (`load_core`); 

ALTER TABLE cpu_measurements ADD INDEX (`node`); 
ALTER TABLE cpu_measurements ADD INDEX (`node`,`timestamp`);
ALTER TABLE cpu_measurements ADD INDEX (`timestamp`); 

ALTER TABLE mic_measurements ADD INDEX (`node`);
ALTER TABLE mic_measurements ADD INDEX (`node`,`timestamp`);
ALTER TABLE mic_measurements ADD INDEX (`timestamp`);

alter table jobs_to_nodes add index (`node_id`,`job_id_string`);

-- temp 
select avg(pow_cpu),avg(pow_dram),avg(pow_pkg),avg(dT_cpu) from cpu_measurements_001 where timestamp between '2013/11/23 15:00:00' and  '2013/11/23 15:00:20' group by cpu_id;

select core_id as id,avg(cpi) as avg_cpi, avg(load_core) as avg_load, avg(mfreq) as avg_mfreq, avg(rfreq) as avg_rfreq, avg(dT_core) as avg_dT, avg(temp) as avg_temp from core_measurements_001 where timestamp between '2013/11/23 15:00:00' and '2013/11/23 15:00:20' group by core_id;

-- insert from text
load data local infile '/database/Cineca/Eurora/data/download/001.core.all.val' into table core_measurements_test fields terminated by ';' lines terminated by '\n' ignore 1 lines (core_id, cpi, load_core, rfreq, mfreq, dT_core, temp, ips, timestamp);

-- show the number of nodes used by a job (more or less..)
select j.job_id_string,count(*) from jobs j,jobs_to_nodes jn where j.job_id_string=jn.job_id_string group by j.job_id_string order by count(*);

-- check fast load infile progress
SELECT table_rows FROM information_schema.tables WHERE table_name = 'table';

-- count job reqeusts grouped by time_req
select count(*),time_req from jobs group by time_req;

-- check innoDB status
SHOW ENGINE INNODB STATUS\G

-- select jobs which waited in queue for at least 5 min
select job_id_string,start_time,run_start_time,end_time,queue,node_req,cpu_req from jobs where cpu_req is not null and TIME_TO_SEC(TIMEDIFF(run_start_time,start_time))>300;

-- force default values for unspecified job requests
update jobs set time_req = '00:30' where time_req like "--" and queue like 'debug';
update jobs set time_req = '06:00' where time_req like "--" and queue like 'parallel';
update jobs set time_req = '24:00' where time_req like "--" and queue like 'longpar';

-- count jobs which executed in a specified period of time
select count(*) from jobs where time_req is not null and start_time between '2014-03-01 00:00:00' and '2014-03-11 00:00:00';
