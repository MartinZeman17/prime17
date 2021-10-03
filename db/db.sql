-- phpMyAdmin SQL Dump
-- version 4.9.5
-- https://www.phpmyadmin.net/
--
-- Host: localhost:3306
-- Generation Time: Sep 27, 2021 at 05:18 PM
-- Server version: 10.3.16-MariaDB
-- PHP Version: 7.3.23

SET SQL_MODE = "NO_AUTO_VALUE_ON_ZERO";
SET AUTOCOMMIT = 0;
START TRANSACTION;
SET time_zone = "+00:00";

--
-- Database: `id16232074_prime17`
--
CREATE DATABASE IF NOT EXISTS `id16232074_prime17` DEFAULT CHARACTER SET utf8 COLLATE utf8_unicode_ci;
USE `id16232074_prime17`;

DELIMITER $$
--
-- Procedures
--
DROP PROCEDURE IF EXISTS `check_diff02`$$
CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `check_diff02` ()  READS SQL DATA
select 
*
from work_results A 
join work_results02 B on A.task_id = B.task_id and A.wr_power2 = B.wr_power2 and A.wr_inner_bits = B.wr_inner_bits

where A.wr_value <> B.wr_value$$

DROP PROCEDURE IF EXISTS `check_worker`$$
CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `check_worker` (IN `post_input` VARCHAR(20000))  READS SQL DATA
BEGIN
set @post = post_input;



set @worker_id = SUBSTRING_INDEX(@post, ',', 1);
set @w_seed =  SUBSTRING_INDEX(SUBSTRING_INDEX(@post, ',', 2), ',', -1);
set @w_name = SUBSTRING_INDEX(SUBSTRING_INDEX(@post, ',', 3), ',', -1);
set @u_name = SUBSTRING_INDEX(SUBSTRING_INDEX(@post, ',', 4), ',', -1);

set @res = null;
select 
w.worker_id into @res
from worker w
join user u on u.user_id = w.user_id
where w.worker_id = @worker_id and w.w_seed = @w_seed;

if @res is null THEN
	select @post as status;
ELSE
	select "OK" as status;
end if;

END$$

DROP PROCEDURE IF EXISTS `check_work_results`$$
CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `check_work_results` ()  READS SQL DATA
    COMMENT 'checks work_results against sum from work_results_rec'
with 
a1 as (    
SELECT 
task_id, wr_power2, wr_inner_bits, sum(wr_value) as wr_value_sum
FROM work_results_rec
group by task_id, wr_power2, wr_inner_bits

) , A2 as (    
select 
  A.*
, coalesce(B.wr_value, 0) as wr_value

   
from A1 A
left join work_results B on A.task_id = B.task_id and A.wr_power2 = B.wr_power2 and A.wr_inner_bits = B.wr_inner_bits
)
select * from A2
where wr_value <> wr_value_sum$$

DROP PROCEDURE IF EXISTS `completed_report`$$
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

    # full outer join not supported in MariaDB
) , c1 as (    
    select 
      c.c_end
    , w.w_end
    , coalesce(w.w_power2, c.c_power2) as power2
    , c.c_created
    , c.c_last_update
    from completed c
    left join w on c.c_power2 = w.w_power2
    union 
    select 
      c.c_end
    , w.w_end
    , coalesce(w.w_power2, c.c_power2) as power2
    , c.c_created
    , c.c_last_update    
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
    , c.c_created
    , c.c_last_update    
    from c1 c
    left join w_taken on c.power2 = w_taken.w_power2
    union 
    select 
      c.c_end
    , c.w_end
    , w_taken.w_end_taken
    , coalesce(w_taken.w_power2, c.power2) as power2
    , c.c_created
    , c.c_last_update    
    from c1 c
    right join w_taken on c.power2 = w_taken.w_power2
    
) , p4 as (
    SELECT
      power2
    , AA.c_end
    , AA.w_end
    , AA.w_end_taken
    , 100.0 * (AA.c_end / p2.p2_end_max_offset) as Completed
    , 100.0 * (AA.w_end / p2.p2_end_max_offset) as Completed_Gaps
    , 100.0 * (AA.w_end_taken / p2.p2_end_max_offset) as Taken
    , AA.c_created as Created
    , AA.c_last_update as LastUpdate
    from c2 AA
    join power2 p2 on AA.power2 = p2.power2_id     
),  p5 as (
    
    SELECT
      AA.*
    #, timediff(LastUpdate, Created) as Duration
    , format(TIMESTAMPDIFF(MINUTE, Created, LastUpdate)/(60*24.0), 2) as Days
    , (Completed + Completed_Gaps + Taken) / 300.0 as ComplAvg
    from p4 AA
)
    
select 
      power2
    , c_end
    , case when w_end = c_end then null else w_end end as w_end
    , w_end_taken
    , Completed
    , case when Completed_Gaps = Completed then null else Completed_Gaps end as Completed_Gaps
    , case when Taken = Completed then null else Taken end as Taken
    , convert_tz(Created, '+0:00','+2:00') as Created
    , convert_tz(LastUpdate, '+0:00','+2:00') as LastUpdate 
    #, Duration
    , Days
    , format(Days *  (1- ComplAvg) / ComplAvg, 2) as  RemainingDays

from p5
order by power2 DESC$$

DROP PROCEDURE IF EXISTS `configuration`$$
CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `configuration` ()  READS SQL DATA
select 
cg.cg_key, cg.cg_val_ui
from configuration cg$$

DROP PROCEDURE IF EXISTS `get_graph_data`$$
CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `get_graph_data` (IN `tbl` VARCHAR(50), IN `nbeg` VARCHAR(1000), IN `nend` VARCHAR(1000), IN `seq` VARCHAR(50))  READS SQL DATA
BEGIN
	SET @sql = CONCAT('SELECT COUNT(*) FROM ', tbl);
	PREPARE stmt FROM @sql;
	EXECUTE stmt;
	DEALLOCATE PREPARE stmt;
END$$

DROP PROCEDURE IF EXISTS `get_work`$$
CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `get_work` (IN `asking_worker_id` INT UNSIGNED)  MODIFIES SQL DATA
BEGIN

DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
    #GET CURRENT DIAGNOSTICS CONDITION 1 errno = MYSQL_ERRNO;
    #SELECT errno AS MYSQL_ERROR;
    ROLLBACK;
    RESIGNAL;
    END;


SET AUTOCOMMIT =0;
START TRANSACTION;

set @nibble_length = 10000000000;
select
cg.cg_val_ui into @nibble_length
from configuration cg
where cg.cg_key = 'nibble_length';

set @timeout_minutes = 180;
select
cg.cg_val_ui into @timeout_m
from configuration cg
where cg.cg_key = 'timeout_m'; 

set @expired_work_id = null;
with exp1 as (
select 
      w.*
    , case when TIMESTAMPDIFF(MINUTE, w.w_creation, CURRENT_TIMESTAMP)  > @timeout_m then  1 else 0 end expired

    from work as w
    where w.progress_status_id<>0
    )         
select work_id into @expired_work_id
from exp1
where expired = 1
order by w_creation ASC
limit 1;

IF @expired_work_id IS not NULL THEN
	#update worker and also time to prevent double taking by another worker
	update work set worker_id = asking_worker_id, w_creation = now()
    where work_id = @expired_work_id; 
    
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
                , LEAST(new_begin + @nibble_length, p2_end_max_offset) as new_end
                from nw3
        ) 
        select 
          *
        from nw4
        );        
    
    
    set @cnt = null;
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

DROP PROCEDURE IF EXISTS `get_work_backup`$$
CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `get_work_backup` (IN `asking_worker_id` INT UNSIGNED)  MODIFIES SQL DATA
BEGIN

DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
    #GET CURRENT DIAGNOSTICS CONDITION 1 errno = MYSQL_ERRNO;
    #SELECT errno AS MYSQL_ERROR;
    ROLLBACK;
    RESIGNAL;
    END;


SET AUTOCOMMIT =0;
START TRANSACTION;

set @nibble_length = 10000000000;
select
cg.cg_val_ui into @nibble_length
from configuration cg
where cg.cg_key = 'nibble_length';

set @timeout_minutes = 180;
select
cg.cg_val_ui into @timeout_m
from configuration cg
where cg.cg_key = 'timeout_m'; 

set @expired_work_id = null;
with exp1 as (
select 
      w.*
    , case when TIMESTAMPDIFF(MINUTE, w.w_creation, CURRENT_TIMESTAMP)  > @timeout_m then  1 else 0 end expired

    from work as w
    where w.progress_status_id<>0
    )         
select work_id into @expired_work_id
from exp1
where expired = 1
order by w_creation ASC
limit 1;

IF @expired_work_id IS not NULL THEN
	#update worker and also time to prevent double taking by another worker
	update work set worker_id = asking_worker_id, w_creation = now()
    where work_id = @expired_work_id; 
    
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
                , LEAST(new_begin + @nibble_length, p2_end_max_offset) as new_end
                from nw3
        ) 
        select 
          *
        from nw4
        );        
    
    
    set @cnt = null;
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

DROP PROCEDURE IF EXISTS `register_user_get_workers`$$
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

DROP PROCEDURE IF EXISTS `register_worker`$$
CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `register_worker` (IN `post` VARCHAR(20000))  MODIFIES SQL DATA
BEGIN
DECLARE EXIT HANDLER FOR SQLEXCEPTION
    BEGIN
    #GET CURRENT DIAGNOSTICS CONDITION 1 errno = MYSQL_ERRNO;
    #SELECT errno AS MYSQL_ERROR;
    ROLLBACK;
    RESIGNAL;
    END;


SET AUTOCOMMIT=0;
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

set @user_id = null;
select 
 u.user_id into @user_id
from user u	
where upper(u.u_mail) = upper(@email);

if @user_id is null then
    INSERT INTO user (u_mail, u_name) VALUES (@email, @u_name);
    SELECT LAST_INSERT_ID() into @user_id;
end if;

set @UUID=null;
set @worker_id = null;
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

DROP PROCEDURE IF EXISTS `user_merit`$$
CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `user_merit` ()  READS SQL DATA
BEGIN

set @Scale=null; 
select 
p2.p2_end_max_offset+1 into @Scale 
from 
power2 p2 
where p2.power2_id = 40;

#select @scale;


with A1 as ( 
    SELECT 
            wm.worker_id
          , sum((p2.p2_end_max_offset * wm.wm_merit)/(@scale*100.0)) as Merit 

    FROM worker_merit wm
    join power2 as p2 on wm.power2_id = p2.power2_id
    join tasks t on t.task_id = wm.task_id and t.t_current =1

    group by  wm.worker_id
    
) , A2 as (
 	SELECT  
      u.u_mail
    , u.u_name
    , u.u_registered_date   
    , w.w_name
    , w.w_registered
    , A1.merit
    from A1
    join worker w on w.worker_id = A1.worker_id
    join user u on u.user_id = w.user_id
), A3 as (
    select 
      u_mail
    , u_name
    , u_registered_date 
    , sum(merit) as merit
    from A2
    group by 
      u_mail
    , u_name
    , u_registered_date   
), A4 as (
     select 
      case when instr(u_mail, '@')=0 THEN  @mail
      ELSE 
        concat(substring(u_mail, 1,1), '***' , substring(u_mail, instr(u_mail, '@')-1,1 ), substring(u_mail, instr(u_mail, '@') ))
      end as u_mail      
    , u_name
    , u_registered_date 
    , merit
    from A3
)
select 
* 
from A4
order by Merit desc
limit 500
;



END$$

DROP PROCEDURE IF EXISTS `worker_merit`$$
CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `worker_merit` ()  READS SQL DATA
BEGIN
select 
p2.p2_end_max_offset+1 into @Scale 
from 
power2 p2 
where p2.power2_id = 40;

with A1 as ( 
    SELECT 
            wm.worker_id
          , sum((p2.p2_end_max_offset * wm.wm_merit)/(@scale*100.0)) as Merit 

    FROM worker_merit wm
    join power2 as p2 on wm.power2_id = p2.power2_id
    join tasks t on t.task_id = wm.task_id and t.t_current =1

    group by  wm.worker_id
    
) , A2 as (
 	SELECT  
      case when instr(u.u_mail, '@')=0 THEN  @mail
      ELSE 
        concat(substring(u.u_mail, 1,1), '***' , substring(u.u_mail, instr(u.u_mail, '@')-1,1 ), substring(u.u_mail, instr(u.u_mail, '@') ))
      end as u_mail     
    , u.u_name
    , u.u_registered_date   
    , w.w_name
    , w.w_registered
    , A1.merit
    from A1
    join worker w on w.worker_id = A1.worker_id
    join user u on u.user_id = w.user_id
)
select 
* 
from A2
order by Merit desc
limit 500
;


END$$

DROP PROCEDURE IF EXISTS `work_output`$$
CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `work_output` ()  READS SQL DATA
    COMMENT 'number of primes in T slices'
select

wr_power2 as power2
, wr.wr_inner_bits as bits
, wr.wr_value as val
#, concat(wr_power2, ', ', wr_inner_bits, ', ',  wr_value) as CSV
from  work_results wr 
where task_id =1 and wr.wr_inner_bits <= wr.wr_power2
order by  power2  asc, bits asc$$

DROP PROCEDURE IF EXISTS `work_outputTT`$$
CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `work_outputTT` ()  READS SQL DATA
    COMMENT 'number of primes in T slices'
with 
A1 as (
    select
      wr_power2 as power2
    , wr.wr_inner_bits as bits
    , wr.wr_value as val  
from  work_results wr 
where task_id =1 and wr.wr_inner_bits <= wr.wr_power2
) , A2 as (
    Select 
      A.power2
    , least (A.bits, B.bits) as bits
    #, A.val as Aval
    #, B.val as Bval
    , case when A.bits = B.bits then A.val else A.val + B.val end as val
    , case when A.bits = B.bits then 1 else 2 end as TTCnt
    from A1 A 
    join A1 B on A.power2 = B.power2 and  A.bits + B.bits =  B.power2 + 1  
    # be careful so that join condition can be evaluated inside of unsigned datatype
), A3 as (
	select  distinct * from A2
)
select 
*
from A3
order by power2 asc, bits asc$$

DROP PROCEDURE IF EXISTS `work_output_full_t`$$
CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `work_output_full_t` ()  READS SQL DATA
    COMMENT 'number of primes in T slices'
WITH
A1 as (
    
select
	  wr_power2 as power2
	, wr.wr_inner_bits as bits
	, wr.wr_value as pi_Nj
	from  work_results wr
where task_id =1 and wr.wr_inner_bits <= wr.wr_power2

), A2 as (
    
    select
    power2, 
    sum(pi_Nj) as Pi_N
    from  A1 AA
    group by power2 
    
), A3 as (
	select 
      A1.*
    , A2.Pi_N
    from A1
    join A2 on A1.power2 = A2.power2
    
),  A4 as (
	select 
    AA.*
    , b.bin_val as T_Nj
    , sum(b.bin_val) over (partition by  b.bin_n) as  T_N
    from A3 AA
    left join binomials b on b.bin_n+1 =  AA.power2 and  b.bin_k+1 = AA.bits

), A5 as (

select 
  AA.power2 as N 
, AA.bits as j 
, AA.T_Nj
, AA.Pi_N
, AA.Pi_Nj
, 100.0 * Pi_Nj/ Pi_N as q_Nj
, 100.0 * T_Nj/T_N as Z_Nj
from A4 AA 
) , A6 as (
    
    select 
      AA.* 
    , q_Nj - Z_Nj as F_Nj
    , q_Nj / Z_Nj as V_Nj
    , Pi_N * Z_Nj as L_Nj
    
    from A5 AA
) 
    select 
        AA.*
      , Pi_Nj - L_Nj as  P_Nj
    from A6 AA    
    order by  N  asc, j asc$$

DROP PROCEDURE IF EXISTS `work_received`$$
CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `work_received` (IN `post_input` VARCHAR(20000) CHARSET utf8)  MODIFIES SQL DATA
BEGIN


DECLARE EXIT HANDLER FOR SQLEXCEPTION 
        BEGIN
            #GET CURRENT DIAGNOSTICS CONDITION 1 errno = MYSQL_ERRNO;
    		#SELECT errno AS MYSQL_ERROR;              
    		ROLLBACK;
            set @msg = concat(@msg, "rollback");
            select @msg as output;            
            #RESIGNAL;
        END;


SET AUTOCOMMIT=0;
START TRANSACTION;
#SET AUTOCOMMIT=0;
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
set @msg = concat(@msg, "Autocommit: ", @@AUTOCOMMIT, " ");


set @w_creation = null;
select 
w_creation into @w_creation
from  work w
where w.work_id = @new_work_id and w.task_id = @task_id and  w.w_begin=@new_begin and w.w_end=@new_end and w.w_power2 = @c_power2 and w.worker_id = @asking_worker_id and w.progress_status_id<>0
;
#w.worker_id = @asking_worker_id and 


#select @check_work;
if @w_creation = null THEN 
	#select concat ("Work task not found.", @msg) as output;
    set @msg = concat(@msg, "Work task not found. ");
ELSE

 	CREATE temporary table inp
    (
        inner_bits tinyint(3) UNSIGNED NOT NULL,
  		val bigint(20) UNSIGNED NOT NULL
    ); 
	
    #FOR Stress in 1..100 DO
    FOR j IN 1..65
    DO
		#, i =1..65, 1 gives index 0
        set @i=j;
    	set @wr_value = SUBSTRING_INDEX(SUBSTRING_INDEX(@res, ',', @i), ',', -1);
        
        if @wr_value <>0 then 
        	INSERT into inp (inner_bits, val) values (@i-1, @wr_value);
        end if;
        

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
    #END FOR; #stress
    
    INSERT INTO work_results_rec (work_id, wr_inner_bits, wr_value, wr_power2, task_id)
    select @new_work_id, inner_bits, val, @c_power2, @task_id from inp;
    
    drop table inp;
       
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


	set @completed_id = null;
    set @completed_end_old =null;    
    set @completed_end_old = null;
    set @sel_inserted_completed_id = null;
    set @inserted_c_end = null;    
    
    select completed_id, c.c_end into @completed_id, @completed_end_old
        from completed c
        where  c.task_id = @task_id and c.c_power2 = @c_power2;

    set @msg = concat(@msg, "@completed_id=", COALESCE(@completed_id,"NULL"), " ");
    set @msg = concat(@msg, "@completed_end_old=", COALESCE(@completed_end_old,"NULL"), " ");        

	if  @new_end_to_update is not null then
        if @completed_id is null then
            insert into completed (task_id, c_power2, c_end, c_created, c_last_update) values (@task_id, @c_power2, @new_end_to_update, @w_creation, now());
            #set @msg = concat(@msg, "new end inserted ", COALESCE(LAST_INSERT_ID(), "NULL", " ");
            
            select 
            completed_id, c_end into @sel_inserted_completed_id, @inserted_c_end
            from completed where task_id = @task_id and c_power2 = @c_power2;
                              
            set @msg = concat(@msg, "select inserted completed_id: ", COALESCE(@sel_inserted_completed_id,"NULL"), " ");                              
            set @msg = concat(@msg, "select inserted c_end: ", COALESCE(@inserted_c_end,"NULL"), " ");            
        else
            update completed set c_end = @new_end_to_update, c_last_update = now()
            where completed_id  = @completed_id;
            #select "new end updated" as output;
            set @msg = concat(@msg, "new end updated ");
            
            select 
            c.completed_id, c.task_id, c.c_power2, c.c_end into @cid, @ctid, @cp2, @cend
            from completed c
            where c.completed_id = @completed_id;
            set @msg = concat(@msg, "@cid=", COALESCE(@cid,"NULL"), " ");        
            set @msg = concat(@msg, "@ctid=", COALESCE(@ctid,"NULL"), " ");        
            set @msg = concat(@msg, "@cp2=", COALESCE(@cp2,"NULL"), " ");        
            set @msg = concat(@msg, "@cend=", COALESCE(@cend,"NULL"), " ");            
            
        end if;
    ELSE
    	if @completed_id is null THEN
        	set @msg = concat(@msg,  "Error -  @new_end_to_update and @completed_id is null -> rollback");	
            rollback;
        ELSE
    		update completed set c_last_update = now() where completed_id  = @completed_id;
    	end if;
        set @msg = concat(@msg,  "no new end ");
    end if;    
   
	# power64 length is 2^64-1. When 1 is added it would overflow
    select 
        (100.0 * (@new_end - @new_begin + 1)) / case when p2.power2_id <=64 then p2.p2_end_max_offset+1 else p2.p2_end_max_offset+1 end  into @wm_merit
    from power2 as p2 
    where p2.power2_id = @c_power2;

    set @worker_merit_id = null;
    select 
    worker_merit_id into @worker_merit_id
    from worker_merit as wm
    where wm.task_id = @task_id and wm.worker_id = @asking_worker_id and wm.power2_id = @c_power2;

    if @worker_merit_id is null THEN
        INSERT INTO worker_merit (task_id, worker_id, power2_id, wm_merit) VALUES (@task_id, @asking_worker_id, @c_power2, @wm_merit);
        set @msg = concat(@msg, " merit inserted ", @wm_merit);
    ELSE
        UPDATE worker_merit 
        SET wm_merit = wm_merit + @wm_merit 
        where  worker_merit_id = @worker_merit_id;
        
        set @msg = concat(@msg, " merit ", @worker_merit_id, " increased by ", @wm_merit);     
    END IF; 
    
    update worker set w_last_activity = now() where worker_id = @asking_worker_id;
    
   
end if;
if @msg is null then set @msg="An error, output msg is null"; end if;
select @msg as output;

commit;

    
END$$

DROP PROCEDURE IF EXISTS `work_received_backup`$$
CREATE DEFINER=`id16232074_main`@`%` PROCEDURE `work_received_backup` (IN `post_input` VARCHAR(20000) CHARSET utf8)  MODIFIES SQL DATA
BEGIN


DECLARE EXIT HANDLER FOR SQLEXCEPTION 
        BEGIN
            #GET CURRENT DIAGNOSTICS CONDITION 1 errno = MYSQL_ERRNO;
    		#SELECT errno AS MYSQL_ERROR;              
    		ROLLBACK;
            set @msg = concat(@msg, "rollback");
            select @msg as output;            
            #RESIGNAL;
        END;


SET AUTOCOMMIT=0;
START TRANSACTION;
#SET AUTOCOMMIT=0;
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
set @msg = concat(@msg, "Autocommit: ", @@AUTOCOMMIT, " ");


set @w_creation = null;
select 
w_creation into @w_creation
from  work w
where w.work_id = @new_work_id and w.task_id = @task_id and  w.w_begin=@new_begin and w.w_end=@new_end and w.w_power2 = @c_power2 and w.worker_id = @asking_worker_id and w.progress_status_id<>0
;
#w.worker_id = @asking_worker_id and 


#select @check_work;
if @w_creation = null THEN 
	#select concat ("Work task not found.", @msg) as output;
    set @msg = concat(@msg, "Work task not found. ");
ELSE

 	CREATE temporary table inp
    (
        inner_bits tinyint(3) UNSIGNED NOT NULL,
  		val bigint(20) UNSIGNED NOT NULL
    ); 
	
    #FOR Stress in 1..100 DO
    FOR j IN 1..65
    DO
		#, i =1..65, 1 gives index 0
        set @i=j;
    	set @wr_value = SUBSTRING_INDEX(SUBSTRING_INDEX(@res, ',', @i), ',', -1);
        
        if @wr_value <>0 then 
        	INSERT into inp (inner_bits, val) values (@i-1, @wr_value);
        end if;
        

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
    #END FOR; #stress
    
    INSERT INTO work_results_rec (work_id, wr_inner_bits, wr_value, wr_power2, task_id)
    select @new_work_id, inner_bits, val, @c_power2, @task_id from inp;
    
    drop table inp;
       
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


	set @completed_id = null;
    set @completed_end_old =null;    
    set @completed_end_old = null;
    set @sel_inserted_completed_id = null;
    set @inserted_c_end = null;    
    
    select completed_id, c.c_end into @completed_id, @completed_end_old
        from completed c
        where  c.task_id = @task_id and c.c_power2 = @c_power2;

    set @msg = concat(@msg, "@completed_id=", COALESCE(@completed_id,"NULL"), " ");
    set @msg = concat(@msg, "@completed_end_old=", COALESCE(@completed_end_old,"NULL"), " ");        

	if  @new_end_to_update is not null then
        if @completed_id is null then
            insert into completed (task_id, c_power2, c_end, c_created, c_last_update) values (@task_id, @c_power2, @new_end_to_update, @w_creation, now());
            #set @msg = concat(@msg, "new end inserted ", COALESCE(LAST_INSERT_ID(), "NULL", " ");
            
            select 
            completed_id, c_end into @sel_inserted_completed_id, @inserted_c_end
            from completed where task_id = @task_id and c_power2 = @c_power2;
                              
            set @msg = concat(@msg, "select inserted completed_id: ", COALESCE(@sel_inserted_completed_id,"NULL"), " ");                              
            set @msg = concat(@msg, "select inserted c_end: ", COALESCE(@inserted_c_end,"NULL"), " ");            
        else
            update completed set c_end = @new_end_to_update, c_last_update = now()
            where completed_id  = @completed_id;
            #select "new end updated" as output;
            set @msg = concat(@msg, "new end updated ");
            
            select 
            c.completed_id, c.task_id, c.c_power2, c.c_end into @cid, @ctid, @cp2, @cend
            from completed c
            where c.completed_id = @completed_id;
            set @msg = concat(@msg, "@cid=", COALESCE(@cid,"NULL"), " ");        
            set @msg = concat(@msg, "@ctid=", COALESCE(@ctid,"NULL"), " ");        
            set @msg = concat(@msg, "@cp2=", COALESCE(@cp2,"NULL"), " ");        
            set @msg = concat(@msg, "@cend=", COALESCE(@cend,"NULL"), " ");            
            
        end if;
    ELSE
    	if @completed_id is null THEN
        	set @msg = concat(@msg,  "Error -  @new_end_to_update and @completed_id is null -> rollback");	
            rollback;
        ELSE
    		update completed set c_last_update = now() where completed_id  = @completed_id;
    	end if;
        set @msg = concat(@msg,  "no new end ");
    end if;    
   
	# power64 length is 2^64-1. When 1 is added it would overflow
    select 
        (100.0 * (@new_end - @new_begin + 1)) / case when p2.power2_id <=64 then p2.p2_end_max_offset+1 else p2.p2_end_max_offset+1 end  into @wm_merit
    from power2 as p2 
    where p2.power2_id = @c_power2;

    set @worker_merit_id = null;
    select 
    worker_merit_id into @worker_merit_id
    from worker_merit as wm
    where wm.task_id = @task_id and wm.worker_id = @asking_worker_id and wm.power2_id = @c_power2;

    if @worker_merit_id is null THEN
        INSERT INTO worker_merit (task_id, worker_id, power2_id, wm_merit) VALUES (@task_id, @asking_worker_id, @c_power2, @wm_merit);
        set @msg = concat(@msg, " merit inserted ", @wm_merit);
    ELSE
        UPDATE worker_merit 
        SET wm_merit = wm_merit + @wm_merit 
        where  worker_merit_id = @worker_merit_id;
        
        set @msg = concat(@msg, " merit ", @worker_merit_id, " increased by ", @wm_merit);     
    END IF; 
    
    update worker set w_last_activity = now() where worker_id = @asking_worker_id;
    
   
end if;
if @msg is null then set @msg="An error, output msg is null"; end if;
select @msg as output;

commit;

    
END$$

DELIMITER ;

-- --------------------------------------------------------

--
-- Table structure for table `binomials`
--

DROP TABLE IF EXISTS `binomials`;
CREATE TABLE IF NOT EXISTS `binomials` (
  `bin_n` tinyint(3) UNSIGNED NOT NULL,
  `bin_k` tinyint(3) UNSIGNED NOT NULL,
  `bin_val` bigint(20) UNSIGNED NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `completed`
--

DROP TABLE IF EXISTS `completed`;
CREATE TABLE IF NOT EXISTS `completed` (
  `completed_id` int(11) NOT NULL AUTO_INCREMENT,
  `task_id` tinyint(3) UNSIGNED NOT NULL,
  `c_power2` tinyint(3) UNSIGNED NOT NULL,
  `c_end` bigint(20) UNSIGNED NOT NULL COMMENT 'offset from power2',
  `c_created` datetime DEFAULT current_timestamp(),
  `c_last_update` datetime DEFAULT NULL,
  PRIMARY KEY (`completed_id`),
  KEY `fk_completed_task_id` (`task_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `configuration`
--

DROP TABLE IF EXISTS `configuration`;
CREATE TABLE IF NOT EXISTS `configuration` (
  `configuration_id` tinyint(4) NOT NULL AUTO_INCREMENT,
  `task_id` tinyint(3) UNSIGNED DEFAULT NULL,
  `cg_key` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `cg_val_ui` bigint(20) UNSIGNED NOT NULL,
  PRIMARY KEY (`configuration_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `pi`
--

DROP TABLE IF EXISTS `pi`;
CREATE TABLE IF NOT EXISTS `pi` (
  `power2` tinyint(3) UNSIGNED NOT NULL,
  `pi_N` bigint(20) UNSIGNED NOT NULL,
  PRIMARY KEY (`power2`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `power2`
--

DROP TABLE IF EXISTS `power2`;
CREATE TABLE IF NOT EXISTS `power2` (
  `power2_id` tinyint(3) UNSIGNED NOT NULL COMMENT 'ID=value of exponent for power of 2. Rage 1..64',
  `p2_end_max_offset` bigint(20) UNSIGNED NOT NULL COMMENT '(2^^id)-1',
  PRIMARY KEY (`power2_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `progress_status`
--

DROP TABLE IF EXISTS `progress_status`;
CREATE TABLE IF NOT EXISTS `progress_status` (
  `progress_status_id` tinyint(3) UNSIGNED NOT NULL COMMENT 'ID of the progress status',
  `ps_description` varchar(1000) COLLATE utf8_unicode_ci NOT NULL COMMENT 'Description of the progress status',
  PRIMARY KEY (`progress_status_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `tasks`
--

DROP TABLE IF EXISTS `tasks`;
CREATE TABLE IF NOT EXISTS `tasks` (
  `task_id` tinyint(3) UNSIGNED NOT NULL COMMENT 'ID of the task',
  `t_description` varchar(1000) COLLATE utf8_unicode_ci NOT NULL COMMENT 'Description of the task',
  `t_current` tinyint(1) NOT NULL COMMENT 'only one task may be current',
  PRIMARY KEY (`task_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `user`
--

DROP TABLE IF EXISTS `user`;
CREATE TABLE IF NOT EXISTS `user` (
  `user_id` int(11) NOT NULL AUTO_INCREMENT COMMENT 'ID of the user',
  `u_mail` varchar(254) COLLATE utf8_unicode_ci NOT NULL COMMENT 'user email',
  `u_registered_date` datetime NOT NULL DEFAULT current_timestamp() COMMENT 'time of user registration',
  `u_name` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  PRIMARY KEY (`user_id`),
  UNIQUE KEY `idx__user_unique` (`u_mail`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Stand-in structure for view `v_aux`
-- (See below for the actual view)
--
DROP VIEW IF EXISTS `v_aux`;
CREATE TABLE IF NOT EXISTS `v_aux` (
`N` tinyint(3) unsigned
,`j` tinyint(3) unsigned
,`m` decimal(8,1)
,`mm` bigint(10)
,`pi_Nj` bigint(20) unsigned
,`pi_N` bigint(20) unsigned
,`T_Nj` bigint(20) unsigned
,`T_N` decimal(42,0)
);

-- --------------------------------------------------------

--
-- Stand-in structure for view `v_check_pi`
-- (See below for the actual view)
--
DROP VIEW IF EXISTS `v_check_pi`;
CREATE TABLE IF NOT EXISTS `v_check_pi` (
`Power2` tinyint(3) unsigned
,`wr_pi_N` decimal(42,0)
,`pi_N` bigint(20) unsigned
,`diff` decimal(43,0)
,`diffP` decimal(51,5)
);

-- --------------------------------------------------------

--
-- Stand-in structure for view `v_full`
-- (See below for the actual view)
--
DROP VIEW IF EXISTS `v_full`;
CREATE TABLE IF NOT EXISTS `v_full` (
`N` tinyint(3) unsigned
,`Odd` int(3) unsigned
,`j` tinyint(3) unsigned
,`m` decimal(8,1)
,`mm` bigint(10)
,`pi_N` bigint(20) unsigned
,`T_Nj` bigint(20) unsigned
,`pi_Nj` bigint(20) unsigned
,`q_Nj` double
,`Z_Nj` double
,`F_Nj` double
,`V_Nj` double
,`L_Nj` double
,`P_Nj` double
,`TT_Nj` bigint(21) unsigned
,`ppi_Nj` decimal(21,0)
,`qq_Nj` double
,`ZZ_Nj` double
,`FF_Nj` double
,`VV_Nj` double
,`LL_Nj` double
,`PP_Nj` double
,`TTCnt` int(1)
);

-- --------------------------------------------------------

--
-- Stand-in structure for view `v_merit`
-- (See below for the actual view)
--
DROP VIEW IF EXISTS `v_merit`;
CREATE TABLE IF NOT EXISTS `v_merit` (
`t_description` varchar(1000)
,`u_mail` text
,`u_name` varchar(50)
,`u_registered_date` datetime
,`w_name` varchar(1000)
,`w_registered` timestamp
,`power2` tinyint(3) unsigned
,`IntervalPercent` double
,`Merit` double
,`current_power2` tinyint(3) unsigned
,`worker_last_activity` timestamp
);

-- --------------------------------------------------------

--
-- Stand-in structure for view `v_merit_user`
-- (See below for the actual view)
--
DROP VIEW IF EXISTS `v_merit_user`;
CREATE TABLE IF NOT EXISTS `v_merit_user` (
`t_description` varchar(1000)
,`u_mail` text
,`u_name` varchar(50)
,`u_registered_date` datetime
,`worker_last_activity` timestamp
,`Merit` double
);

-- --------------------------------------------------------

--
-- Stand-in structure for view `v_merit_worker`
-- (See below for the actual view)
--
DROP VIEW IF EXISTS `v_merit_worker`;
CREATE TABLE IF NOT EXISTS `v_merit_worker` (
`t_description` varchar(1000)
,`u_mail` text
,`u_name` varchar(50)
,`u_registered_date` datetime
,`w_registered` timestamp
,`worker_last_activity` timestamp
,`w_name` varchar(1000)
,`Merit` double
);

-- --------------------------------------------------------

--
-- Stand-in structure for view `v_merit_worker_current`
-- (See below for the actual view)
--
DROP VIEW IF EXISTS `v_merit_worker_current`;
CREATE TABLE IF NOT EXISTS `v_merit_worker_current` (
`t_description` varchar(1000)
,`u_mail` text
,`u_name` varchar(50)
,`u_registered_date` datetime
,`w_registered` timestamp
,`worker_last_activity` timestamp
,`current_power2` tinyint(3) unsigned
,`w_name` varchar(1000)
,`IntervalPercent` double
,`Merit` double
);

-- --------------------------------------------------------

--
-- Stand-in structure for view `v_S`
-- (See below for the actual view)
--
DROP VIEW IF EXISTS `v_S`;
CREATE TABLE IF NOT EXISTS `v_S` (
`N` tinyint(3) unsigned
,`Odd` int(3) unsigned
,`T_Ns` decimal(42,0)
,`pi_N` bigint(20) unsigned
,`pi_Ns0` decimal(42,0)
,`pi_Ns1` decimal(42,0)
,`qs0` double
,`qs1` double
,`Z_N` decimal(3,1)
,`V_Ns0` double
,`V_Ns1` double
,`F_Ns0` varchar(71)
,`F_Ns1` varchar(71)
,`L_Ns` double
,`P_Ns0` varchar(70)
,`P_Ns1` varchar(70)
);

-- --------------------------------------------------------

--
-- Stand-in structure for view `v_S_aux`
-- (See below for the actual view)
--
DROP VIEW IF EXISTS `v_S_aux`;
CREATE TABLE IF NOT EXISTS `v_S_aux` (
`N` tinyint(3) unsigned
,`Odd` int(3) unsigned
,`jOdd` int(1)
,`pi_N` bigint(20) unsigned
,`T_Ns` decimal(42,0)
,`pi_Ns` decimal(42,0)
,`qs` double
,`F_Ns` double
,`L_Ns` double
,`P_Ns` double
);

-- --------------------------------------------------------

--
-- Stand-in structure for view `v_S_aux2`
-- (See below for the actual view)
--
DROP VIEW IF EXISTS `v_S_aux2`;
CREATE TABLE IF NOT EXISTS `v_S_aux2` (
`N` tinyint(3) unsigned
,`Odd` int(3) unsigned
,`T_Ns` decimal(42,0)
,`pi_N` bigint(20) unsigned
,`pi_Ns0` decimal(42,0)
,`pi_Ns1` decimal(42,0)
,`qs0` double
,`qs1` double
,`F_Ns0` double
,`F_Ns1` double
,`L_Ns` double
,`P_Ns0` double
,`P_Ns1` double
);

-- --------------------------------------------------------

--
-- Stand-in structure for view `v_S_even`
-- (See below for the actual view)
--
DROP VIEW IF EXISTS `v_S_even`;
CREATE TABLE IF NOT EXISTS `v_S_even` (
`N` tinyint(3) unsigned
,`Odd` int(3) unsigned
,`T_Ns` decimal(42,0)
,`pi_N` bigint(20) unsigned
,`pi_Ns0` decimal(42,0)
,`pi_Ns1` decimal(42,0)
,`qs0` double
,`F_Ns0` varchar(71)
,`P_Ns0` varchar(70)
);

-- --------------------------------------------------------

--
-- Stand-in structure for view `v_S_odd`
-- (See below for the actual view)
--
DROP VIEW IF EXISTS `v_S_odd`;
CREATE TABLE IF NOT EXISTS `v_S_odd` (
`N` tinyint(3) unsigned
,`Odd` int(3) unsigned
,`T_Ns` decimal(42,0)
,`pi_N` bigint(20) unsigned
,`pi_Ns0` decimal(42,0)
,`pi_Ns1` decimal(42,0)
,`qs0` double
,`F_Ns0` varchar(71)
,`P_Ns0` varchar(70)
);

-- --------------------------------------------------------

--
-- Stand-in structure for view `v_T`
-- (See below for the actual view)
--
DROP VIEW IF EXISTS `v_T`;
CREATE TABLE IF NOT EXISTS `v_T` (
`N` tinyint(3) unsigned
,`Odd` int(3) unsigned
,`j` tinyint(3) unsigned
,`m` decimal(8,1)
,`mm` bigint(10)
,`pi_N` bigint(20) unsigned
,`T_Nj` bigint(20) unsigned
,`pi_Nj` bigint(20) unsigned
,`q_Nj` double
,`Z_Nj` double
,`F_Nj` double
,`V_Nj` double
,`L_Nj` double
,`P_Nj` double
);

-- --------------------------------------------------------

--
-- Table structure for table `v_t2`
--

DROP TABLE IF EXISTS `v_t2`;
CREATE ALGORITHM=UNDEFINED DEFINER=`id16232074_main`@`%` SQL SECURITY DEFINER VIEW `v_t2`  AS  with A4 as (select `A`.`N` AS `N`,`A`.`j` AS `j`,`A`.`m` AS `m`,`A`.`mm` AS `mm`,`A`.`pi_Nj` AS `pi_Nj`,`A`.`Pi_N` AS `Pi_N`,`A`.`T_Nj` AS `T_Nj`,`A`.`T_N` AS `T_N`,case when `A`.`mm` is null then NULL else case when `A`.`j` = `B`.`j` then `A`.`pi_Nj` else `A`.`pi_Nj` + `B`.`pi_Nj` end end AS `ppi_Nj`,case when `A`.`j` = `B`.`j` then 1 else 2 end AS `TTCnt` from (`v_T_aux` `A` left join `v_T_aux` `B` on(`A`.`N` = `B`.`N` and `A`.`j` + `B`.`j` = `B`.`N` + 1))), A5 as (select `AA`.`N` AS `N`,`AA`.`j` AS `j`,`AA`.`m` AS `m`,`AA`.`mm` AS `mm`,`AA`.`T_Nj` AS `T_Nj`,`AA`.`Pi_N` AS `Pi_N`,`AA`.`pi_Nj` AS `Pi_Nj`,cast(100.0 * `AA`.`pi_Nj` / `AA`.`Pi_N` as double) AS `q_Nj`,cast(100.0 * `AA`.`T_Nj` / `AA`.`T_N` as double) AS `Z_Nj`,cast(`AA`.`Pi_N` * (`AA`.`T_Nj` / `AA`.`T_N`) as double) AS `L_Nj`,`AA`.`ppi_Nj` AS `ppi_Nj`,`AA`.`TTCnt` AS `TTCnt` from `A4` `AA`), A6 as (select `AA`.`N` AS `N`,`AA`.`j` AS `j`,`AA`.`m` AS `m`,`AA`.`mm` AS `mm`,`AA`.`T_Nj` AS `T_Nj`,`AA`.`Pi_N` AS `Pi_N`,`AA`.`Pi_Nj` AS `Pi_Nj`,`AA`.`q_Nj` AS `q_Nj`,`AA`.`Z_Nj` AS `Z_Nj`,`AA`.`q_Nj` - `AA`.`Z_Nj` AS `F_Nj`,cast(`AA`.`q_Nj` / `AA`.`Z_Nj` as double) AS `V_Nj`,`AA`.`L_Nj` AS `L_Nj`,`AA`.`ppi_Nj` AS `ppi_Nj`,`AA`.`TTCnt` AS `TTCnt` from `A5` `AA`)select `AA`.`N` AS `N`,case when `AA`.`N` & 0x01 then 1 else 0 end AS `Odd`,`AA`.`j` AS `j`,`AA`.`m` AS `m`,`AA`.`mm` AS `mm`,`AA`.`T_Nj` AS `T_Nj`,`AA`.`Pi_N` AS `Pi_N`,`AA`.`Pi_Nj` AS `Pi_Nj`,`AA`.`q_Nj` AS `q_Nj`,`AA`.`Z_Nj` AS `Z_Nj`,`AA`.`F_Nj` AS `F_Nj`,`AA`.`V_Nj` AS `V_Nj`,`AA`.`L_Nj` AS `L_Nj`,`AA`.`Pi_Nj` - `AA`.`L_Nj` AS `P_Nj`,`AA`.`ppi_Nj` AS `ppi_Nj`,`AA`.`TTCnt` AS `TTCnt` from `A6` `AA` order by `AA`.`N`,`AA`.`j` ;

-- --------------------------------------------------------

--
-- Stand-in structure for view `v_TT`
-- (See below for the actual view)
--
DROP VIEW IF EXISTS `v_TT`;
CREATE TABLE IF NOT EXISTS `v_TT` (
`N` tinyint(3) unsigned
,`Odd` int(3) unsigned
,`j` tinyint(3) unsigned
,`m` decimal(8,1)
,`mm` bigint(10)
,`pi_N` bigint(20) unsigned
,`TT_Nj` bigint(21) unsigned
,`ppi_Nj` decimal(21,0)
,`qq_Nj` double
,`ZZ_Nj` double
,`FF_Nj` double
,`VV_Nj` double
,`LL_Nj` double
,`PP_Nj` double
,`TTCnt` int(1)
);

-- --------------------------------------------------------

--
-- Stand-in structure for view `v_U`
-- (See below for the actual view)
--
DROP VIEW IF EXISTS `v_U`;
CREATE TABLE IF NOT EXISTS `v_U` (
`N` tinyint(3) unsigned
,`Odd` int(3) unsigned
,`pi_N` bigint(20) unsigned
,`pi_Nu0` decimal(43,0)
,`pi_Nu1` decimal(43,0)
,`T_Nu0` decimal(43,0)
,`T_Nu1` decimal(43,0)
,`qu0` double
,`qu1` double
,`Z_Nu0` double
,`Z_Nu1` double
,`F_Nu0` double
,`F_Nu1` double
,`V_Nu0` double
,`V_Nu1` double
,`L_Nu0` double
,`L_Nu1` double
,`P_Nu0` double
,`P_Nu1` double
,`UCnt0` decimal(32,0)
,`UCnt1` decimal(32,0)
);

-- --------------------------------------------------------

--
-- Stand-in structure for view `v_U_aux`
-- (See below for the actual view)
--
DROP VIEW IF EXISTS `v_U_aux`;
CREATE TABLE IF NOT EXISTS `v_U_aux` (
`N` tinyint(3) unsigned
,`Odd` int(3) unsigned
,`jOdd` int(1)
,`pi_N` bigint(20) unsigned
,`T_Nu` decimal(43,0)
,`pi_Nu` decimal(43,0)
,`q_Nu` double
,`Z_Nu` double
,`F_Nu` double
,`V_Nu` double
,`L_Nu` double
,`P_Nu` double
,`UCnt` decimal(32,0)
);

-- --------------------------------------------------------

--
-- Stand-in structure for view `v_worker_merit_current_work`
-- (See below for the actual view)
--
DROP VIEW IF EXISTS `v_worker_merit_current_work`;
CREATE TABLE IF NOT EXISTS `v_worker_merit_current_work` (
`t_description` varchar(1000)
,`power2_id` tinyint(3) unsigned
,`u_mail` varchar(254)
,`u_name` varchar(50)
,`w_name` varchar(1000)
,`wm_merit` double
);

-- --------------------------------------------------------

--
-- Table structure for table `work`
--

DROP TABLE IF EXISTS `work`;
CREATE TABLE IF NOT EXISTS `work` (
  `work_id` bigint(20) NOT NULL AUTO_INCREMENT COMMENT 'ID of work',
  `task_id` tinyint(4) UNSIGNED NOT NULL,
  `worker_id` int(10) UNSIGNED NOT NULL,
  `progress_status_id` tinyint(11) UNSIGNED NOT NULL,
  `w_creation` timestamp NOT NULL DEFAULT current_timestamp(),
  `w_begin` bigint(20) NOT NULL,
  `w_end` bigint(20) NOT NULL,
  `w_power2` tinyint(4) UNSIGNED NOT NULL COMMENT 'power frompower of two indication beginning if interval being analysed',
  PRIMARY KEY (`work_id`),
  UNIQUE KEY `idx_unique` (`w_power2`,`w_begin`),
  KEY `fk_work_progress_status_id` (`progress_status_id`),
  KEY `fk_work_task_id` (`task_id`),
  KEY `fk_work_worker_id` (`worker_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `worker`
--

DROP TABLE IF EXISTS `worker`;
CREATE TABLE IF NOT EXISTS `worker` (
  `worker_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT COMMENT 'ID of the worker',
  `user_id` int(11) NOT NULL,
  `w_registered` timestamp NOT NULL DEFAULT current_timestamp() ON UPDATE current_timestamp(),
  `w_seed` varchar(50) COLLATE utf8_unicode_ci NOT NULL,
  `w_name` varchar(1000) COLLATE utf8_unicode_ci NOT NULL,
  `w_last_activity` timestamp NULL DEFAULT NULL,
  PRIMARY KEY (`worker_id`),
  KEY `fk_worker_user_id` (`user_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `worker_merit`
--

DROP TABLE IF EXISTS `worker_merit`;
CREATE TABLE IF NOT EXISTS `worker_merit` (
  `worker_merit_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `task_id` tinyint(3) UNSIGNED NOT NULL,
  `worker_id` int(10) UNSIGNED NOT NULL,
  `power2_id` tinyint(3) UNSIGNED NOT NULL,
  `wm_merit` double NOT NULL,
  PRIMARY KEY (`worker_merit_id`),
  KEY `work_merit_task_id` (`task_id`),
  KEY `work_merit_worker_id` (`worker_id`),
  KEY `work_merit_power2_id` (`power2_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `work_results`
--

DROP TABLE IF EXISTS `work_results`;
CREATE TABLE IF NOT EXISTS `work_results` (
  `work_results_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `wr_inner_bits` tinyint(3) UNSIGNED NOT NULL,
  `wr_value` bigint(20) UNSIGNED NOT NULL,
  `wr_power2` tinyint(3) UNSIGNED NOT NULL,
  `task_id` tinyint(3) UNSIGNED NOT NULL,
  PRIMARY KEY (`work_results_id`),
  KEY `work_results_task_id` (`task_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `work_results02`
--

DROP TABLE IF EXISTS `work_results02`;
CREATE TABLE IF NOT EXISTS `work_results02` (
  `work_results_id` int(10) UNSIGNED NOT NULL AUTO_INCREMENT,
  `wr_inner_bits` tinyint(3) UNSIGNED NOT NULL,
  `wr_value` bigint(20) UNSIGNED NOT NULL,
  `wr_power2` tinyint(3) UNSIGNED NOT NULL,
  `task_id` tinyint(3) UNSIGNED NOT NULL,
  PRIMARY KEY (`work_results_id`),
  KEY `work_results_task_id` (`task_id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Table structure for table `work_results_rec`
--

DROP TABLE IF EXISTS `work_results_rec`;
CREATE TABLE IF NOT EXISTS `work_results_rec` (
  `wrr_id` bigint(20) NOT NULL AUTO_INCREMENT,
  `work_id` bigint(20) UNSIGNED NOT NULL,
  `wr_inner_bits` tinyint(3) UNSIGNED NOT NULL,
  `wr_value` bigint(20) UNSIGNED NOT NULL,
  `wr_power2` tinyint(3) UNSIGNED NOT NULL,
  `task_id` tinyint(3) UNSIGNED NOT NULL,
  PRIMARY KEY (`wrr_id`),
  UNIQUE KEY `work_result_rec_unique` (`work_id`,`task_id`,`wr_power2`,`wr_inner_bits`) USING BTREE
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_unicode_ci;

-- --------------------------------------------------------

--
-- Structure for view `v_aux`
--
DROP TABLE IF EXISTS `v_aux`;

CREATE ALGORITHM=UNDEFINED DEFINER=`id16232074_main`@`%` SQL SECURITY DEFINER VIEW `v_aux`  AS  with A1 as (select `wr`.`wr_power2` AS `N`,`wr`.`wr_inner_bits` AS `j`,round(1.0 * `wr`.`wr_inner_bits` - (`wr`.`wr_power2` + 1.0) / 2.0,1) AS `m`,`wr`.`wr_value` AS `pi_Nj`,cast(sum(`wr`.`wr_value`) over ( partition by `wr`.`wr_power2`) as unsigned) AS `pi_N` from `work_results` `wr` where `wr`.`task_id` = 1 and `wr`.`wr_inner_bits` <= `wr`.`wr_power2`), A3 as (select `AA`.`N` AS `N`,`AA`.`j` AS `j`,`AA`.`m` AS `m`,case when `AA`.`N` & 0x01 = 1 then case when round(`AA`.`m` - 0.25,0) <= 0 then cast(round(`AA`.`m` - 0.25,0) as signed) else NULL end else case when round(`AA`.`m` - 0.25,0) < 0 then cast(round(`AA`.`m` - 0.25,0) as signed) else NULL end end AS `mm`,`AA`.`pi_Nj` AS `pi_Nj`,`AA`.`pi_N` AS `pi_N`,`b`.`bin_val` AS `T_Nj`,sum(`b`.`bin_val`) over ( partition by `b`.`bin_n`) AS `T_N` from (`A1` `AA` left join `binomials` `b` on(`b`.`bin_n` + 1 = `AA`.`N` and `b`.`bin_k` + 1 = `AA`.`j`)))select `A3`.`N` AS `N`,`A3`.`j` AS `j`,`A3`.`m` AS `m`,`A3`.`mm` AS `mm`,`A3`.`pi_Nj` AS `pi_Nj`,`A3`.`pi_N` AS `pi_N`,`A3`.`T_Nj` AS `T_Nj`,`A3`.`T_N` AS `T_N` from `A3` order by `A3`.`N`,`A3`.`j` ;

-- --------------------------------------------------------

--
-- Structure for view `v_check_pi`
--
DROP TABLE IF EXISTS `v_check_pi`;

CREATE ALGORITHM=UNDEFINED DEFINER=`id16232074_main`@`%` SQL SECURITY DEFINER VIEW `v_check_pi`  AS  with A1 as (select `wr`.`wr_power2` AS `wr_power2`,sum(`wr`.`wr_value`) AS `wr_pi_N` from `work_results` `wr` group by `wr`.`wr_power2`)select `BB`.`power2` AS `Power2`,`AA`.`wr_pi_N` AS `wr_pi_N`,`BB`.`pi_N` AS `pi_N`,`AA`.`wr_pi_N` - `BB`.`pi_N` AS `diff`,100.0 * (`AA`.`wr_pi_N` - `BB`.`pi_N`) / `BB`.`pi_N` AS `diffP` from (`A1` `AA` join `pi` `BB` on(`AA`.`wr_power2` = `BB`.`power2`)) where `BB`.`pi_N` <> `AA`.`wr_pi_N` ;

-- --------------------------------------------------------

--
-- Structure for view `v_full`
--
DROP TABLE IF EXISTS `v_full`;

CREATE ALGORITHM=UNDEFINED DEFINER=`id16232074_main`@`%` SQL SECURITY DEFINER VIEW `v_full`  AS  with A4 as (select `A`.`N` AS `N`,`A`.`j` AS `j`,`A`.`m` AS `m`,`A`.`mm` AS `mm`,`A`.`pi_Nj` AS `pi_Nj`,`A`.`pi_N` AS `pi_N`,`A`.`T_Nj` AS `T_Nj`,`A`.`T_N` AS `T_N`,case when `A`.`mm` is null then NULL else case when `A`.`j` = `B`.`j` then `A`.`pi_Nj` else `A`.`pi_Nj` + `B`.`pi_Nj` end end AS `ppi_Nj`,case when `A`.`j` = `B`.`j` then 1 else 2 end AS `TTCnt`,case when `A`.`j` = `B`.`j` then `A`.`T_Nj` else `A`.`T_Nj` + `A`.`T_Nj` end AS `TT_Nj` from (`v_aux` `A` left join `v_aux` `B` on(`A`.`N` = `B`.`N` and `A`.`j` + `B`.`j` = `B`.`N` + 1))), A5 as (select `AA`.`N` AS `N`,`AA`.`j` AS `j`,`AA`.`m` AS `m`,`AA`.`mm` AS `mm`,`AA`.`T_Nj` AS `T_Nj`,`AA`.`pi_N` AS `pi_N`,`AA`.`pi_Nj` AS `pi_Nj`,cast(100.0 * `AA`.`pi_Nj` / `AA`.`pi_N` as double) AS `q_Nj`,cast(100.0 * `AA`.`T_Nj` / `AA`.`T_N` as double) AS `Z_Nj`,cast(`AA`.`pi_N` * (`AA`.`T_Nj` / `AA`.`T_N`) as double) AS `L_Nj`,`AA`.`ppi_Nj` AS `ppi_Nj`,`AA`.`TTCnt` AS `TTCnt`,`AA`.`T_N` AS `T_N`,`AA`.`TT_Nj` AS `TT_Nj` from `A4` `AA`), A6 as (select `AA`.`N` AS `N`,`AA`.`j` AS `j`,`AA`.`m` AS `m`,`AA`.`mm` AS `mm`,`AA`.`T_Nj` AS `T_Nj`,`AA`.`pi_N` AS `pi_N`,`AA`.`pi_Nj` AS `pi_Nj`,`AA`.`q_Nj` AS `q_Nj`,`AA`.`Z_Nj` AS `Z_Nj`,`AA`.`q_Nj` - `AA`.`Z_Nj` AS `F_Nj`,cast(`AA`.`q_Nj` / `AA`.`Z_Nj` as double) AS `V_Nj`,`AA`.`L_Nj` AS `L_Nj`,`AA`.`TT_Nj` AS `TT_Nj`,`AA`.`ppi_Nj` AS `ppi_Nj`,`AA`.`TTCnt` AS `TTCnt`,cast(100.0 * `AA`.`ppi_Nj` / `AA`.`pi_N` as double) AS `qq_Nj`,cast(100.0 * `AA`.`TT_Nj` / `AA`.`T_N` as double) AS `ZZ_Nj`,cast(`AA`.`pi_N` * (`AA`.`TT_Nj` / `AA`.`T_N`) as double) AS `LL_Nj` from `A5` `AA`)select `AA`.`N` AS `N`,`AA`.`N` MOD 4 AS `Odd`,`AA`.`j` AS `j`,`AA`.`m` AS `m`,`AA`.`mm` AS `mm`,`AA`.`pi_N` AS `pi_N`,`AA`.`T_Nj` AS `T_Nj`,`AA`.`pi_Nj` AS `pi_Nj`,`AA`.`q_Nj` AS `q_Nj`,`AA`.`Z_Nj` AS `Z_Nj`,`AA`.`F_Nj` AS `F_Nj`,`AA`.`V_Nj` AS `V_Nj`,`AA`.`L_Nj` AS `L_Nj`,`AA`.`pi_Nj` - `AA`.`L_Nj` AS `P_Nj`,`AA`.`TT_Nj` AS `TT_Nj`,`AA`.`ppi_Nj` AS `ppi_Nj`,`AA`.`qq_Nj` AS `qq_Nj`,`AA`.`ZZ_Nj` AS `ZZ_Nj`,`AA`.`qq_Nj` - `AA`.`ZZ_Nj` AS `FF_Nj`,cast(`AA`.`qq_Nj` / `AA`.`ZZ_Nj` as double) AS `VV_Nj`,`AA`.`LL_Nj` AS `LL_Nj`,`AA`.`ppi_Nj` - `AA`.`LL_Nj` AS `PP_Nj`,`AA`.`TTCnt` AS `TTCnt` from `A6` `AA` order by `AA`.`N`,`AA`.`j` ;

-- --------------------------------------------------------

--
-- Structure for view `v_merit`
--
DROP TABLE IF EXISTS `v_merit`;

CREATE ALGORITHM=UNDEFINED DEFINER=`id16232074_main`@`%` SQL SECURITY DEFINER VIEW `v_merit`  AS  with A0 as (select `p2`.`p2_end_max_offset` + 1 AS `scale` from `power2` `p2` where `p2`.`power2_id` = 40), A1 as (select `t`.`task_id` AS `task_id`,`t`.`t_description` AS `t_description`,max(`wm`.`power2_id`) AS `current_power2` from (`worker_merit` `wm` join `tasks` `t` on(`t`.`task_id` = `wm`.`task_id`)) where `t`.`t_current` = 1), A2 as (select `A1`.`t_description` AS `t_description`,case when locate('@',`u`.`u_mail`) = 0 then `u`.`u_mail` else concat(substr(`u`.`u_mail`,1,1),'***',substr(`u`.`u_mail`,locate('@',`u`.`u_mail`) - 1,1),substr(`u`.`u_mail`,locate('@',`u`.`u_mail`))) end AS `u_mail`,`u`.`u_name` AS `u_name`,`u`.`u_registered_date` AS `u_registered_date`,`w`.`w_name` AS `w_name`,`w`.`w_registered` AS `w_registered`,`wm`.`power2_id` AS `power2`,`wm`.`wm_merit` AS `IntervalPercent`,`p2`.`p2_end_max_offset` * `wm`.`wm_merit` / (`A0`.`scale` * 100.0) AS `Merit`,`A1`.`current_power2` AS `current_power2`,`A0`.`scale` AS `Scale`,`w`.`w_last_activity` AS `w_last_activity` from (((((`worker_merit` `wm` join `worker` `w` on(`w`.`worker_id` = `wm`.`worker_id`)) join `user` `u` on(`u`.`user_id` = `w`.`user_id`)) join `A1` on(`A1`.`task_id` = `wm`.`task_id`)) join `power2` `p2` on(`p2`.`power2_id` = `wm`.`power2_id`)) join `A0`))select `A2`.`t_description` AS `t_description`,`A2`.`u_mail` AS `u_mail`,`A2`.`u_name` AS `u_name`,`A2`.`u_registered_date` AS `u_registered_date`,`A2`.`w_name` AS `w_name`,`A2`.`w_registered` AS `w_registered`,`A2`.`power2` AS `power2`,`A2`.`IntervalPercent` AS `IntervalPercent`,`A2`.`Merit` AS `Merit`,`A2`.`current_power2` AS `current_power2`,`A2`.`w_last_activity` AS `worker_last_activity` from `A2` order by `A2`.`power2` desc,`A2`.`Merit` desc ;

-- --------------------------------------------------------

--
-- Structure for view `v_merit_user`
--
DROP TABLE IF EXISTS `v_merit_user`;

CREATE ALGORITHM=UNDEFINED DEFINER=`id16232074_main`@`%` SQL SECURITY DEFINER VIEW `v_merit_user`  AS  with A1 as (select `m`.`t_description` AS `t_description`,`m`.`u_mail` AS `u_mail`,`m`.`u_name` AS `u_name`,`m`.`u_registered_date` AS `u_registered_date`,sum(`m`.`Merit`) AS `Merit`,max(`m`.`worker_last_activity`) AS `worker_last_activity` from `v_merit` `m` group by `m`.`t_description`,`m`.`u_mail`,`m`.`u_name`,`m`.`u_registered_date`)select `A1`.`t_description` AS `t_description`,`A1`.`u_mail` AS `u_mail`,`A1`.`u_name` AS `u_name`,`A1`.`u_registered_date` AS `u_registered_date`,`A1`.`worker_last_activity` AS `worker_last_activity`,`A1`.`Merit` AS `Merit` from `A1` order by `A1`.`Merit` desc limit 1000 ;

-- --------------------------------------------------------

--
-- Structure for view `v_merit_worker`
--
DROP TABLE IF EXISTS `v_merit_worker`;

CREATE ALGORITHM=UNDEFINED DEFINER=`id16232074_main`@`%` SQL SECURITY DEFINER VIEW `v_merit_worker`  AS  with A1 as (select `m`.`t_description` AS `t_description`,`m`.`u_mail` AS `u_mail`,`m`.`u_name` AS `u_name`,`m`.`u_registered_date` AS `u_registered_date`,`m`.`w_registered` AS `w_registered`,`m`.`w_name` AS `w_name`,sum(`m`.`Merit`) AS `Merit`,max(`m`.`worker_last_activity`) AS `worker_last_activity` from `v_merit` `m` group by `m`.`t_description`,`m`.`u_mail`,`m`.`u_name`,`m`.`u_registered_date`,`m`.`w_name`,`m`.`w_registered`)select `A1`.`t_description` AS `t_description`,`A1`.`u_mail` AS `u_mail`,`A1`.`u_name` AS `u_name`,`A1`.`u_registered_date` AS `u_registered_date`,`A1`.`w_registered` AS `w_registered`,`A1`.`worker_last_activity` AS `worker_last_activity`,`A1`.`w_name` AS `w_name`,`A1`.`Merit` AS `Merit` from `A1` order by `A1`.`Merit` desc limit 1000 ;

-- --------------------------------------------------------

--
-- Structure for view `v_merit_worker_current`
--
DROP TABLE IF EXISTS `v_merit_worker_current`;

CREATE ALGORITHM=UNDEFINED DEFINER=`id16232074_main`@`%` SQL SECURITY DEFINER VIEW `v_merit_worker_current`  AS  select `m`.`t_description` AS `t_description`,`m`.`u_mail` AS `u_mail`,`m`.`u_name` AS `u_name`,`m`.`u_registered_date` AS `u_registered_date`,`m`.`w_registered` AS `w_registered`,`m`.`worker_last_activity` AS `worker_last_activity`,`m`.`current_power2` AS `current_power2`,`m`.`w_name` AS `w_name`,`m`.`IntervalPercent` AS `IntervalPercent`,`m`.`Merit` AS `Merit` from `v_merit` `m` where `m`.`current_power2` = `m`.`power2` order by `m`.`Merit` desc limit 1000 ;

-- --------------------------------------------------------

--
-- Structure for view `v_S`
--
DROP TABLE IF EXISTS `v_S`;

CREATE ALGORITHM=UNDEFINED DEFINER=`id16232074_main`@`%` SQL SECURITY DEFINER VIEW `v_S`  AS  select `A`.`N` AS `N`,`A`.`Odd` AS `Odd`,`A`.`T_Ns` AS `T_Ns`,`A`.`pi_N` AS `pi_N`,`A`.`pi_Ns0` AS `pi_Ns0`,`A`.`pi_Ns1` AS `pi_Ns1`,`A`.`qs0` AS `qs0`,`A`.`qs1` AS `qs1`,50.0 AS `Z_N`,`A`.`qs0` / 50.0 AS `V_Ns0`,`A`.`qs1` / 50.0 AS `V_Ns1`,format(`A`.`F_Ns0`,15) AS `F_Ns0`,format(`A`.`F_Ns1`,15) AS `F_Ns1`,`A`.`L_Ns` AS `L_Ns`,format(`A`.`P_Ns0`,1) AS `P_Ns0`,format(`A`.`P_Ns1`,1) AS `P_Ns1` from `v_S_aux2` `A` order by `A`.`N` ;

-- --------------------------------------------------------

--
-- Structure for view `v_S_aux`
--
DROP TABLE IF EXISTS `v_S_aux`;

CREATE ALGORITHM=UNDEFINED DEFINER=`id16232074_main`@`%` SQL SECURITY DEFINER VIEW `v_S_aux`  AS  with A as (select `v_full`.`N` AS `N`,`v_full`.`Odd` AS `Odd`,case when `v_full`.`j` & 0x01 then 1 else 0 end AS `jOdd`,max(`v_full`.`pi_N`) AS `pi_N`,sum(case when `v_full`.`T_Nj` is null then 0 else `v_full`.`T_Nj` end) AS `T_Ns`,sum(`v_full`.`pi_Nj`) AS `pi_Ns` from `v_full` group by `v_full`.`N`,`v_full`.`Odd`,case when `v_full`.`j` & 0x01 then 1 else 0 end)select `A`.`N` AS `N`,`A`.`Odd` AS `Odd`,`A`.`jOdd` AS `jOdd`,`A`.`pi_N` AS `pi_N`,`A`.`T_Ns` AS `T_Ns`,`A`.`pi_Ns` AS `pi_Ns`,cast(100.0 * (`A`.`pi_Ns` / `A`.`pi_N`) as double) AS `qs`,cast(100.0 * (`A`.`pi_Ns` / `A`.`pi_N`) as double) - 50.0 AS `F_Ns`,cast(`A`.`pi_N` / 2.0 as double) AS `L_Ns`,cast(`A`.`pi_Ns` - `A`.`pi_N` / 2.0 as double) AS `P_Ns` from `A` order by `A`.`N`,`A`.`Odd`,`A`.`jOdd` ;

-- --------------------------------------------------------

--
-- Structure for view `v_S_aux2`
--
DROP TABLE IF EXISTS `v_S_aux2`;

CREATE ALGORITHM=UNDEFINED DEFINER=`id16232074_main`@`%` SQL SECURITY DEFINER VIEW `v_S_aux2`  AS  select `A`.`N` AS `N`,`A`.`Odd` AS `Odd`,`A`.`T_Ns` AS `T_Ns`,`A`.`pi_N` AS `pi_N`,`A`.`pi_Ns` AS `pi_Ns0`,`B`.`pi_Ns` AS `pi_Ns1`,`A`.`qs` AS `qs0`,`B`.`qs` AS `qs1`,`A`.`F_Ns` AS `F_Ns0`,`B`.`F_Ns` AS `F_Ns1`,cast(`A`.`pi_N` * 0.5 as double) AS `L_Ns`,cast(`A`.`pi_Ns` - `A`.`pi_N` * 0.5 as double) AS `P_Ns0`,cast(`B`.`pi_Ns` - `A`.`pi_N` * 0.5 as double) AS `P_Ns1` from (`v_S_aux` `A` join `v_S_aux` `B` on(`A`.`N` = `B`.`N` and `A`.`jOdd` <> `B`.`jOdd`)) where `A`.`jOdd` = 0 order by `A`.`N` ;

-- --------------------------------------------------------

--
-- Structure for view `v_S_even`
--
DROP TABLE IF EXISTS `v_S_even`;

CREATE ALGORITHM=UNDEFINED DEFINER=`id16232074_main`@`%` SQL SECURITY DEFINER VIEW `v_S_even`  AS  select `A`.`N` AS `N`,`A`.`Odd` AS `Odd`,`A`.`T_Ns` AS `T_Ns`,`A`.`pi_N` AS `pi_N`,`A`.`pi_Ns0` AS `pi_Ns0`,`A`.`pi_Ns1` AS `pi_Ns1`,`A`.`qs0` AS `qs0`,format(`A`.`F_Ns0`,15) AS `F_Ns0`,format(`A`.`P_Ns0`,1) AS `P_Ns0` from `v_S_aux2` `A` where `A`.`Odd` in (0,2) order by `A`.`N` ;

-- --------------------------------------------------------

--
-- Structure for view `v_S_odd`
--
DROP TABLE IF EXISTS `v_S_odd`;

CREATE ALGORITHM=UNDEFINED DEFINER=`id16232074_main`@`%` SQL SECURITY DEFINER VIEW `v_S_odd`  AS  select `A`.`N` AS `N`,`A`.`Odd` AS `Odd`,`A`.`T_Ns` AS `T_Ns`,`A`.`pi_N` AS `pi_N`,`A`.`pi_Ns0` AS `pi_Ns0`,`A`.`pi_Ns1` AS `pi_Ns1`,`A`.`qs0` AS `qs0`,format(`A`.`F_Ns0`,15) AS `F_Ns0`,format(`A`.`P_Ns0`,1) AS `P_Ns0` from `v_S_aux2` `A` where `A`.`Odd` in (1,3) order by `A`.`N` ;

-- --------------------------------------------------------

--
-- Structure for view `v_T`
--
DROP TABLE IF EXISTS `v_T`;

CREATE ALGORITHM=UNDEFINED DEFINER=`id16232074_main`@`%` SQL SECURITY DEFINER VIEW `v_T`  AS  select `v_full`.`N` AS `N`,`v_full`.`Odd` AS `Odd`,`v_full`.`j` AS `j`,`v_full`.`m` AS `m`,`v_full`.`mm` AS `mm`,`v_full`.`pi_N` AS `pi_N`,`v_full`.`T_Nj` AS `T_Nj`,`v_full`.`pi_Nj` AS `pi_Nj`,`v_full`.`q_Nj` AS `q_Nj`,`v_full`.`Z_Nj` AS `Z_Nj`,`v_full`.`F_Nj` AS `F_Nj`,`v_full`.`V_Nj` AS `V_Nj`,`v_full`.`L_Nj` AS `L_Nj`,`v_full`.`P_Nj` AS `P_Nj` from `v_full` order by `v_full`.`N`,`v_full`.`j` ;

-- --------------------------------------------------------

--
-- Structure for view `v_TT`
--
DROP TABLE IF EXISTS `v_TT`;

CREATE ALGORITHM=UNDEFINED DEFINER=`id16232074_main`@`%` SQL SECURITY DEFINER VIEW `v_TT`  AS  select `v_full`.`N` AS `N`,`v_full`.`Odd` AS `Odd`,`v_full`.`j` AS `j`,`v_full`.`m` AS `m`,`v_full`.`mm` AS `mm`,`v_full`.`pi_N` AS `pi_N`,`v_full`.`TT_Nj` AS `TT_Nj`,`v_full`.`ppi_Nj` AS `ppi_Nj`,`v_full`.`qq_Nj` AS `qq_Nj`,`v_full`.`ZZ_Nj` AS `ZZ_Nj`,`v_full`.`FF_Nj` AS `FF_Nj`,`v_full`.`VV_Nj` AS `VV_Nj`,`v_full`.`LL_Nj` AS `LL_Nj`,`v_full`.`PP_Nj` AS `PP_Nj`,`v_full`.`TTCnt` AS `TTCnt` from `v_full` where `v_full`.`mm` is not null and `v_full`.`TT_Nj` is not null order by `v_full`.`N`,`v_full`.`j` ;

-- --------------------------------------------------------

--
-- Structure for view `v_U`
--
DROP TABLE IF EXISTS `v_U`;

CREATE ALGORITHM=UNDEFINED DEFINER=`id16232074_main`@`%` SQL SECURITY DEFINER VIEW `v_U`  AS  select `A`.`N` AS `N`,`A`.`Odd` AS `Odd`,`A`.`pi_N` AS `pi_N`,`A`.`pi_Nu` AS `pi_Nu0`,`B`.`pi_Nu` AS `pi_Nu1`,`A`.`T_Nu` AS `T_Nu0`,`B`.`T_Nu` AS `T_Nu1`,`A`.`q_Nu` AS `qu0`,`B`.`q_Nu` AS `qu1`,`A`.`Z_Nu` AS `Z_Nu0`,`B`.`Z_Nu` AS `Z_Nu1`,`A`.`F_Nu` AS `F_Nu0`,`B`.`F_Nu` AS `F_Nu1`,`A`.`V_Nu` AS `V_Nu0`,`B`.`V_Nu` AS `V_Nu1`,`A`.`L_Nu` AS `L_Nu0`,`B`.`L_Nu` AS `L_Nu1`,`A`.`P_Nu` AS `P_Nu0`,`B`.`P_Nu` AS `P_Nu1`,`A`.`UCnt` AS `UCnt0`,`B`.`UCnt` AS `UCnt1` from (`v_U_aux` `A` join `v_U_aux` `B` on(`A`.`N` = `B`.`N` and `A`.`jOdd` <> `B`.`jOdd` and `A`.`jOdd` = 0)) order by `A`.`N` ;

-- --------------------------------------------------------

--
-- Structure for view `v_U_aux`
--
DROP TABLE IF EXISTS `v_U_aux`;

CREATE ALGORITHM=UNDEFINED DEFINER=`id16232074_main`@`%` SQL SECURITY DEFINER VIEW `v_U_aux`  AS  select `v_TT`.`N` AS `N`,`v_TT`.`Odd` AS `Odd`,case when `v_TT`.`j` & 0x01 then 1 else 0 end AS `jOdd`,`v_TT`.`pi_N` AS `pi_N`,sum(`v_TT`.`TT_Nj`) AS `T_Nu`,sum(`v_TT`.`ppi_Nj`) AS `pi_Nu`,sum(`v_TT`.`qq_Nj`) AS `q_Nu`,sum(`v_TT`.`ZZ_Nj`) AS `Z_Nu`,sum(`v_TT`.`qq_Nj`) - sum(`v_TT`.`ZZ_Nj`) AS `F_Nu`,sum(`v_TT`.`qq_Nj`) / sum(`v_TT`.`ZZ_Nj`) AS `V_Nu`,`v_TT`.`pi_N` * sum(`v_TT`.`ZZ_Nj`) / 100.0 AS `L_Nu`,sum(`v_TT`.`ppi_Nj`) - `v_TT`.`pi_N` * sum(`v_TT`.`ZZ_Nj`) / 100.0 AS `P_Nu`,sum(`v_TT`.`TTCnt`) AS `UCnt` from `v_TT` group by `v_TT`.`N`,`v_TT`.`Odd`,case when `v_TT`.`j` & 0x01 then 1 else 0 end,`v_TT`.`pi_N` ;

-- --------------------------------------------------------

--
-- Structure for view `v_worker_merit_current_work`
--
DROP TABLE IF EXISTS `v_worker_merit_current_work`;

CREATE ALGORITHM=UNDEFINED DEFINER=`id16232074_main`@`%` SQL SECURITY DEFINER VIEW `v_worker_merit_current_work`  AS  with A1 as (select `t`.`task_id` AS `task_id`,`t`.`t_description` AS `t_description`,max(`wm`.`power2_id`) AS `power2` from (`worker_merit` `wm` join `tasks` `t` on(`t`.`task_id` = `wm`.`task_id`)) where `t`.`t_current` = 1)select `A1`.`t_description` AS `t_description`,`wm`.`power2_id` AS `power2_id`,`u`.`u_mail` AS `u_mail`,`u`.`u_name` AS `u_name`,`w`.`w_name` AS `w_name`,`wm`.`wm_merit` AS `wm_merit` from (((`worker_merit` `wm` join `worker` `w` on(`w`.`worker_id` = `wm`.`worker_id`)) join `user` `u` on(`u`.`user_id` = `w`.`user_id`)) join `A1` on(`A1`.`task_id` = `wm`.`task_id` and `A1`.`power2` = `wm`.`power2_id`)) ;

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
-- Constraints for table `worker_merit`
--
ALTER TABLE `worker_merit`
  ADD CONSTRAINT `work_merit_power2_id` FOREIGN KEY (`power2_id`) REFERENCES `power2` (`power2_id`),
  ADD CONSTRAINT `work_merit_task_id` FOREIGN KEY (`task_id`) REFERENCES `tasks` (`task_id`),
  ADD CONSTRAINT `work_merit_worker_id` FOREIGN KEY (`worker_id`) REFERENCES `worker` (`worker_id`);

--
-- Constraints for table `work_results`
--
ALTER TABLE `work_results`
  ADD CONSTRAINT `work_results_task_id` FOREIGN KEY (`task_id`) REFERENCES `tasks` (`task_id`);
COMMIT;
