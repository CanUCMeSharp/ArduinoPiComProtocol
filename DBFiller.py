import redis

pool = redis.ConnectionPool(host='localhost', port=6379, db=0)
redis = redis.Redis(connection_pool=pool)

data = {'temp':50, 'wind':30}
for i in range(1000):
    redis.hset(i, mapping=data)