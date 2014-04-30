# useful commands for bash 

# connect to DB on PLX
mysql -u aborghes -h login1.plx.cineca.it -P 33022 -D EuroraMeasurements -paborghes123

# create tunnel on PLX
ssh -L login1.plx.cineca.it:33022:rvn06:3302 aborghes@rvn06

# detach script from bash
nohup python nomefile.py &

# stop running job
Ctrl-z

# put job background
bg job_number #(in jobs)

# detach running job from terminal
disown %job_number

# to change const.py file - and apply the change tu all versions
# copy one file to multiple destinations
echo loadDB/gpu/ loadDB/cpu/ loadDB/core/ loadDB/mic/ loadDB/jobs/ | xargs -n 1 cp const.py
echo newLoadDB/gpu/ newLoadDB/cpu/ newLoadDB/core/ newLoadDB/mic/ newLoadDB/jobs/ | xargs -n 1 cp const.py

# remote copy files from Eurora to micrelSCC (or anywhere else or vice-versa)
scp aborghes@login.eurora.cineca.it:~/jobs/data_log/jobs_cleansed.log jobs.log

# append file2 content to file1
cat file2 >> file1

# delete content of a file
> filename

# extract multiple files in tgz format
cat *.tgz | tar -zxvf - -i

# delete first N lines from file
sed -i '1,Nd' myfile


# load DB daemon
cd /database/Cineca/Eurora/data/download/
cat *.tgz | tar -zxvf - -i
mv database/Cineca/Eurora/data/download/* . && rm -r database/
/home0/euroralocal/aborghesi/parser/filterHeader.sh /database/Cineca/Eurora/data/download/
/home0/euroralocal/aborghesi/loadDB_daemon/load_data.sh
/home0/euroralocal/aborghesi/loadDB_daemon/jobs/load_jobs.py
rm *.trace && rm *.log && rm *.head && rm *.val 

# drop all tables which start with 'Whatever'
mysql -u matlab -D matlabDB -e "show tables" -s -pM@tl4b | egrep "^Whatever_" | xargs -I "@@" mysql -u matlab -D matlabDB -e "DROP TABLE @@" -pM@tl4b

# write in file3 the lines which are in file1 but not in file2 
diff --changed-group-format='%<' --unchanged-group-format='' file1 file2 > file3

# copy file1 in file2 deleting duplicate lines
awk '!seen[$0]++' file1 >> file2

# delete blank lines from file1 and then insert a blank line every text line
grep -v '^$' file1 > file2 && sed G file2 > file3 && mv file3 file1

cat test > test3 && cat test3 >> test

# sort a file based on the numeric values of the first column only
sort -s -n -k 1,1 unsorted_file > file_tmp && mv file_tmp unsorted_file

# check if tunnel PLX is already active --> if it returns a number greater than 1 the tunnel is only
ps aux | grep 33022 | wc -l

# file with cron downloadLog.sh output
/var/mail/aborghes

# append script times and delete old vals
grep 'Starting\|ends' /var/mail/aborghes >> times.log
> /var/mail/aborghes

# kill all load db lingering (various reasons/problems) processes
kill -9 `pgrep -f  load_db`
