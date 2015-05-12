# hdcd_dawnsea
hdfs directory explorer

export CLASSPATH=`hadoop classpath`

export LD_LIBRARY_PATH=/app/hadoop-2.5.0-cdh5.3.0/lib/native:/app/jdk/jre/lib/amd64/server 


gcc -o hdcd 004.c -lhdfs -ljvm -lncurses -I/app/hdfs/include -L/app/hdfs/lib/native -L/app/jdk/jre/lib/amd64/server

 
