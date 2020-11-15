# Workloads

## Memcached
### Deploy Memcached

https://hub.docker.com/r/bitnami/memcached

```
docker pull bitnami/memcached:latest
docker run -d --name memcached -p 11211:11211 -e MEMCACHED_CACHE_SIZE=128 bitnami/memcached:latest
```

Other configurations:
- `MEMCACHED_CACHE_SIZE`: default 64 (MB)
- `MEMCACHED_MAX_CONNECTIONS`: default 1024
- `MEMCACHED_THREADS`: default 4

### Run Memcached Benchmarks

#### `memtier_benchmark`

https://github.com/RedisLabs/memtier_benchmark

```
memtier_benchmark -p 11211 -P memcache_binary -R --key-pattern=S:S --ratio=1:1 --hide-histogram --print-percentiles=50,90,99,99.9
```

For other options and configurations see `memtier_benchmark --help`.`

- `--key-pattern`: `S:S` for sequential, giving you high hit-rate if set-get ratio is balanced as well.
- `--ratio`: ratio of set:get, default `1:10`
- `--hide-histogram`: hide detailed histogram stats for GET and SET
- `-R`: random data
- `--print-percentiles`: set the percentiles to print at the end
- `-c`: number of connections per client

## Nginx

https://www.digitalocean.com/community/tutorials/how-to-run-nginx-in-a-docker-container-on-ubuntu-14-04

```
docker pull nginx

# default
docker run -d --name docker-nginx -p 80:80 nginx

# use your own static HTML file
mkdir -p ~/docker-nginx/html
# edit ~/docker-nginx/html/index.html
docker run -d --name docker-nginx -p 80:80 -v ~/cs523/docker-nginx/html:/usr/share/nginx/html nginx

# use your own nginx config file
# check the default config file used in the container
docker cp docker-nginx:/etc/nginx/conf.d/default.conf docker-nginx/default.conf
# delete the container
docker stop docker-nginx
docker rm docker-nginx
# edit ~/docker-nginx/default.conf
docker run -d --name docker-nginx -p 80:80 -v ~/cs523/docker-nginx/html:/usr/share/nginx/html -v ~/cs523/docker-nginx/default.conf:/etc/nginx/conf.d/default.conf nginx
# restart the docker container every time after you make changes to the config file
```

Serving random web pages from a directory: https://subscription.packtpub.com/book/networking_and_servers/9781849514965/1/ch01lvl1sec21/serving-any-random-web-page-from-a-directory

### Benchmark - wrk2

https://github.com/giltene/wrk2

```
make
./wrk -t2 -c100 -d30s -R2000 http://localhost:80
```

- `-t`: total number of threads
- `-c`: total number of connections
- `-d`: duration
- `-R`: rate (# of req/s, total, across all connections)

## MongoDB

https://phoenixnap.com/kb/docker-mongodb

```
docker pull mongo
docker run -v ~/cs523/mongodata:/data/db -p 27017:27017 --name mongodb -d mongo

# check the logs
docker logs mongodb
# start interactive docker terminal to manage MongoDB database
docker exec -it mongodb bash
mongo
```

### Benchmark - YCSB

https://benchmark-docs.readthedocs.io/en/latest/benchmarks/mongodb-ycsb-ct720-jul2015.1.html
https://github.com/brianfrankcooper/YCSB/tree/master/mongodb

https://scalegrid.io/blog/how-to-benchmark-mongodb-with-ycsb/

Make sure `javac`, `mvn`, and `git` are available.

Set up YCSB:

```
git clone git://github.com/brianfrankcooper/YCSB.git
cd YCSB
mvn clean package
```

Run:

```
./bin/ycsb load mongodb -s -P workloads/workloada -p recordcount=100000 -threads 16 -p mongodb.url="mongodb://localhost:27017/dbname"

# synchronous driver
./bin/ycsb load mongodb -s -P workloads/workloada -p recordcount=100000 -threads 16 -p mongodb.url="mongodb://localhost:27017/ycsb?w=0"
# async driver
./bin/ycsb load mongodb-async -s -P workloads/workloada -p recordcount=100000 -threads 16 -p mongodb.url="mongodb://localhost:27017/ycsb?w=0"

# run phase (after load)
./bin/ycsb run mongodb -s -P workloads/workloada -p recordcount=100000 -threads 16 -p mongodb.url="mongodb://localhost:27017/ycsb?w=0"
```

- load: indicates this is a load run (loading phase)
- run: indicates this is the run phase
- s: indicates status is printed at 10 sec interval
- recordcount: record count to insert into the database (close to the number of ops you intend to run on it)
- threads: the number of client threads

https://github.com/brianfrankcooper/YCSB/wiki
