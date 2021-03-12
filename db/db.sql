-- phpMyAdmin SQL Dump
-- version 4.9.5
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: Mar 11, 2021 at 07:36 PM
-- Server version: 10.3.16-MariaDB
-- PHP Version: 7.3.23

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";

--
-- Database: `id16232074_prime17`
--

DELIMITER $$
--
-- Procedures
--
CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `check_diff` ()  READS SQL DATA
select 
*
from work_results A 
join work_results01 B on A.task_id = B.task_id and A.wr_power2 = B.wr_power2 and A.wr_inner_bits = B.wr_inner_bits

where A.wr_value <> B.wr_value$$

CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `completed_report` ()  READS SQL DATA
    COMMENT 'table of continually and partialy completed range'
with         
w as (    
    select 
      w.w_power2
    , max(w.w_end) as w_end
    from work w
    where w.progress_status_id=0
    group by w.w_power2
    
) , c1 as (    
    select 
      c.c_end
    , w.w_end
    , coalesce(w.w_power2, c.c_power2) as power2
    from completed c
    left join w on c.c_power2 = w.w_power2
    union 
    select 
      c.c_end
    , w.w_end
    , coalesce(w.w_power2, c.c_power2) as power2
    from completed c
    right join w on c.c_power2 = w.w_power2    
    
), w_taken as (
  	SELECT
      w.w_power2
    , max(w.w_end) as w_end_taken
    from work w
    where w.progress_status_id<>0
    group by w.w_power2    

) , c2 as (    
    select 
      c.c_end
    , c.w_end
    , w_taken.w_end_taken
    , coalesce(w_taken.w_power2, c.power2) as power2
    from c1 c
    left join w_taken on c.power2 = w_taken.w_power2
    union 
    select 
      c.c_end
    , c.w_end
    , w_taken.w_end_taken
    , coalesce(w_taken.w_power2, c.power2) as power2
    from c1 c
    right join w_taken on c.power2 = w_taken.w_power2
    
) , p4 as (
    SELECT
      power2
    , AA.c_end
    , AA.w_end
    , AA.w_end_taken
    , 100.0 * (AA.c_end / p2.p2_end_max_offset) as Completed
    , 100.0 * (AA.w_end / p2.p2_end_max_offset) as Completed_Max
    , 100.0 * (AA.w_end_taken / p2.p2_end_max_offset) as Taken
    from c2 AA
    join power2 p2 on AA.power2 = p2.power2_id     
)
select 
      power2
    , c_end
    , w_end
    , w_end_taken
    , Completed
    , case when Completed_Max = Completed then null else Completed_Max end as Completed_Max
    , case when Taken = Completed then null else Taken end as Taken
from p4
order by power2 DESC$$

CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `get_new_work` (IN `asking_worker_id` INT UNSIGNED)  MODIFIES SQL DATA
BEGIN
    START TRANSACTION;

create or replace temporary table curr as (	    
 with t as (
    SELECT `task_id` FROM `tasks` WHERE t_current = 1
    
), p2 as (
	SELECT t.task_id, coalesce(max(c.c_power2),1) as c_power2 
    from completed as c 
    right join t on t.task_id = c.task_id
    
), c as (
	SELECT p2.task_id, p2.c_power2, c.c_end
    FROM completed as c
    right JOIN p2 on p2.c_power2 = c.c_power2 and p2.task_id = c.task_id 
    
), cend as (
	SELECT task_id, c_power2, coalesce(max(c_end),0) as c_end
    from c
    group by task_id, c_power2
    
), cend2 as (
    SELECT task_id, c_power2, c_end, p.p2_end_max_offset
    from cend
    join power2 p on p.power2_id = cend.c_power2
    
), wrk as (
    SELECT 
    w.*
    , ps.ps_description
    from work as w 
    right join progress_status as ps on ps.progress_status_id = w.progress_status_id
    where ps.ps_description not in ('done')
    
), wrk2 as (
	select 
      cend2.task_id, cend2.c_power2, cend2.c_end, cend2.p2_end_max_offset
    , w.w_begin, w.w_end, w.w_creation 
    , TIMESTAMPDIFF(MINUTE, w.w_creation, CURRENT_TIMESTAMP) creation_min_diff
    , w.progress_status_id, w.ps_description
    , w.worker_id, w.work_id
    from wrk as w
    right join cend2 on cend2.task_id = w.task_id and cend2.c_power2 = w.w_power2  
    
) , wrk3 as (
	select 
    
    	  task_id, c_power2, c_end, p2_end_max_offset, w_begin, w_end
    	, w_creation, creation_min_diff
    	, CASE WHEN creation_min_diff > 500000 THEN 1 ELSE 0 end as expired
    	, progress_status_id, ps_description, worker_id, work_id  
    from wrk2
)
select 
    task_id, c_power2, c_end, p2_end_max_offset, w_begin, w_end, w_creation, creation_min_diff, expired, progress_status_id, ps_description, worker_id, work_id 
from wrk3
);


select task_id into @expired_task_id 
from curr 
where expired = 1
order by creation_min_diff DESC, w_begin asc
limit 1;

IF @expired_task_id IS not NULL THEN
	update work set worker_id = 1 where task_id = @expired_work_id; 
    
    SELECT
          task_id
        , asking_worker_id   
        , w_begin
        , w_end
        , c_power2
 		, @expired_work_id as new_work_id
    from curr 
    where work_id = @expired_work_id; 
    
ELSE
    create or replace temporary table new_work_record as (	    
    with nw as (
        select
        task_id, c_power2, c_end, p2_end_max_offset, max(w_end) as w_end
        from curr 
        group by task_id, c_power2, c_end, p2_end_max_offset

    ) , nw2 as (
        select
        task_id, c_power2, c_end, p2_end_max_offset
        , case
            when isnull(w_end) THEN 0
            when w_end < p2_end_max_offset THEN  w_end + 1
            else null
            end as new_begin
        from nw

    ), nw3 as ( 
        select 
          task_id 
        , case when isnull(new_begin) then c_power2+1 else c_power2 end as c_power2
        , case when isnull(new_begin) then 0 else c_end end as c_end
        , case when isnull(new_begin) then p2_end_max_offset + 1 + p2_end_max_offset else p2_end_max_offset end as p2_end_max_offset
        , case when isnull(new_begin) then 0 else new_begin end as new_begin
        from nw2

    ), nw4 as (
        select
          task_id 
        , c_power2
        , c_end, p2_end_max_offset
        , new_begin
        , LEAST(new_begin + 5000000, p2_end_max_offset) as new_end
        from nw3
    )   
    select
          task_id
        , asking_worker_id  as asking_worker_id
        ,  1 as progress_status_id
        , new_begin
        , new_end
        , c_power2 
    from nw4
    );
    
        
    INSERT INTO `work`(`task_id`, `worker_id`, `progress_status_id`, `w_begin`, `w_end`, `w_power2`)  	
    select 
          task_id
        , asking_worker_id
        , progress_status_id
        , new_begin
        , new_end
        , c_power2
    from new_work_record;
        
        
	select
          task_id
        , asking_worker_id   
        , new_begin
        , new_end
        , c_power2
        , last_insert_id() as new_work_id
    from new_work_record;
        
    drop table new_work_record;    
      
END IF;


drop table curr;

commit;
    
END$$

CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `get_work` (IN `asking_worker_id` INT UNSIGNED)  MODIFIES SQL DATA
BEGIN
    START TRANSACTION;

with exp1 as (
select 
      w.*
    , case when TIMESTAMPDIFF(MINUTE, w.w_creation, CURRENT_TIMESTAMP)  > 60 then  1 else 0 end expired

    from work as w
    where w.progress_status_id<>0
    )         
select work_id into @expired_work_id
from exp1
where expired = 1
order by w_creation ASC
limit 1;

#select @expired_work_id;

IF @expired_work_id IS not NULL THEN
	update work set worker_id = asking_worker_id where work_id = @expired_work_id; 
    
    SELECT
          task_id
        , asking_worker_id   
        , w_begin as new_begin
        , w_end as new_end
        , w_power2 as power2
 		, @expired_work_id as new_work_id
    from work 
    where work_id = @expired_work_id; 
    
ELSE
    create or replace temporary table new_work_record as (
        with
        t as (
            SELECT task_id FROM tasks WHERE t_current = 1

        #max work progress for each power
        ), w as (    
            select 
              w.w_power2
            , max(w.w_end) as w_end
            , t.task_id
            from work w
            join t on t.task_id = w.task_id 
            group by t.task_id, w.w_power2

        # max work progress from completed table
        ) , c as (    
            select 
              c.c_end
             ,c.c_power2
            , t.task_id
            from completed c
            join t on t.task_id = c.task_id
            
        # full outer join of completed and work    
        ), wc as (
            select
              c.c_end
            , w.w_end
            , COALESCE(c.c_power2, w.w_power2) as power2
            , COALESCE(c.task_id, w.task_id) as task_id
            from c
            right join w on w.w_power2 = c.c_power2 and w.task_id = c.task_id
            UNION    
            select
              c.c_end
            , w.w_end
            , COALESCE(c.c_power2, w.w_power2) as power2 
            , COALESCE(c.task_id, w.task_id) as task_id
            from c
            left join w on w.w_power2 = c.c_power2 and w.task_id = c.task_id

        # select max from completed and work
        ), curr as (
            select 
              wc.*
            , case 
                when  c_end is null then w_end
                when  w_end is null then c_end
                else greatest(c_end, w_end) end as work_end 
            , p2.p2_end_max_offset

            from wc
            join power2 p2 on p2.power2_id = wc.power2

        # select highest power2. 
        ), nw0 as (
                select max(power2) as max_power2 from curr

        # work only with the highest power
        ), nw as (
            select
            curr.*
            from curr
            join nw0  on nw0.max_power2 = curr.power2

        # Select next work (only begin at this stage) 
        ) , nw2 as (
            select
            task_id, power2, p2_end_max_offset
            , case
                when isnull(work_end) THEN 0
                when work_end < p2_end_max_offset THEN  work_end + 1
                else null
            end as new_begin
            from nw

        # and if needed skip to the next power2    
        ), nw3 as ( 
                select 
                  task_id 
                , case when isnull(new_begin) then power2+1 else power2 end as power2
                , case when isnull(new_begin) then p2_end_max_offset + 1 + p2_end_max_offset else p2_end_max_offset end as p2_end_max_offset
                , case when isnull(new_begin) then 0 else new_begin end as new_begin
                from nw2

        # compute an end from the calculated begin
        ), nw4 as (
                select
                  task_id 
                , power2
                #, p2_end_max_offset
                , new_begin
                , LEAST(new_begin + 1000000000, p2_end_max_offset) as new_end
                from nw3
        ) 
        select 
          *
        from nw4
        );        
    
        
    select count(1) into @cnt from new_work_record;
    if @cnt is null or @cnt =0 then   
   	    # history is empty, insert the very first record
    	insert into  new_work_record (task_id, new_begin, new_end, power2) values (1, 0, 1, 1);
    end if;
    
    INSERT INTO work (task_id, worker_id, progress_status_id, w_begin, w_end, w_power2)  	
    select 
          task_id
        , asking_worker_id
        , 1 as progress_status_id
        , new_begin
        , new_end
        , power2
    from new_work_record;
        
        
	select
          task_id
        , asking_worker_id   
        , new_begin
        , new_end
        , power2
        , last_insert_id() as new_work_id
    from new_work_record;
        
    drop table new_work_record;    
      
END IF;

commit;
    
END$$

CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `get_work02` (IN `asking_worker_id` INT UNSIGNED)  MODIFIES SQL DATA
BEGIN
    start transaction;
	
   with t as (
    SELECT `task_id` FROM `tasks` WHERE t_current = 1
), p2 as (
	SELECT t.task_id, coalesce(max(c.c_power2),1) as c_power2 
    from completed as c 
    right join t on t.task_id = c.task_id
), c as (
	SELECT p2.task_id, p2.c_power2, c.c_end
    FROM completed as c
    right JOIN p2 on p2.c_power2 = c.c_power2 and p2.task_id = c.task_id 
), cend as (
	SELECT task_id, c_power2, coalesce(max(c_end),0) as c_end
    from c
    group by task_id, c_power2
), cend2 as (
    SELECT task_id, c_power2, c_end, p.p2_end_max_offset
    from cend
    join power2 p on p.power2_id = cend.c_power2
), wrk as (
	select cend2.task_id, cend2.c_power2, cend2.c_end, cend2.p2_end_max_offset, w.w_begin, w.w_end, w.w_creation, w.progress_status_id, w.worker_id, w.work_id
    from work as w
    right join cend2 on cend2.task_id = w.task_id and cend2.c_power2 = w.w_power2 and w.w_begin > cend2.c_end 
)
select 
    task_id, c_power2, c_end, p2_end_max_offset, w_begin, w_end, w_creation, progress_status_id, worker_id, work_id 
from wrk;

	commit;
    
END$$

CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `get_work2` (IN `asking_worker_id` INT UNSIGNED)  MODIFIES SQL DATA
BEGIN
    START TRANSACTION;

with exp1 as (
select 
      w.*
    , case when TIMESTAMPDIFF(MINUTE, w.w_creation, CURRENT_TIMESTAMP)  > 60 then  1 else 0 end expired

    from work as w
    where w.progress_status_id<>0
    )         
select work_id into @expired_work_id
from exp1
where expired = 1
order by w_creation ASC
limit 1;

#select @expired_work_id;

IF @expired_work_id IS not NULL THEN
	update work set worker_id = asking_worker_id where work_id = @expired_work_id; 
    
    SELECT
          task_id
        , asking_worker_id   
        , w_begin as new_begin
        , w_end as new_end
        , w_power2 as power2
 		, @expired_work_id as new_work_id
    from work 
    where work_id = @expired_work_id; 
    
ELSE
    create or replace temporary table new_work_record as (
        with
        t as (
            SELECT task_id FROM tasks WHERE t_current = 1

        ), w as (    
            select 
              w.w_power2
            , max(w.w_end) as w_end
            , t.task_id
            from work w
            join t on t.task_id = w.task_id 
            group by t.task_id, w.w_power2


        ) , c as (    
            select 
              c.c_end
             ,c.c_power2
            , t.task_id
            from completed c
            join t on t.task_id = c.task_id
            
            
        ), wc as (
            select
              c.c_end
            , w.w_end
            , COALESCE(c.c_power2, w.w_power2) as power2
            , COALESCE(c.task_id, w.task_id) as task_id
            from c
            right join w on w.w_power2 = c.c_power2 and w.task_id = c.task_id
            UNION    
            select
              c.c_end
            , w.w_end
            , COALESCE(c.c_power2, w.w_power2) as power2 
            , COALESCE(c.task_id, w.task_id) as task_id
            from c
            left join w on w.w_power2 = c.c_power2 and w.task_id = c.task_id

        ), curr as (
            select 
              wc.*
            , case 
                when  c_end is null then w_end
                when  w_end is null then c_end
                else greatest(c_end, w_end) end as work_end 
            , p2.p2_end_max_offset

            from wc
            join power2 p2 on p2.power2_id = wc.power2

        # select highest power2. 
        ), nw0 as (
                select max(power2) as max_power2 from curr

        # work only with the highest power
        ), nw as (
            select
            curr.*
            from curr
            join nw0  on nw0.max_power2 = curr.power2

        # Select next work (only begin) 
        ) , nw2 as (
            select
            task_id, power2, p2_end_max_offset
            , case
                when isnull(work_end) THEN 0
                when work_end < p2_end_max_offset THEN  work_end + 1
                else null
            end as new_begin
            from nw

        # and if needed skip to the next power2    
        ), nw3 as ( 
                select 
                  task_id 
                , case when isnull(new_begin) then power2+1 else power2 end as power2
                , case when isnull(new_begin) then p2_end_max_offset + 1 + p2_end_max_offset else p2_end_max_offset end as p2_end_max_offset
                , case when isnull(new_begin) then 0 else new_begin end as new_begin
                from nw2

        # compute end from begin
        ), nw4 as (
                select
                  task_id 
                , power2
                , p2_end_max_offset
                , new_begin
                , LEAST(new_begin + 1000000000, p2_end_max_offset) as new_end
                from nw3
        ) 
        select 
          *
        from nw4
        );        
    
        
    INSERT INTO work (task_id, worker_id, progress_status_id, w_begin, w_end, w_power2)  	
    select 
          task_id
        , asking_worker_id
        , 1 as progress_status_id
        , new_begin
        , new_end
        , power2
    from new_work_record;
        
        
	select
          task_id
        , asking_worker_id   
        , new_begin
        , new_end
        , power2
        , last_insert_id() as new_work_id
    from new_work_record;
        
    drop table new_work_record;    
      
END IF;

commit;
    
END$$

CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `get_work_01` (IN `asking_worker_id` INT UNSIGNED)  MODIFIES SQL DATA
BEGIN
   with t as (
    SELECT `task_id` FROM `tasks` WHERE t_current = 1
), p2 as (
	SELECT t.task_id, coalesce(max(c.c_power2),1) as c_power2 
    from completed as c 
    right join t on t.task_id = c.task_id
), c as (
	SELECT p2.task_id, p2.c_power2, c.c_end
    FROM completed as c
    right JOIN p2 on p2.c_power2 = c.c_power2 and p2.task_id = c.task_id 
), cend as (
	SELECT task_id, c_power2, coalesce(max(c_end),0) as c_end
    from c
    group by task_id, c_power2
), cend2 as (
    SELECT task_id, c_power2, c_end, p.p2_end_max_offset
    from cend
    join power2 p on p.power2_id = cend.c_power2
), wrk as (
	select cend2.task_id, cend2.c_power2, cend2.c_end, cend2.p2_end_max_offset, w.w_begin, w.w_end, w.w_creation, w.progress_status_id, w.worker_id, w.work_id
    from work as w
    right join cend2 on cend2.task_id = w.task_id and cend2.c_power2 = w.w_power2 and w.w_begin > cend2.c_end 
)
select 
    task_id, c_power2, c_end, p2_end_max_offset, w_begin, w_end, w_creation, progress_status_id, worker_id, work_id 
from wrk;
    
END$$

CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `register_user_get_workers` (IN `email` VARCHAR(50))  READS SQL DATA
SELECT
  wer.worker_id
, wer.w_seed
, wer.w_name
, u.u_name
, date_format(wer.w_registered, '%Y-%m-%d %H:%i:%s') as w_created
FROM user u 
join worker wer on wer.user_id = u.user_id
WHERE 
upper(u.u_mail) = upper(email)$$

CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `register_worker` (IN `post` VARCHAR(20000))  MODIFIES SQL DATA
BEGIN
START TRANSACTION;

set @post = post;

set @email = upper(SUBSTRING_INDEX(@post, ',', 1));
set @w_name = SUBSTRING_INDEX(SUBSTRING_INDEX(@post, ',',2), ',', -1);
set @u_name = SUBSTRING_INDEX(@post, ',',-1); 

set @msg = "";
set @msg = concat(@msg, "@email = ", COALESCE(@email,"NULL"), ", ");
set @msg = concat(@msg, "@w_name = ", COALESCE(@w_name,"NULL"), ", ");
set @msg = concat(@msg, "@u_name = ", COALESCE(@u_name,"NULL"), ", ");


#select @msg as output;
#select @w_name as output;

select 
 u.user_id into @user_id
from user u	
where upper(u.u_mail) = upper(@email);

if @user_id is null then
    INSERT INTO user (u_mail, u_name) VALUES (@email, @u_name);
    SELECT LAST_INSERT_ID() into @user_id;
end if;

SELECT UUID() into @UUID;
INSERT INTO worker (user_id, w_seed, w_name) VALUES (@user_id, @UUID, @w_name);
SELECT LAST_INSERT_ID() into @worker_id;

commit;

select 
  @worker_id as worker_id
, @UUID as w_seed
, w.w_name
, u.u_name
, w.w_registered

from worker w 
join user u on u.user_id=w.user_id 
where w.worker_id = @worker_id
;


    
END$$

CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `work_output` ()  READS SQL DATA
    COMMENT 'provides also csv format'
select

wr_power2 as power2
, wr.wr_inner_bits as bits
, wr.wr_value as val
, concat(wr_power2, ', ', wr_inner_bits, ', ',  wr_value) as CSV
from  work_results wr 
where task_id =1 and wr.wr_inner_bits <= wr.wr_power2
order by  power2  asc, bits asc$$

CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `work_received` (IN `post_input` VARCHAR(20000) CHARSET utf8)  MODIFIES SQL DATA
BEGIN
START TRANSACTION;
set @post = post_input;

set @new_work_id = SUBSTRING_INDEX(@post, ',', 1);
set @asking_worker_id =  SUBSTRING_INDEX(SUBSTRING_INDEX(@post, ',', 2), ',', -1);
set @task_id = SUBSTRING_INDEX(SUBSTRING_INDEX(@post, ',', 3), ',', -1);
set @c_power2 = SUBSTRING_INDEX(SUBSTRING_INDEX(@post, ',', 4), ',', -1);
set @new_begin = SUBSTRING_INDEX(SUBSTRING_INDEX(@post, ',', 5), ',', -1);
set @new_end = SUBSTRING_INDEX(SUBSTRING_INDEX(@post, ',', 6), ',', -1);

set @msg = "";
set @res = SUBSTRING_INDEX(@post, ',', -65);
set @msg = concat(@msg, "@new_work_id = ", COALESCE(@new_work_id,"NULL"), ", ");
set @msg = concat(@msg, "@asking_worker_id = ", COALESCE(@asking_worker_id,"NULL"), ", ");
set @msg = concat(@msg, "@task_id = ", COALESCE(@task_id,"NULL"), ", ");
set @msg = concat(@msg, "@c_power2 = ", COALESCE(@c_power2,"NULL"), ", ");
set @msg = concat(@msg, "@new_begin = ", COALESCE(@new_begin,"NULL"), ",  ");

#select @msg as output;

select 
count(1) into @check_work
from  work w
where w.work_id = @new_work_id and w.task_id = @task_id and  w.w_begin=@new_begin and w.w_end=@new_end and w.w_power2 = @c_power2 and w.worker_id = @asking_worker_id and w.progress_status_id<>0
;
#w.worker_id = @asking_worker_id and 


#select @check_work;
if @check_work <> 1 THEN 
	#select concat ("Work task not found.", @msg) as output;
    set @msg = concat(@msg, "Work task not found. ");
ELSE
	
    FOR j IN 1..65
    DO
		#, i =1..65, 1 gives index 0
        set @i=j;
    	set @wr_value = SUBSTRING_INDEX(SUBSTRING_INDEX(@res, ',', @i), ',', -1);

        set @work_results_id=null;
        select 
        wr.work_results_id into @work_results_id
        from work_results wr
        where wr.wr_power2 = @c_power2 and wr.task_id = @task_id and  wr.wr_inner_bits = @i-1;
        
        if @work_results_id is null THEN
			INSERT INTO work_results (wr_inner_bits, wr_value, wr_power2, task_id)  VALUES (@i-1, @wr_value, @c_power2, @task_id);          
        ELSE
        	if @wr_value <> 0 then 
        		UPDATE work_results as wr SET wr_value = wr_value + @wr_value WHERE wr.work_results_id =  @work_results_id; 
            end if;
        END IF;

    END FOR; 
       
    update work w set w.progress_status_id = 0 where w.work_id = @new_work_id;
    #select "updated work and work_results" as output;
    
    # calculate new end for completed if neccessary
    WITH c1 AS(
        # work for selected power and its completed record (if any)
        SELECT
            w.*
          , c.c_end
          , COALESCE(c_end, -1) as begin_check
        FROM work w
        left JOIN completed c ON 
            c.c_power2 = w.w_power2 
        WHERE
            w.w_power2 = @c_power2 and w.progress_status_id = 0 and (c.c_end < w.w_begin or c.c_end is null)         
    
    # order by begin        
    ), c2 as (
        select 
          c1. *
        , ROW_NUMBER() OVER (ORDER BY w_begin asc) AS row_num
        from c1 

    # all finished rows join with the following  (even unfinished) 
    ), c3 as (
        select 
          A.*
        , case when A.progress_status_id is null then null else B.work_id end as work_id_following
        from c2 as A  
        left join c2 as B on A.w_end+1 = B.w_begin
    
    # first end of continuos range
    ), c4 as (
        SELECT
        min(row_num) as row_num_min
        FROM c3 where work_id_following is null
    
    # keep only very first continuos block and keep first row without a follower 
    ), c5 as (
		select 
        *
        from c3 A
        join c4 B on A.row_num <= B.row_num_min

    # min of begin must be begin_check, all rows must be completed, max is the output
    ), c6 as (
        SELECT
          min(c5.w_begin) as min_w_begin
        , max(c5.w_end) as max_w_end
        , min(c5.begin_check) as begin_check
        from c5
        where c5.progress_status_id = 0
           
    )
    select max_w_end into @new_end_to_update 
    from c6
    where min_w_begin = begin_check+1
    ;
    
    set @msg = concat(@msg, "@new_end_to_update=", COALESCE(@new_end_to_update,"NULL"), ", ");
    #select @new_end_to_update;

	if  @new_end_to_update is not null then

        select completed_id into @completed_id
        from completed c
        where  c.task_id = @task_id and c.c_power2 = @c_power2;
        set @msg = concat(@msg, "@completed_id=", COALESCE(@completed_id,"NULL", ", "));
        #select @completed_id;

        if @completed_id is null then
            insert into completed (task_id, c_power2, c_end) values (@task_id, @c_power2, @new_end_to_update);
            #select "new end inserted" as output;
            set @msg = concat(@msg, "new end inserted ");
        else
            update completed c set c.c_end = @new_end_to_update where c.completed_id  = @completed_id;
            #select "new end updated" as output;
            set @msg = concat(@msg, "new end updated ");
        end if;
    ELSE
    	set @msg = concat(@msg,  "no new end ");
        #select @msg as output; 
    end if;
   
end if;
if @msg is null then set @msg="An error, output msg is null"; end if;
select @msg as output;

commit;

    
END$$

CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `work_received2` (IN `post` VARCHAR(2000))  MODIFIES SQL DATA
BEGIN
    	set @post2 = SUBSTRING_INDEX(post, ',', 1);
		select @post2 from work;

    
	END$$

DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `completed`
--

CREATE TABLE `completed` (
  `completed_id` int(11) NOT NULL,
  `task_id` tinyint(3) UNSIGNED NOT NULL,
  `c_power2` tinyint(3) UNSIGNED NOT NULL,
  `c_end` bigint(20) UNSIGNED NOT NULL COMMENT 'offset from power2'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `power2`
--

CREATE TABLE `power2` (
  `power2_id` tinyint(3) UNSIGNED NOT NULL COMMENT 'ID=value of exponent for power of 2. Rage 1..64',
  `p2_end_max_offset` bigint(20) UNSIGNED NOT NULL COMMENT '(2^^id)-1'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `progress_status`
--

CREATE TABLE `progress_status` (
  `progress_status_id` tinyint(3) UNSIGNED NOT NULL COMMENT 'ID of the progress status',
  `ps_description` varchar(1000) COLLATE utf8_unicode_ci NOT NULL COMMENT 'Description of the progress status'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `tasks`
--

CREATE TABLE `tasks` (
  `task_id` tinyint(3) UNSIGNED NOT NULL COMMENT 'ID of the task',
  `t_description` varchar(1000) COLLATE utf8_unicode_ci NOT NULL COMMENT 'Description of the task',
  `t_current` tinyint(1) NOT NULL COMMENT 'only one task may be current'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `test`
--

CREATE TABLE `test` (
  `teststr` varchar(1000) COLLATE utf8_unicode_ci NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `user`
--

CREATE TABLE `user` (
  `user_id` int(11) NOT NULL COMMENT 'ID of the user',
  `u_mail` varchar(254) COLLATE utf8_unicode_ci NOT NULL COMMENT 'user email',
  `u_registered_date` datetime NOT NULL DEFAULT current_timestamp() COMMENT 'time of user registration',
  `u_name` varchar(50) COLLATE utf8_unicode_ci NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `work`
--

CREATE TABLE `work` (
  `work_id` bigint(20) NOT NULL COMMENT 'ID of work',
  `task_id` tinyint(4) UNSIGNED NOT NULL,
  `worker_id` int(10) UNSIGNED NOT NULL,
  `progress_status_id` tinyint(11) UNSIGNED NOT NULL,
  `w_creation` timestamp NOT NULL DEFAULT current_timestamp(),
  `w_begin` bigint(20) NOT NULL,
  `w_end` bigint(20) NOT NULL,
  `w_power2` tinyint(4) UNSIGNED NOT NULL COMMENT 'power frompower of two indication beginning if interval being analysed'
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `worker`
--

CREATE TABLE `worker` (
  `worker_id` int(10) UNSIGNED NOT NULL COMMENT 'ID of the worker',
  `user_id` int(11) NOT NULL,
  `w_registered` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `w_seed` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `w_name` varchar(1000) COLLATE utf8_unicode_ci NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `work_results`
--

CREATE TABLE `work_results` (
  `work_results_id` int(10) UNSIGNED NOT NULL,
  `wr_inner_bits` tinyint(3) UNSIGNED NOT NULL,
  `wr_value` bigint(20) UNSIGNED NOT NULL,
  `wr_power2` tinyint(3) UNSIGNED NOT NULL,
  `task_id` tinyint(3) UNSIGNED NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `work_results01`
--

CREATE TABLE `work_results01` (
  `work_results01_id` int(10) UNSIGNED NOT NULL,
  `wr_inner_bits` tinyint(3) UNSIGNED NOT NULL,
  `wr_value` bigint(20) UNSIGNED NOT NULL,
  `wr_power2` tinyint(3) UNSIGNED NOT NULL,
  `task_id` tinyint(3) UNSIGNED NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

--
-- Indexes for dumped tables
--

--
-- Indexes for table `completed`
--
ALTER TABLE `completed`
  ADD PRIMARY KEY (`completed_id`),
  ADD KEY `fk_completed_task_id` (`task_id`);

--
-- Indexes for table `power2`
--
ALTER TABLE `power2`
  ADD PRIMARY KEY (`power2_id`);

--
-- Indexes for table `progress_status`
--
ALTER TABLE `progress_status`
  ADD PRIMARY KEY (`progress_status_id`);

--
-- Indexes for table `tasks`
--
ALTER TABLE `tasks`
  ADD PRIMARY KEY (`task_id`);

--
-- Indexes for table `user`
--
ALTER TABLE `user`
  ADD PRIMARY KEY (`user_id`),
  ADD UNIQUE KEY `idx__user_unique` (`u_mail`) USING BTREE;

--
-- Indexes for table `work`
--
ALTER TABLE `work`
  ADD PRIMARY KEY (`work_id`),
  ADD UNIQUE KEY `idx_unique` (`w_power2`,`w_begin`),
  ADD KEY `fk_work_progress_status_id` (`progress_status_id`),
  ADD KEY `fk_work_task_id` (`task_id`),
  ADD KEY `fk_work_worker_id` (`worker_id`);

--
-- Indexes for table `worker`
--
ALTER TABLE `worker`
  ADD PRIMARY KEY (`worker_id`),
  ADD KEY `fk_worker_user_id` (`user_id`);

--
-- Indexes for table `work_results`
--
ALTER TABLE `work_results`
  ADD PRIMARY KEY (`work_results_id`),
  ADD KEY `work_results_task_id` (`task_id`);

--
-- Indexes for table `work_results01`
--
ALTER TABLE `work_results01`
  ADD PRIMARY KEY (`work_results01_id`),
  ADD KEY `work_results01_task_id` (`task_id`);

--
-- AUTO_INCREMENT for dumped tables
--

--
-- AUTO_INCREMENT for table `completed`
--
ALTER TABLE `completed`
  MODIFY `completed_id` int(11) NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `user`
--
ALTER TABLE `user`
  MODIFY `user_id` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID of the user';

--
-- AUTO_INCREMENT for table `work`
--
ALTER TABLE `work`
  MODIFY `work_id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT 'ID of work';

--
-- AUTO_INCREMENT for table `worker`
--
ALTER TABLE `worker`
  MODIFY `worker_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT 'ID of the worker';

--
-- AUTO_INCREMENT for table `work_results`
--
ALTER TABLE `work_results`
  MODIFY `work_results_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- AUTO_INCREMENT for table `work_results01`
--
ALTER TABLE `work_results01`
  MODIFY `work_results01_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT;

--
-- Constraints for dumped tables
--

--
-- Constraints for table `completed`
--
ALTER TABLE `completed`
  ADD CONSTRAINT `fk_completed_task_id` FOREIGN KEY (`task_id`) REFERENCES `tasks` (`task_id`);

--
-- Constraints for table `work`
--
ALTER TABLE `work`
  ADD CONSTRAINT `fk_work_progress_status_id` FOREIGN KEY (`progress_status_id`) REFERENCES `progress_status` (`progress_status_id`),
  ADD CONSTRAINT `fk_work_task_id` FOREIGN KEY (`task_id`) REFERENCES `tasks` (`task_id`),
  ADD CONSTRAINT `fk_work_worker_id` FOREIGN KEY (`worker_id`) REFERENCES `worker` (`worker_id`);

--
-- Constraints for table `worker`
--
ALTER TABLE `worker`
  ADD CONSTRAINT `fk_worker_user_id` FOREIGN KEY (`user_id`) REFERENCES `user` (`user_id`);

--
-- Constraints for table `work_results`
--
ALTER TABLE `work_results`
  ADD CONSTRAINT `work_results_task_id` FOREIGN KEY (`task_id`) REFERENCES `tasks` (`task_id`);

--
-- Constraints for table `work_results01`
--
ALTER TABLE `work_results01`
  ADD CONSTRAINT `work_results01_task_id` FOREIGN KEY (`task_id`) REFERENCES `tasks` (`task_id`);
COMMIT;
